/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   messages.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 03:06:28 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/27 23:11:15 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

int	msg_sleeping(t_philo *ph)
{
	struct timeval	tv_start;
	struct timeval	tv;

	pthread_mutex_lock(ph->print_mutex);
	pthread_mutex_lock(ph->is_end_mutex);
	if (*ph->is_end != 0)
		return (rtn_n_and_unlock(NORMAL, ph->is_end_mutex, ph->print_mutex));
	if (gettimeofday(&tv_start, NULL) != 0)
		return (FAIL);
	printf("%ld %d is sleeping\n", tv_in_ms(tv_start), ph->my_index);
	rtn_n_and_unlock(CONTINUE, ph->is_end_mutex, ph->print_mutex);
	tv = tv_start;
	while (tv_in_ms(tv_start) + ph->args->time_to_sleep > tv_in_ms(tv))
	{
		if (gettimeofday(&tv, NULL) != 0)
			return (FAIL);
		usleep(100);
		pthread_mutex_lock(ph->is_end_mutex);
		if (*ph->is_end != 0)
			return (rtn_n_and_unlock(NORMAL, ph->is_end_mutex, NULL));
		pthread_mutex_unlock(ph->is_end_mutex);
	}
	return (CONTINUE);
}

int	msg_thinking(t_philo *ph)
{
	struct timeval	tv;

	pthread_mutex_lock(ph->print_mutex);
	pthread_mutex_lock(ph->is_end_mutex);
	if (*ph->is_end != 0)
		return (rtn_n_and_unlock(NORMAL, ph->is_end_mutex, ph->print_mutex));
	if (gettimeofday(&tv, NULL) != 0)
		return (FAIL);
	printf("%ld %d is thinking\n", tv_in_ms(tv), ph->my_index);
	rtn_n_and_unlock(CONTINUE, ph->is_end_mutex, ph->print_mutex);
	return (CONTINUE);
}
