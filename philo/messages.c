/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   messages.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 03:06:28 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/27 18:27:20 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

int	msg_take_fork(t_philo *ph)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) != 0)
		return (FAIL);
	pthread_mutex_lock(ph->print_mutex);

	pthread_mutex_lock(ph->is_end_mutex);
	if (*ph->is_end != 0)
		return (rtn_n_and_unlock(NORMAL, ph->is_end_mutex, ph->print_mutex));
	printf("%ld %d has taken a fork\n", tv_in_ms(tv), ph->my_index);
	rtn_n_and_unlock(CONTINUE, ph->is_end_mutex, ph->print_mutex);
	return (CONTINUE);
}

int increment_eat_nb(t_philo *ph)
{
	pthread_mutex_unlock(ph->eat_tv_mutex);
	pthread_mutex_lock(ph->eat_nb_mutex);
	ph->eat_nb++;
	pthread_mutex_unlock(ph->eat_nb_mutex);
	return (CONTINUE);
}

int	msg_eating(t_philo *ph)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) != 0)
		return (FAIL);
	pthread_mutex_lock(ph->eat_tv_mutex);
	*ph->latest_eat_tv = tv;
	pthread_mutex_unlock(ph->eat_tv_mutex);
	pthread_mutex_lock(ph->print_mutex);
	pthread_mutex_lock(ph->is_end_mutex);
	if (*ph->is_end != 0)
		return (rtn_n_and_unlock(NORMAL, ph->is_end_mutex, ph->print_mutex));
	printf("%ld %d is eating\n", tv_in_ms(tv), ph->my_index);
	rtn_n_and_unlock(CONTINUE, ph->is_end_mutex, ph->print_mutex);
	while (1)
	{
		pthread_mutex_lock(ph->eat_tv_mutex);
		if(tv_in_ms(*ph->latest_eat_tv)+ ph->args->time_to_eat <= tv_in_ms(tv))
			return (increment_eat_nb(ph));
		pthread_mutex_unlock(ph->eat_tv_mutex);
		if (gettimeofday(&tv, NULL) != 0)
			return (FAIL);	
	}
	return (CONTINUE);
}

int	msg_sleeping(t_philo *ph)
{
	struct timeval	tv_start;
	struct timeval	tv;

	if (gettimeofday(&tv_start, NULL) != 0)
		return (FAIL);
	tv = tv_start;

	pthread_mutex_lock(ph->print_mutex);
	pthread_mutex_lock(ph->is_end_mutex);
	if (*ph->is_end != 0)
		return (rtn_n_and_unlock(NORMAL, ph->is_end_mutex, ph->print_mutex));
	printf("%ld %d is sleeping\n", tv_in_ms(tv_start), ph->my_index);
	rtn_n_and_unlock(CONTINUE, ph->is_end_mutex, ph->print_mutex);
	while (tv_in_ms(tv_start) + ph->args->time_to_sleep > tv_in_ms(tv))
	{
		if (gettimeofday(&tv, NULL) != 0)
			return (FAIL);
	}
	return (CONTINUE);
}

int	msg_thinking(t_philo *ph)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) != 0)
		return (FAIL);
	pthread_mutex_lock(ph->print_mutex);
	pthread_mutex_lock(ph->is_end_mutex);
	if (*ph->is_end != 0)
		return (rtn_n_and_unlock(NORMAL, ph->is_end_mutex, ph->print_mutex));
	printf("%ld %d is thinking\n", tv_in_ms(tv), ph->my_index);
	rtn_n_and_unlock(CONTINUE, ph->is_end_mutex, ph->print_mutex);
	return (CONTINUE);
}
