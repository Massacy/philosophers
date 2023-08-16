/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/30 03:57:19 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/17 03:21:23 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

// struct timeval {
//     time_t tv_sec;            /* Seconds.  */
//     suseconds_t tv_usec;      /* Microseconds.  */
// } t_timeval;

void	*start_routine(void *v_data)
{
	pthread_mutex_t	*fork_r;
	pthread_mutex_t	*fork_l;
	t_data			*data;

	data = (t_data *)v_data;
	fork_r = &data->mutex[data->my_index - 1];
	fork_l = &data->mutex[data->my_index % data->args->nb_of_philos];
	while (*data->is_end == 0)
	{
		if (data->my_index % 2 == 1)
			usleep(200);
		pthread_mutex_lock(fork_r);
		if (*data->is_end == 1)
		{
			pthread_mutex_unlock(fork_r);
			break ;
		}
		if (msg_take_fork(data->my_index))
			return ((void *)1);
		pthread_mutex_lock(fork_l);
		if (*data->is_end == 1)
		{
			pthread_mutex_unlock(fork_r);
			pthread_mutex_unlock(fork_l);
			break ;
		}
		if (msg_take_fork(data->my_index))
			return ((void *)1);
		if (msg_eating(data))
			return ((void *)1);
		// todo このあたりで切り分け。
		if (*data->is_end == 1)
		{
			pthread_mutex_unlock(fork_r);
			pthread_mutex_unlock(fork_l);
			break ;
		}
		pthread_mutex_unlock(fork_r);
		pthread_mutex_unlock(fork_l);
		if (msg_sleeping(data))
			return ((void *)1);
		if (*data->is_end == 1)
			break ;
		msg_thinking(data);
	}
	return (NULL);
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
	int				i;
	t_data			*datas;
	t_args			args;
	struct timeval	tv_start;

	if (argc < 5 || argc > 6)
		return (1);
	set_args(argc, argv, &args);
	if (init_datas(&datas, &args) != 0)
		return (1);
	if (gettimeofday(&tv_start, NULL) != 0)
		return (free_all_before_end(datas, FAIL));
	set_default_latest_eat_tv(datas, &tv_start);
	// todo group 2 切り出す
	if (pthread_create(datas->th, NULL, check_end, datas) != 0)
		return (free_all_before_end(datas, FAIL));
	i = 0;
	while (++i <= args.nb_of_philos)
	{
		datas[i].my_index = i;
		if (pthread_create(datas[i].th, NULL, start_routine, &datas[i]) != 0)
			return (free_all_before_end(datas, FAIL));
	}
	// todo group 3 切り出す
	i = -1;
	while (++i <= args.nb_of_philos)
	{
		if (pthread_join(*(datas[i].th), datas[i].rtn_status) != 0)
			return (free_all_before_end(datas, FAIL));
		if (&(datas->rtn_status)[i] != NULL)
		{
			if (&datas->rtn_status[i] == (void *)1)
				return (free_all_before_end(datas, FAIL));
		}
	}
	return (free_all_before_end(datas, NORMAL));
}
