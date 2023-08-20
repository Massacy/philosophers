/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initializer.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 03:01:29 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/20 22:55:24 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

static int	default_allocation(t_sv *sv, t_philo *philos)
{
	int n;
	
	n = philos->args->nb_of_philos;
	philos->th = malloc(sizeof(pthread_t) * (n + 1));
	philos->rtn_status = malloc(sizeof(void *) * (n + 1));
	philos->latest_eat_tv = malloc(sizeof(struct timeval) * (n + 1));
	sv->fork_mutex = malloc(sizeof(pthread_mutex_t) * (n + 1));
	sv->eat_tv_mutex = malloc(sizeof(pthread_mutex_t) * (n + 1));
	sv->eat_nb_mutex = malloc(sizeof(pthread_mutex_t) * (n + 1));
	if (philos->th == NULL || philos->rtn_status == NULL \
		|| sv->fork_mutex == NULL || sv->eat_tv_mutex == NULL \
		|| philos->latest_eat_tv == NULL || sv->eat_nb_mutex == NULL)
	{
		free_datas(sv);
		return (1);
	}
	return (0);
}

// 奇数番目は右手から、偶数番目は左手からフォークを取る。
static void	set_adrs_philo(t_sv *sv, t_philo *philos, t_args *args)
{
	int	i;

	i = 0;
	while (++i <= args->nb_of_philos)
	{
		philos[i].args = args;
		philos[i].th = &philos->th[i];
		philos[i].rtn_status = &philos->rtn_status[i];
		philos[i].latest_eat_tv = &philos->latest_eat_tv[i];
		philos[i].eat_nb_mutex = &philos->eat_nb_mutex[i];
		philos[i].eat_tv_mutex = &philos->eat_tv_mutex[i];
		philos[i].is_end = &sv->is_end;
		if (i % 2 == 1)
		{
			philos[i].fork_first = &sv->fork_mutex[i - 1];
			philos[i].fork_second = &sv->fork_mutex[i % args->nb_of_philos];
			// if (i == 1 && args->nb_of_philos == 1)
				// philos[i].fork_second = NULL;
		}
		else
		{
			philos[i].fork_second = &sv->fork_mutex[i - 1];
			philos[i].fork_first = &sv->fork_mutex[i % args->nb_of_philos];
		}
	}
}


static void set_default_supervisor(t_sv *sv)
{
	sv->is_end = 0;
}

static int set_default_philos(t_sv *sv)
{
	int n;
	int i;
	struct timeval	tv_start;

	n = sv->philo_head->args->nb_of_philos;
	i = 0;
	while (++i <= n)
	{
		sv->philo_head[i].my_index = i;
		sv->philo_head[i].eat_nb = 0;
	}
	if (gettimeofday(&tv_start, NULL) != 0)
		return (free_all_before_end(sv, FAIL));
	i = 0;
	while (++i <= n)
		*(sv->philo_head[i].latest_eat_tv) = tv_start;
	return (0);
}

int	init_datas(t_sv *sv, t_args *args)
{
	int				i;
	struct timeval	tv_start;

	t_philo *philos;

	philos = malloc(sizeof(t_philo) * (args->nb_of_philos + 1));
	if (philos == NULL)
		return (1);
	philos->args = args;
	sv->philo_head = philos;
	if (default_allocation(sv, philos) != 0)
		return (1);
	if (init_mutexes(sv, args->nb_of_philos) != 0)
	{
		free_datas(sv);
		return (1);
	}
	set_adrs_philo(sv, philos, args);
	set_default_supervisor(sv);
	if(set_default_philos(sv) != 0)
		return (1);
	return (0);
}
