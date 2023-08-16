/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 03:03:28 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/17 03:04:13 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

long	tv_in_ms(struct timeval tv)
{
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	free_datas(t_data *data)
{
	free(data->th);
	free(data->rtn_status);
	free(data->mutex);
	free(data->latest_eat_tv);
	free(data->is_end);
	free(data);
}

// mutex destroyとfree datas
int	free_all_before_end(t_data *datas, int is_fail)
{
	int	i;

	i = 0;
	while (i < datas->args->nb_of_philos)
	{
		pthread_mutex_destroy(&datas->mutex[i]);
		i++;
	}
	free_datas(datas);
	if (is_fail)
		return (1);
	return (0);
}
