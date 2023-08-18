/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   supervisor.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 02:58:13 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/18 15:37:12 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

static int	check_died(t_data *datas)
{
	int				i;
	struct timeval	tv;

	i = 0;
	while (++i <= datas->args->nb_of_philos)
	{
		if (gettimeofday(&tv, NULL) != 0)
			return (FAIL);
		if (tv_in_ms(datas->latest_eat_tv[i])
			+ datas->args->time_to_die <= tv_in_ms(tv))
		{
			*datas->is_end = 1;
			usleep(1000);
			printf("%ld %d died\n", tv_in_ms(tv), i);
			return (NORMAL);
		}
	}
	return (CONTINUE);
}

static int	check_ate_enough(t_data *datas)
{
	int	i;

	i = 0;
	while (++i <= datas->args->nb_of_philos)
	{
		if (datas[i].nb_eat < datas->args->nb_of_times_each_philo_must_eat)
			return (CONTINUE);
	}
	*datas->is_end = 1;
	usleep(1000);
	printf("all philos ate enough\n");
	return (NORMAL);
}

void	*check_end(void *v_datas)
{
	t_data			*datas;
	int				rtn;

	datas = (t_data *)v_datas;
	while (*datas->is_end == 0)
	{
		rtn = check_died(datas);
		if (rtn == FAIL)
			return ((void *)1);
		else if (rtn == NORMAL)
			return (NULL);
		rtn = check_ate_enough(datas);
		if (rtn == NORMAL)
			return (NULL);
	}
	return (NULL);
}
