/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/30 03:57:19 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/16 04:45:03 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

// struct timeval {
//     time_t tv_sec;            /* Seconds.  */
//     suseconds_t tv_usec;      /* Microseconds.  */
// } t_timeval;

long	tv_in_ms(struct timeval tv)
{
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	free_datas(t_data *data)
{
	free(data->th);
	free(data->rtn_status);
	free(data->mutex);
	free(data->latest_eat_tv);
	free(data->is_end);
}

// mutex destroyとfree datas
int	free_all_before_end(t_data *datas, int is_fail)
{
	int	i;

	i = 0;
	while (i < datas->args->nb_of_philos)
	{
		pthread_mutex_destroy(&datas->mutex[i]);
		i++;
	}
	free_datas(datas);
	free(datas);
	if (is_fail)
		return (1);
	return (0);
}

int	msg_take_fork(int i)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) != 0)
		return (1);
	printf("%ld %d has taken a fork\n", tv_in_ms(tv), i);
	return (0);
}

int	msg_eating(t_data *data)
{
	struct timeval	tv;
	long long		waiting_time;
	int				i;

	if (gettimeofday(&tv, NULL) != 0)
		return (1);
	*data->latest_eat_tv = tv;
	printf("%ld %d is eating\n", tv_in_ms(tv), data->my_index);
	waiting_time = 1000 * data->args->time_to_eat;
	// printf("wating_time : %lld (microsec)\n", waiting_time); //todo delete
	i = 0;
	while (*data->is_end == 0 && i < waiting_time)
	{
		usleep(10);
		i += 10;
	}
	data->nb_eat++;
	printf("nb_eat %d : %d \n", data->my_index, data->nb_eat); // todo delete
	return (0);
}

int	msg_sleeping(t_data *data)
{
	struct timeval	tv;
	long long		waiting_time;
	int				i;

	if (gettimeofday(&tv, NULL) != 0)
		return (1);
	printf("%ld %d is sleeping\n", tv_in_ms(tv), data->my_index);
	waiting_time = 1000 * data->args->time_to_sleep;
	i = 0;
	while (*data->is_end == 0 && i < waiting_time)
	{
		usleep(10);
		i += 10;
	}
	return (0);
}

int	msg_thinking(t_data *data)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) != 0)
		return (1);
	printf("%ld %d is thinking\n", tv_in_ms(tv), data->my_index);
	return (0);
}

// todo fix
// TODO datas受け取ってるので、失敗したときは内部でall_freeしてよさそう
// →pthread_joinでstatusうけとったとき1だったらそこでall_freeしてる。
void	*check_end(void *v_datas)
{
	int				i;
	struct timeval	tv;
	t_data			*datas;

	printf("監視用スレッド起動\n");
	datas = (t_data *)v_datas;
	while (*datas->is_end == 0)
	{
		i = 0;
		while (++i <= datas->args->nb_of_philos)
		{
			if (gettimeofday(&tv, NULL) != 0)
				return ((void *)1);
			if (tv_in_ms(datas->latest_eat_tv[i]) + datas->args->time_to_die <= tv_in_ms(tv))
			{
				*datas->is_end = 1;
				printf("%ld %d died\n", tv_in_ms(tv), i);
				return (NULL);
			}
		}
		i = 0;
		while (++i <= datas->args->nb_of_philos)
		{
			if (datas[i].nb_eat < datas->args->nb_of_times_each_philo_must_eat)
				break ;
		}
		if (i > datas->args->nb_of_philos)
		{
			printf("all philos ate enough\n");
			*datas->is_end = 1;
		}
	}
	printf("監視用スレッド終了\n");
	return (NULL);
}

