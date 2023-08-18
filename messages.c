/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   messages.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 03:06:28 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/18 15:59:41 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

int	msg_take_fork(t_data *data)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) != 0)
		return (1);
	if (*data->is_end != 0)
		return (0);
	printf("%ld %d has taken a fork\n", tv_in_ms(tv), data->my_index);
	return (0);
}

int	msg_eating(t_data *data)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) != 0)
		return (1);
	*data->latest_eat_tv = tv;
	if (*data->is_end != 0)
		return (0);
	printf("%ld %d is eating\n", tv_in_ms(tv), data->my_index);
	while (*data->is_end == 0 && tv_in_ms(*data->latest_eat_tv)
		+ data->args->time_to_eat > tv_in_ms(tv))
	{
		if (gettimeofday(&tv, NULL) != 0)
			return (1);
	}
	data->nb_eat++;
	return (0);
}

int	msg_sleeping(t_data *data)
{
	struct timeval	tv_start;
	struct timeval	tv;

	if (gettimeofday(&tv_start, NULL) != 0)
		return (1);
	if (*data->is_end != 0)
		return (0);
	printf("%ld %d is sleeping\n", tv_in_ms(tv_start), data->my_index);
	tv = tv_start;
	while (*data->is_end == 0 && tv_in_ms(tv_start)
		+ data->args->time_to_sleep > tv_in_ms(tv))
	{
		if (gettimeofday(&tv, NULL) != 0)
			return (1);
	}
	return (0);
}

int	msg_thinking(t_data *data)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) != 0)
		return (1);
	if (*data->is_end != 0)
		return (0);
	printf("%ld %d is thinking\n", tv_in_ms(tv), data->my_index);
	return (0);
}
