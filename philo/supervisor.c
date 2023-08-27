/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   supervisor.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 02:58:13 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/27 18:28:27 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

static int	process_died(t_sv *sv, struct timeval *tv, int i)
{
	t_philo	*ph;

	ph = sv->philo_head;
	pthread_mutex_unlock(ph[i].eat_tv_mutex);
	pthread_mutex_lock(sv->is_end_mutex);
	sv->is_end = 1;
	pthread_mutex_unlock(sv->is_end_mutex);
	// usleep(1000);
	pthread_mutex_lock(sv->print_mutex);
	printf("%ld %d died\n", tv_in_ms(*tv), i);
	pthread_mutex_unlock(sv->print_mutex);
	return (NORMAL);
}

static int	check_died(t_sv *sv)
{
	int				i;
	struct timeval	tv;
	t_philo			*ph;

	ph = sv->philo_head;
	i = 0;
	while (++i <= ph->args->nb_of_philos)
	{
		if (gettimeofday(&tv, NULL) != 0)
			return (FAIL);
		pthread_mutex_lock(ph[i].eat_tv_mutex);
		if (tv_in_ms(*(ph[i].latest_eat_tv))
			+ ph->args->time_to_die <= tv_in_ms(tv))
			return (process_died(sv, &tv, i));
		pthread_mutex_unlock(ph[i].eat_tv_mutex);
	}
	return (CONTINUE);
}

static int	check_ate_enough(t_sv *sv)
{
	int		i;
	t_philo	*ph;

	ph = sv->philo_head;
	i = 0;
	while (++i <= ph->args->nb_of_philos)
	{
		pthread_mutex_lock(ph[i].eat_nb_mutex);
		if (ph[i].eat_nb < ph->args->nb_of_times_each_philo_must_eat)
			return (rtn_n_and_unlock(CONTINUE, ph[i].eat_nb_mutex, NULL));
		pthread_mutex_unlock(ph[i].eat_nb_mutex);
	}
	pthread_mutex_lock(sv->is_end_mutex);
	sv->is_end = 1;
	pthread_mutex_unlock(sv->is_end_mutex);
	pthread_mutex_lock(sv->print_mutex);
	printf("all philos ate enough\n");
	pthread_mutex_unlock(sv->print_mutex);
	return (NORMAL);
}

void	*check_end(void *v_sv)
{
	t_sv	*sv;
	int		rtn;
	struct timeval	tv;

	sv = (t_sv *)v_sv;
	while (1)
	{
		rtn = check_died(sv);
		if (rtn == FAIL)
			return ((void *)1);
		else if (rtn == NORMAL)
			return (NULL);
		rtn = check_ate_enough(sv);
		if (rtn == NORMAL)
			return (NULL);
		gettimeofday(&tv, NULL);
		check_set_priority(sv);
	}
}
