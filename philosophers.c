/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/30 03:57:19 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/13 23:53:03 by imasayos         ###   ########.fr       */
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
	while (i <= datas->args->nb_of_philos)
	{
		pthread_mutex_destroy(&datas->mutex[i]);
		i++;
	}
	free_datas(datas);
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
	data->latest_eat_tv[data->my_index] = tv;
	printf("%ld %d is eating\n", tv_in_ms(tv), data->my_index);
	waiting_time = 1000 * data->args->time_to_eat;
	printf("wating_time : %lld\n", waiting_time); //todo delete
	i = 0;
	while (*(data->is_end) == 0 && i < waiting_time)
	{
		usleep(10);
		i += 10;
	}
	printf("after eating\n"); //todo delete
	data->nb_eat++;
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
	while (data->is_end == 0 && i < waiting_time)
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
void	*check_death(void *v_datas)
{
	int				i;
	struct timeval	tv;
	t_data *datas;
	int *rtn;

	datas = (t_data *)v_datas;
	rtn = malloc(sizeof(int));

	while (datas->is_end == 0)
	{
		i = 0;
		while (i < datas->args->nb_of_philos)
		{
			if (gettimeofday(&tv, NULL) != 0)
			{
				*rtn = free_all_before_end(datas, FAIL);
				// return ((void *)rtn);
				return ((void *)1);
			}
			if (tv_in_ms(datas->latest_eat_tv[i])
				+ datas->args->time_to_die > tv_in_ms(tv))
			{
				pthread_mutex_lock(datas->mutex);
				*(datas->is_end) = 1;
				pthread_mutex_unlock(datas->mutex);
				printf("%ld %d died\n", tv_in_ms(tv), i);
			}
			i++;
		}
	}
	// *rtn = 0;
	return ((void *)0);
}

void	*check_eat_enough(void *v_datas)
{
	int	i;
	t_data *datas;

	datas = (t_data *)v_datas;
	while (datas->is_end == 0)
	{
		i = 0;
		while (++i <= datas->args->nb_of_philos)
		{
			if (datas[i].nb_eat < datas->args->nb_of_times_each_philo_must_eat)
				break ;
		}
		if (i == datas->args->nb_of_philos)
		{
			pthread_mutex_lock(datas->mutex);
			*datas->is_end = 1;
			pthread_mutex_unlock(datas->mutex);
		}
	}
	return (NULL); // ここの情報は特にいらない
}

void *rtn_int_ptr_1(void)
{
	int *rtn;

	rtn = malloc(sizeof(int));
	*rtn = 1;
	return ((void *)rtn);
}

