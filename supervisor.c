/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   supervisor.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 02:58:13 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/21 15:54:59 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

static int	process_died(t_sv *sv, struct timeval *tv, int i)
{
	t_philo	*ph;

	ph = sv->philo_head;
	pthread_mutex_unlock(&ph->eat_tv_mutex[i]);
	pthread_mutex_lock(&sv->is_end_mutex);
	sv->is_end = 1;
	pthread_mutex_lock(&sv->is_end_mutex);
	// usleep(1000);
	pthread_mutex_lock(&sv->print_mutex);
	printf("%ld %d died\n", tv_in_ms(*tv), i);
	pthread_mutex_unlock(&sv->print_mutex);
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
		pthread_mutex_lock(&ph->eat_tv_mutex[i]);
		if (tv_in_ms(ph->latest_eat_tv[i])
			+ ph->args->time_to_die <= tv_in_ms(tv))
			return (process_died(sv, &tv, i));
		pthread_mutex_unlock(&ph->eat_tv_mutex[i]);
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
		pthread_mutex_lock(&ph->eat_nb_mutex[i]);
		if (ph[i].eat_nb < ph->args->nb_of_times_each_philo_must_eat)
		{
			pthread_mutex_unlock(&ph->eat_nb_mutex[i]);
			return (CONTINUE);
		}
		pthread_mutex_unlock(&ph->eat_nb_mutex[i]);
	}
	pthread_mutex_lock(&sv->is_end_mutex);
	sv->is_end = 1;
	pthread_mutex_unlock(&sv->is_end_mutex);
	// usleep(1000);
	pthread_mutex_lock(&sv->print_mutex);
	printf("all philos ate enough\n");
	pthread_mutex_unlock(&sv->print_mutex);
	return (NORMAL);
}

void	*check_end(void *v_sv)
{
	t_sv	*sv;
	int		rtn;

	sv = (t_sv *)v_sv;
	while (1)
	{
		pthread_mutex_lock(&sv->is_end_mutex);
		if (sv->is_end != 0)
			break ;
		pthread_mutex_unlock(&sv->is_end_mutex);
		rtn = check_died(sv);
		if (rtn == FAIL)
			return ((void *)1);
		else if (rtn == NORMAL)
			return (NULL);
		rtn = check_ate_enough(sv);
		if (rtn == NORMAL)
			return (NULL);
	}
	return (NULL);
}


// waiterの実装
// void *check_priority_eat()
// {

// }