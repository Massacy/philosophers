/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/30 03:57:19 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/20 23:46:18 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

// めも
// struct timeval {
//     time_t tv_sec;            /* Seconds.  */
//     suseconds_t tv_usec;      /* Microseconds.  */
// } t_timeval;


//
// int	check_rtn_end(t_data *data, pthread_mutex_t *fork_r,
// 		pthread_mutex_t *fork_l)
// {
// 	if (*data->is_end == 1)
// 	{
// 		pthread_mutex_unlock(fork_r);
// 		if (fork_l != NULL)
// 			pthread_mutex_unlock(fork_l);
// 		return (NORMAL);
// 	}
// 	return (CONTINUE);
// }

// int	routine_loop(t_data *data, pthread_mutex_t *fork_r, pthread_mutex_t *fork_l)
// {
// 	// think (take fork)
// 	// if (data->my_index % 2 == 1)
// 	// usleep(200);
// 	// 奇数、偶数で右利き左利き
// 	pthread_mutex_lock(fork_r);
// 	if (check_rtn_end(data, fork_r, NULL) == NORMAL)
// 	// この辺りのチェック方法も変わってくる。終了時unlockは必要
// 		return (NORMAL);
// 	if (msg_take_fork(data))
// 		return (FAIL);
// 	pthread_mutex_lock(fork_l);
// 	if (check_rtn_end(data, fork_r, fork_l) == NORMAL)
// 		return (NORMAL);
// 	if (msg_take_fork(data))
// 		return (FAIL);
// 	// eat
// 	if (msg_eating(data))
// 		return (FAIL);
// 	if (check_rtn_end(data, fork_r, fork_l) == NORMAL)
// 		return (NORMAL);
// 	pthread_mutex_unlock(fork_r);
// 	pthread_mutex_unlock(fork_l);
// 	// sleep
// 	if (msg_sleeping(data))
// 		return (FAIL);
// 	if (*data->is_end == 1)
// 		return (NORMAL);
// 	msg_thinking(data);
// 	return (CONTINUE);
// }

// void	*start_routine(void *v_data)
// {
// 	pthread_mutex_t	*fork_r;
// 	pthread_mutex_t	*fork_l;
// 	t_data			*data;
// 	int				rtn;

// 	data = (t_data *)v_data;
// 	// 奇数、偶数で右利き左利きにする。
// 	fork_r = &data->mutex[data->my_index - 1];
// 	fork_l = &data->mutex[data->my_index % data->args->nb_of_philos];
// 	while (*data->is_end == 0)
// 	{
// 		rtn = routine_loop(data, fork_r, fork_l);
// 		if (rtn == FAIL)
// 			return ((void *)1);
// 		else if (rtn == NORMAL)
// 			return (NULL);
// 	}
// 	return (NULL);
// }

// static int create_part(t_sv *sv)
// {

// }

static int	is_rtn_status_1(void *rtn_status)
{
	if (&rtn_status != NULL)
	{
		if (&rtn_status == (void *)1)
			return (1);
	}
	return (0);
}

static int	join_part(t_sv *sv)
{
	int	i;

	if (pthread_join(sv->th, sv->rtn_status) != 0)
		return (1);
	if (is_rtn_status_1(sv->rtn_status) != 0)
		return (1);
	// todo waiterのjoinいれるならここ。
	i = 0;
	while (++i <= sv->philo_head->args->nb_of_philos)
	{
		if (pthread_join(*(sv->philo_head[i].th),
				sv->philo_head[i].rtn_status) != 0)
			return (1);
		if (is_rtn_status_1(sv->philo_head[i].rtn_status) != 0)
			return (1);
	}
	return (0);
}

int	run_threads(t_sv *sv)
{
	int	i;

	if (pthread_create(&sv->th, NULL, check_end, sv) != 0)
		return (free_all_before_end(sv, FAIL));
	// todo waiter用意するならもう一つsupervisor必要。
	i = 0;
	while (++i <= sv->philo_head->args->nb_of_philos)
	{
		if (pthread_create(sv->philo_head[i].th, NULL, start_routine,
				&sv->philo_head[i]) != 0)
			return (free_all_before_end(sv, FAIL));
	}
	if (join_part != 0)
		return (free_all_before_end(sv, FAIL));
	return (free_all_before_end(sv, NORMAL));
}

// __attribute__((destructor)) static void destructor()
// {
// 	system("leaks -q philo");
// }

// argv
// 	number_of_philosophers
// 	time_to_die
// 	time_to_eat
// 	time_to_sleep
// 	(option) number_of_times_each_philosopher_must_eat
int	main(int argc, char *argv[])
{
	t_args	args;
	t_sv	supervisor;

	if (argc < 5 || argc > 6)
		return (1);
	set_args(argc, argv, &args);
	if (init_datas(&supervisor, &args) != 0)
		return (1);
	return (run_threads(&supervisor));
}
