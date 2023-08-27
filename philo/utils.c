/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 03:03:28 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/27 17:53:25 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

long	tv_in_ms(struct timeval tv)
{
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	free_datas(t_sv *sv)
{
	t_philo	*ps;

	ps = sv->philo_head;
	free(ps->th);
	free(ps->latest_eat_tv);
	free(sv->all_mutex_head);
	free(ps);
}

// mutex destroyとfree datas
int	free_all_before_end(t_sv *sv, int is_fail)
{
	int	i;
	int	n;

	n = sv->philo_head->args->nb_of_philos;
	i = 0;
	while (i <= 4 * (n + 1) + 2)
	{
		pthread_mutex_destroy(&sv->all_mutex_head[i]);
		i++;
	}
	free_datas(sv);
	if (is_fail)
		return (1);
	return (0);
}

int	rtn_n_and_unlock(int n, pthread_mutex_t *mu1, pthread_mutex_t *mu2)
{
	pthread_mutex_unlock(mu1);
	if (mu2 != NULL)
		pthread_mutex_unlock(mu2);
	return (n);
}
