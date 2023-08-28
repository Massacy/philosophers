/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   supervisor2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/27 16:48:53 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/29 04:29:44 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

static void	check_priority_1(t_philo *ph)
{
	pthread_mutex_lock(ph[1].eat_priority_mutex);
	ph[1].eat_priority = 1;
	pthread_mutex_unlock(ph[1].eat_priority_mutex);
}

static void	check_priority_2(t_philo *ph)
{
	int	i;
	int	n_l;

	i = 0;
	while (++i <= 2)
	{
		n_l = i % 2 + 1;
		pthread_mutex_lock(ph[n_l].eat_tv_mutex);
		pthread_mutex_lock(ph[i].eat_tv_mutex);
		pthread_mutex_lock(ph[i].eat_priority_mutex);
		if (tv_in_ms(*ph[i].latest_eat_tv) <= tv_in_ms(*ph[n_l].latest_eat_tv))
			ph[i].eat_priority = 1;
		else
			ph[i].eat_priority = 0;
		pthread_mutex_unlock(ph[i].eat_priority_mutex);
		pthread_mutex_unlock(ph[i].eat_tv_mutex);
		pthread_mutex_unlock(ph[n_l].eat_tv_mutex);
	}
}

static void	check_priority_3(t_philo *ph, int n)
{
	int	i;
	int	n_r;
	int	n_l;

	i = 0;
	while (++i <= n)
	{
		n_r = (i - 2 + n) % n + 1;
		n_l = i % n + 1;
		pthread_mutex_lock(ph[n_l].eat_tv_mutex);
		pthread_mutex_lock(ph[i].eat_tv_mutex);
		pthread_mutex_lock(ph[n_r].eat_tv_mutex);
		pthread_mutex_lock(ph[i].eat_priority_mutex);
		if (tv_in_us(*ph[i].latest_eat_tv) <= tv_in_us(*ph[n_r].latest_eat_tv) \
		&& tv_in_us(*ph[i].latest_eat_tv) <= tv_in_us(*ph[n_l].latest_eat_tv))
			ph[i].eat_priority = 1;
		else
			ph[i].eat_priority = 0;
		pthread_mutex_unlock(ph[i].eat_priority_mutex);
		pthread_mutex_unlock(ph[n_r].eat_tv_mutex);
		pthread_mutex_unlock(ph[i].eat_tv_mutex);
		pthread_mutex_unlock(ph[n_l].eat_tv_mutex);
	}
}

void	check_set_priority(t_sv *sv)
{
	int		n;
	t_philo	*ph;

	ph = sv->philo_head;
	n = sv->philo_head->args->nb_of_philos;
	if (n == 1)
		check_priority_1(ph);
	else if (n == 2)
		check_priority_2(ph);
	else
		check_priority_3(ph, n);
}
