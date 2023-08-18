/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/30 03:57:19 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/18 23:33:27 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

// struct timeval {
//     time_t tv_sec;            /* Seconds.  */
//     suseconds_t tv_usec;      /* Microseconds.  */
// } t_timeval;

int	check_rtn_end(t_data *data, pthread_mutex_t *fork_r,
		pthread_mutex_t *fork_l)
{
	if (*data->is_end == 1)
	{
		pthread_mutex_unlock(fork_r);
		if (fork_l != NULL)
			pthread_mutex_unlock(fork_l);
		return (NORMAL);
	}
	return (CONTINUE);
}

int	routine_loop(t_data *data, pthread_mutex_t *fork_r, pthread_mutex_t *fork_l)
{
	// think (take fork)
	// if (data->my_index % 2 == 1)
		// usleep(200);
	// 奇数、偶数で右利き左利き
	pthread_mutex_lock(fork_r);
	if (check_rtn_end(data, fork_r, NULL) == NORMAL) // この辺りのチェック方法も変わってくる。終了時unlockは必要
		return (NORMAL);
	if (msg_take_fork(data))
		return (FAIL);
	pthread_mutex_lock(fork_l);
	if (check_rtn_end(data, fork_r, fork_l) == NORMAL)
		return (NORMAL);
	if (msg_take_fork(data))
		return (FAIL);
	// eat 
	if (msg_eating(data))
		return (FAIL);
	if (check_rtn_end(data, fork_r, fork_l) == NORMAL)
		return (NORMAL);
	pthread_mutex_unlock(fork_r);
	pthread_mutex_unlock(fork_l);
	// sleep
	if (msg_sleeping(data))
		return (FAIL);
	if (*data->is_end == 1)
		return (NORMAL);
	msg_thinking(data);
	return (CONTINUE);
}

void	*start_routine(void *v_data)
{
	pthread_mutex_t	*fork_r;
	pthread_mutex_t	*fork_l;
	t_data			*data;
	int				rtn;

	data = (t_data *)v_data;	
	// 奇数、偶数で右利き左利きにする。
	fork_r = &data->mutex[data->my_index - 1];
	fork_l = &data->mutex[data->my_index % data->args->nb_of_philos];
	while (*data->is_end == 0)
	{
		rtn = routine_loop(data, fork_r, fork_l);
		if (rtn == FAIL)
			return ((void *)1);
		else if (rtn == NORMAL)
			return (NULL);
	}
	return (NULL);
}

int	run_threads(t_data *datas)
{
	int	i;

	if (pthread_create(datas->th, NULL, check_end, datas) != 0)
		return (free_all_before_end(datas, FAIL));
	i = 0;
	while (++i <= datas->args->nb_of_philos)
	{
		if (pthread_create(datas[i].th, NULL, start_routine, &datas[i]) != 0)
			return (free_all_before_end(datas, FAIL));
	}
	i = -1;
	while (++i <= datas->args->nb_of_philos)
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
	t_data	*datas;
	t_args	args;

	if (argc < 5 || argc > 6)
		return (1);
	set_args(argc, argv, &args);
	if (init_datas(&datas, &args) != 0)
		return (1);
	return (run_threads(datas));
}