void	*start_routine(void *v_data)
{
	pthread_mutex_t	*fork_r;
	pthread_mutex_t	*fork_l;
	// pthread_mutex_t	latest_eat_time;
	// struct timeval	tv;
	t_data *data;

	data = (t_data *)v_data;
	fork_r = &(data->mutex)[data->my_index];
	fork_l = &(data->mutex)[(data->my_index + 1) % data->args->nb_of_philos];
	while (data->is_end == 0)
	{
		if (data->my_index % 2 == 1)
			usleep(200);
		pthread_mutex_lock(fork_r);
		if(msg_take_fork(data->my_index))
			// return (rtn_int_ptr_1());
			return ((void *)1);
		pthread_mutex_lock(fork_l);
		if (msg_take_fork(data->my_index))
			return (rtn_int_ptr_1());
		if (msg_eating(data))
			return (rtn_int_ptr_1());
		if (*data->is_end == 1)
			break ;
		pthread_mutex_unlock(fork_r);
		pthread_mutex_unlock(fork_l);
		if (msg_sleeping(data))
			return (rtn_int_ptr_1());
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



// th[0] は死亡監視用に使って、
// th[nb_of_philos + 1]は　nb_of_times_each_philo_must_eat超過監視用
// th[i] は i 番目の哲学者のスレッド
// mutex[0] は is_end のlockに使う。
// mutex[i] は i 番目の哲学者の右のフォーク
// mutex[i + 1] は i 番目の哲学者の左のフォーク
// latest_eat_tv[i] は i 番目の哲学者が最後に食事をしはじめた時間
// is_end は 0 なら生存中、1 なら死亡or全員規定の数の食事後
// nb_eat[i] は i 番目の哲学者が食事した回数

int	default_allocation(t_data *data)
{
	printf("default_allocation\n");
	data->th = malloc(sizeof(pthread_t) * data->args->nb_of_philos + 2);
	data->rtn_status = malloc(sizeof(void *) * data->args->nb_of_philos + 2);
	data->mutex = malloc(sizeof(pthread_mutex_t) * (data->args->nb_of_philos
				+ 1));
	data->latest_eat_tv = malloc(sizeof(struct timeval)* data->args->nb_of_philos + 1);
	data->is_end = malloc(sizeof(int));
	// data->nb_eat = malloc(sizeof(int) * data->args->nb_of_philos+1);
	printf("malloc OK\n");
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
	*(data->is_end) = 0;
	i = 0;
	while (i <= data->args->nb_of_philos)
	{
		pthread_mutex_init(&data->mutex[i], NULL);
		i++;
	}
	printf("mutex init OK\n");
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
		datas[i].th = &(datas->th)[i];
		datas[i].rtn_status = &(datas->rtn_status)[i];
		// datas[i].mutex = &(datas->mutex)[i];
		datas[i].mutex = datas->mutex; // 
		datas[i].latest_eat_tv = &(datas->latest_eat_tv)[i];
		datas[i].is_end = datas->is_end;
		datas[i].nb_eat = 0; //&(datas->nb_eat)[i];
		datas[i].my_index = i;
	}
}

__attribute__((destructor)) static void destructor()
{
	system("leaks -q fdf");
}

// argv
// 	number_of_philosophers
// 	time_to_die
// 	time_to_eat
// 	time_to_sleep
// 	(option) number_of_times_each_philosopher_must_eat
int	main(int argc, char *argv[])
{
	struct timeval tv_start; // todo あとで消す
	struct timeval tv_end;   // todo あとで消す
	int i;
	t_data *datas;
	t_args args;

	if (argc < 5 || argc > 6)
		return (1);
	set_args(argc, argv, &args);
	printf("nb_of_philos : %d\n", args.nb_of_philos);
	printf("time_to_die : %d\n", args.time_to_die);
	printf("time_to_eat : %d\n", args.time_to_eat);
	printf("time_to_sleep : %d\n", args.time_to_sleep);
	printf("nb_of_times_each_philo_must_eat : %d\n", args.nb_of_times_each_philo_must_eat);
		
	datas = malloc(sizeof(t_data) * args.nb_of_philos + 1);
	datas->args = &args;
	if (datas == NULL)
		return (1);
	if (init_datas(datas) != 0)
		return (1);
	printf("1\n");
	set_each_philo(datas, &args);
	if (gettimeofday(&tv_start, NULL) != 0)
		return (free_all_before_end(datas, FAIL));

	printf("start = %ld\n", tv_in_ms(tv_start));

	if (pthread_create(datas->th, NULL, check_death, datas) != 0)
		return (free_all_before_end(datas, FAIL));
	if (pthread_create(&datas->th[args.nb_of_philos + 1], NULL,
			check_eat_enough, datas) != 0)
		return (free_all_before_end(datas, FAIL));
	i = 0;
	while (++i <= args.nb_of_philos)
	{
		datas[i].my_index = i;
		if (pthread_create(&datas->th[i], NULL, start_routine, &datas[i]) != 0)
			return (free_all_before_end(datas, FAIL));
		printf("start routine %d\n", i);
	}
	i = -1;
	while (++i <= args.nb_of_philos + 1)
	{
		if (pthread_join(datas->th[i], &(datas->rtn_status)[i]) != 0)
			return (free_all_before_end(datas, FAIL));
		printf("th[%d] return : %d\n", i, (((int *)(datas->rtn_status))[i]));
		if (&(datas->rtn_status)[i] != NULL)
		{
			free(&(datas->rtn_status)[i]);
			if (*(int *)&(datas->rtn_status)[i] == 1)
				return (free_all_before_end(datas, FAIL));
		}
	}
	printf("  end routine\n");
	// i = -1;
	// while (++i <= args.nb_of_philos + 1)
	// {
	// 	if ((datas->rtn_status)[i] != 0)
	// 	{
	// 		printf("rtn_status = %d\n", *(int *)datas->rtn_status[i]);
	// 		return (free_all_before_end(datas, FAIL));
	// 	}
	// }
	

	// i = 0;
	// while (i < args.nb_of_philos)
	// {
	// 	pthread_mutex_destroy(&datas->mutex[i]);
	// 	i++;
	// }

	if (gettimeofday(&tv_end, NULL) != 0)
		return (1);
	// printf("time = %ld.%d\n", tv_end.tv_sec, tv_end.tv_usec);
	printf("time   end = %ld\n", tv_in_ms(tv_end));
	printf("time  diff = %ld\n", tv_in_ms(tv_end) - tv_in_ms(tv_start));

	return (free_all_before_end(datas, NORMAL));
}