void	*start_routine(void *v_data)
{
	pthread_mutex_t	*fork_r;
	pthread_mutex_t	*fork_l;
	t_data			*data;

	// pthread_mutex_t	latest_eat_time;
	// struct timeval	tv;
	data = (t_data *)v_data;
	fork_r = &data->mutex[data->my_index - 1];
	fork_l = &data->mutex[data->my_index % data->args->nb_of_philos];
	// printf("%d fork_r : %d\n", data->my_index, data->my_index - 1);
	// printf("%d fork_l : %d\n", data->my_index, data->my_index % data->args->nb_of_philos);
	printf("before while %d : \n", data->my_index); //todo delete
	printf("is_end : %d\n", *data->is_end); //todo delete
	while (*data->is_end == 0)
	{	
		printf("start_routine %d : \n", data->my_index); //todo delete
		if (data->my_index % 2 == 1)
			usleep(200);
		pthread_mutex_lock(fork_r);
		if (*data->is_end == 1)
			break ;
		if (msg_take_fork(data->my_index))
		{
			printf("---- ここ?1 ----\n");
			return ((void *)1);
		}
		pthread_mutex_lock(fork_l);
		if (*data->is_end == 1)
			break ;
		if (msg_take_fork(data->my_index))
		{
			printf("---- ここ?2 ----\n");
			return ((void *)1);
		}
		if (msg_eating(data))
		{
			printf("---- ここ?3 ----\n");
			return ((void *)1);
		}
		if (*data->is_end == 1)
			break ;
		pthread_mutex_unlock(fork_r);
		pthread_mutex_unlock(fork_l);
		if (msg_sleeping(data))
		{
			printf("---- ここ?4 ----\n");
			return ((void *)1);
		}
		if (*data->is_end == 1)
			break ;
		msg_thinking(data);
	}
	return (NULL);
}

void	set_args(int argc, char *argv[], t_args *args)
{
	if (argv[1] != NULL)
		args->nb_of_philos = ft_atoi(argv[1]);
	if (argv[2] != NULL)
		args->time_to_die = ft_atoi(argv[2]);
	if (argv[3] != NULL)
		args->time_to_eat = ft_atoi(argv[3]);
	if (argv[4] != NULL)
		args->time_to_sleep = ft_atoi(argv[4]);
	if (argc == 6)
		args->nb_of_times_each_philo_must_eat = ft_atoi(argv[5]);
	else
		args->nb_of_times_each_philo_must_eat = INT_MAX;
}

// th[0] は終了条件監視用
// th[i] は i 番目の哲学者のスレッド
// mutex[i-1] は i 番目の哲学者の右のフォーク
// mutex[i] は i 番目の哲学者の左のフォーク
// latest_eat_tv[i] は i 番目の哲学者が最後に食事をしはじめた時間
// is_end は 0 なら生存中、1 なら死亡or全員規定の数の食事後
// nb_eat[i] は i 番目の哲学者が食事した回数

int	default_allocation(t_data *data)
{
	data->th = malloc(sizeof(pthread_t) * (data->args->nb_of_philos + 1));
	data->rtn_status = malloc(sizeof(void *) * (data->args->nb_of_philos + 1));
	data->mutex = malloc(sizeof(pthread_mutex_t) * (data->args->nb_of_philos));
	data->latest_eat_tv = malloc(sizeof(struct timeval)
			* (data->args->nb_of_philos + 1));
	data->is_end = malloc(sizeof(int));
	if (data->th == NULL || data->mutex == NULL || data->latest_eat_tv == NULL || data->is_end == NULL || data->rtn_status == NULL)
	{
		free_datas(data);
		return (1);
	}
	return (0);
}

int	init_datas(t_data *data)
{
	int	i;

	if (default_allocation(data) != 0)
		return (1);
	printf("default_allocation OK\n");
	*data->is_end = 0;
	i = 0;
	while (i < data->args->nb_of_philos)
	{
		pthread_mutex_init(&data->mutex[i], NULL);
		i++;
	}
	// printf("mutex init OK\n");
	return (0);
}

// datas[i].mutex に &(data->mutex)[i]していないのは、nb_of_philosの左のフォークを指定するのに0番目のmutexを取得するのが面倒だから
void	set_each_philo(t_data *datas, t_args *args)
{
	int	i;

	i = 0;
	while (++i <= args->nb_of_philos)
	{
		datas[i].args = args;
		datas[i].th = &datas->th[i];
		datas[i].rtn_status = &datas->rtn_status[i];
		datas[i].mutex = datas->mutex;
		datas[i].latest_eat_tv = &datas->latest_eat_tv[i];
		datas[i].is_end = datas->is_end;
		datas[i].nb_eat = 0; //&(datas->nb_eat)[i];
		datas[i].my_index = i;
	}
}

