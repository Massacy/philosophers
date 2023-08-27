/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   eat_process.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/27 19:36:56 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/27 23:10:35 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

int	msg_take_fork(t_philo *ph)
{
	struct timeval	tv;

	pthread_mutex_lock(ph->print_mutex);
	pthread_mutex_lock(ph->is_end_mutex);
	if (*ph->is_end != 0)
		return (rtn_n_and_unlock(NORMAL, ph->is_end_mutex, ph->print_mutex));
	if (gettimeofday(&tv, NULL) != 0)
		return (FAIL);
	printf("%ld %d has taken a fork\n", tv_in_ms(tv), ph->my_index);
	rtn_n_and_unlock(CONTINUE, ph->is_end_mutex, ph->print_mutex);
	return (CONTINUE);
}

static void	increment_eat_nb(t_philo *ph)
{
	pthread_mutex_lock(ph->eat_nb_mutex);
	ph->eat_nb++;
	pthread_mutex_unlock(ph->eat_nb_mutex);
}

int	msg_eating(t_philo *ph)
{
	struct timeval	tv;

	pthread_mutex_lock(ph->print_mutex);
	pthread_mutex_lock(ph->is_end_mutex);
	if (*ph->is_end != 0)
		return (rtn_n_and_unlock(NORMAL, ph->is_end_mutex, ph->print_mutex));
	if (gettimeofday(&tv, NULL) != 0)
		return (FAIL);
	pthread_mutex_lock(ph->eat_tv_mutex);
	*ph->latest_eat_tv = tv;
	pthread_mutex_unlock(ph->eat_tv_mutex);
	printf("%ld %d is eating\n", tv_in_ms(tv), ph->my_index);
	increment_eat_nb(ph);
	rtn_n_and_unlock(CONTINUE, ph->is_end_mutex, ph->print_mutex);
	while (1)
	{
		pthread_mutex_lock(ph->eat_tv_mutex);
		if (gettimeofday(&tv, NULL) != 0)
			return (FAIL);
		if (tv_in_ms(*ph->latest_eat_tv)
			+ ph->args->time_to_eat <= tv_in_ms(tv))
			return (rtn_n_and_unlock(CONTINUE, ph->eat_tv_mutex, NULL));
		pthread_mutex_unlock(ph->eat_tv_mutex);
		usleep(100);
		pthread_mutex_lock(ph->is_end_mutex);
		if (*ph->is_end != 0)
			return (rtn_n_and_unlock(NORMAL, ph->is_end_mutex, NULL));
		pthread_mutex_unlock(ph->is_end_mutex);
	}
}

static int	can_eat_or_end(t_philo *ph)
{
	pthread_mutex_lock(ph->eat_priority_mutex);
	if (ph->eat_priority == 1)
		return (CONTINUE);
	pthread_mutex_unlock(ph->eat_priority_mutex);
	pthread_mutex_lock(ph->is_end_mutex);
	if (*ph->is_end != 0)
		return (rtn_n_and_unlock(NORMAL, ph->is_end_mutex, NULL));
	pthread_mutex_unlock(ph->is_end_mutex);
	return (LOOP);
}

int	eat_process(t_philo *ph)
{
	int	rtn;

	while (can_eat_or_end(ph) == LOOP)
		;
	pthread_mutex_unlock(ph->eat_priority_mutex);
	pthread_mutex_lock(ph->fork_first);
	rtn = msg_take_fork(ph);
	if (rtn != CONTINUE)
		return (rtn_n_and_unlock(rtn, ph->fork_first, NULL));
	if (ph->fork_second == NULL)
		return (rtn_n_and_unlock(NORMAL, ph->fork_first, NULL));
	pthread_mutex_lock(ph->fork_second);
	rtn = msg_take_fork(ph);
	if (rtn != CONTINUE)
		return (rtn_n_and_unlock(rtn, ph->fork_first, ph->fork_second));
	rtn = msg_eating(ph);
	if (rtn != CONTINUE)
		return (rtn_n_and_unlock(rtn, ph->fork_first, ph->fork_second));
	pthread_mutex_unlock(ph->fork_first);
	pthread_mutex_unlock(ph->fork_second);
	return (CONTINUE);
}
