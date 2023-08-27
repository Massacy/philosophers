/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initializer2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/20 22:49:46 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/27 19:19:49 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	set_args(int argc, char *argv[], t_args *args)
{
	if (argv[1] != NULL)
		args->nb_of_philos = ft_atoi(argv[1]);
	if (argv[2] != NULL)
		args->time_to_die = ft_atoi(argv[2]);
	if (argv[3] != NULL)
		args->time_to_eat = ft_atoi(argv[3]);
	if (argv[4] != NULL)
		args->time_to_sleep = ft_atoi(argv[4]);
	if (argc == 6)
		args->nb_of_times_each_philo_must_eat = ft_atoi(argv[5]);
	else
		args->nb_of_times_each_philo_must_eat = INT_MAX;
}

int	init_mutexes(t_sv *sv, int n)
{
	int	i;

	i = 0;
	while (i <= 4 * (n + 1) + 2)
	{
		if (pthread_mutex_init(&sv->all_mutex_head[i], NULL) != 0)
		{
			while (i >= 0)
			{
				pthread_mutex_destroy(&sv->all_mutex_head[i]);
				i--;
			}
			return (1);
		}
		i++;
	}
	return (0);
}

int	default_allocation(t_sv *sv, t_philo *philos)
{
	int	n;

	n = philos->args->nb_of_philos;
	philos->th = malloc(sizeof(pthread_t) * (n + 1));
	philos->latest_eat_tv = malloc(sizeof(struct timeval) * (n + 1));
	sv->all_mutex_head = malloc(sizeof(pthread_mutex_t) * 4 * (n + 1) + 2);
	if (philos->th == NULL || philos->latest_eat_tv == NULL \
	|| sv->all_mutex_head == NULL)
	{
		free_datas(sv);
		return (1);
	}
	return (0);
}