void set_default_latest_eat_tv(t_data *datas, struct timeval *tv)
{
	int	i;

	i = 0;
	while (++i <= datas->args->nb_of_philos)
	{
		datas->latest_eat_tv[i] = *tv;
		printf("latest_eat_tv[%d] = %ld\n", i, tv_in_ms(datas->latest_eat_tv[i]));// todo delete
	}
}

__attribute__((destructor)) static void destructor()
{
	system("leaks -q philo");
}

// argv
// 	number_of_philosophers
// 	time_to_die
// 	time_to_eat
// 	time_to_sleep
// 	(option) number_of_times_each_philosopher_must_eat
int	main(int argc, char *argv[])
{
	int		i;
	t_data	*datas;
	t_args	args;

	struct timeval tv_start; // todo あとで消す
	struct timeval tv_end;   // todo あとで消す
	if (argc < 5 || argc > 6)
		return (1);
	set_args(argc, argv, &args);
	printf("nb_of_philos : %d\n", args.nb_of_philos);
	printf("time_to_die : %d\n", args.time_to_die);
	printf("time_to_eat : %d\n", args.time_to_eat);
	printf("time_to_sleep : %d\n", args.time_to_sleep);
	printf("nb_of_times_each_philo_must_eat : %d\n",
			args.nb_of_times_each_philo_must_eat);
	datas = malloc(sizeof(t_data) * (args.nb_of_philos + 1));
	datas->args = &args;
	if (datas == NULL)
		return (1);
	if (init_datas(datas) != 0)
		return (1);
	set_each_philo(datas, &args);
	if (gettimeofday(&tv_start, NULL) != 0)
		return (free_all_before_end(datas, FAIL));
	printf("start = %ld\n", tv_in_ms(tv_start));
	set_default_latest_eat_tv(datas, &tv_start); // 仮


	if (pthread_create(datas->th, NULL, check_end, datas) != 0)
		return (free_all_before_end(datas, FAIL));
	i = 0;
	while (++i <= args.nb_of_philos)
	{
		printf("---- datas[%d].is_end : %d\n",i, *datas[i].is_end);
	}
	
	i = 0;
	while (++i <= args.nb_of_philos)
	{
		datas[i].my_index = i;
		if (pthread_create(datas[i].th, NULL, start_routine, &datas[i]) != 0)
		{
			// printf("pthread_create error [%d]: \n", i);
			perror("pthread_create error: ");
			return (free_all_before_end(datas, FAIL));
		}
		else
		{
			// printf("pthread_create OK [%d]: \n", i);
		}
	}
	i = 0;
	while (++i <= args.nb_of_philos)
	{
		if (pthread_join(*(datas[i].th), datas[i].rtn_status) != 0)
		{
			// printf("pthread_join error [%d]: \n", i);
			perror("pthread_join error: ");
			return (free_all_before_end(datas, FAIL));
		}
		printf("th[%d] return : %lu\n", i, (uintptr_t)((void **)datas->rtn_status)[i]);
		if (&(datas->rtn_status)[i] != NULL)
		{
			// free(&(datas->rtn_status)[i]);
			if (&datas->rtn_status[i] == (void *)1)
				return (free_all_before_end(datas, FAIL));
		}
	}
	if (pthread_join(*datas->th, datas->rtn_status) != 0)
	{
		// printf("pthread_join error [0]: \n");
		perror("pthread_join error: ");
		return (free_all_before_end(datas, FAIL));
	}
	printf("  end routine\n");
	if (gettimeofday(&tv_end, NULL) != 0)
		return (1);
	// printf("time = %ld.%d\n", tv_end.tv_sec, tv_end.tv_usec);
	printf("time   end = %ld\n", tv_in_ms(tv_end));
	printf("time  diff = %ld\n", tv_in_ms(tv_end) - tv_in_ms(tv_start));
	return (free_all_before_end(datas, NORMAL));
}
