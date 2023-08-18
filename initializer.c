/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initializer.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 03:01:29 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/18 15:36:29 by imasayos         ###   ########.fr       */
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

// th[0] は終了条件監視用
// th[i] は i 番目の哲学者のスレッド
// mutex[i-1] は i 番目の哲学者の右のフォーク
// mutex[i] は i 番目の哲学者の左のフォーク
// latest_eat_tv[i] は i 番目の哲学者が最後に食事をしはじめた時間
// is_end は 0 なら生存中、1 なら死亡or全員規定の数の食事後
// nb_eat[i] は i 番目の哲学者が食事した回数

static int	default_allocation(t_data *data)
{
	data->th = malloc(sizeof(pthread_t) * (data->args->nb_of_philos + 1));
	data->rtn_status = malloc(sizeof(void *) * (data->args->nb_of_philos + 1));
	data->mutex = malloc(sizeof(pthread_mutex_t) * (data->args->nb_of_philos));
	data->latest_eat_tv = malloc(sizeof(struct timeval)
			* (data->args->nb_of_philos + 1));
	data->is_end = malloc(sizeof(int));
	if (data->th == NULL || data->mutex == NULL || data->latest_eat_tv == NULL \
		|| data->is_end == NULL || data->rtn_status == NULL)
	{
		free_datas(data);
		return (1);
	}
	return (0);
}

// datas[i].mutex に &(data->mutex)[i]していないのは、
// nb_of_philosの左のフォークを指定するのに0番目からのほうが計算しやすいから
static void	set_each_philo(t_data *datas, t_args *args)
{
	int	i;

	*datas->is_end = 0;
	i = 0;
	while (++i <= args->nb_of_philos)
	{
		datas[i].args = args;
		datas[i].th = &datas->th[i];
		datas[i].rtn_status = &datas->rtn_status[i];
		datas[i].mutex = datas->mutex;
		datas[i].latest_eat_tv = &datas->latest_eat_tv[i];
		datas[i].is_end = datas->is_end;
		datas[i].nb_eat = 0;
		datas[i].my_index = i;
	}
}

static int	init_mutex(t_data *datas)
{
	int	i;

	i = 0;
	while (i < datas->args->nb_of_philos)
	{
		if (pthread_mutex_init(&datas->mutex[i], NULL) != 0)
		{
			while (i > 0)
			{
				pthread_mutex_destroy(&datas->mutex[i]);
				i--;
			}
			return (1);
		}
		i++;
	}
	return (0);
}

int	init_datas(t_data **datas, t_args *args)
{
	int				i;
	struct timeval	tv_start;

	*datas = malloc(sizeof(t_data) * (args->nb_of_philos + 1));
	if (*datas == NULL)
		return (1);
	(*datas)->args = args;
	if (default_allocation(*datas) != 0)
		return (1);
	set_each_philo(*datas, args);
	if (init_mutex(*datas) != 0)
		return (1);
	if (gettimeofday(&tv_start, NULL) != 0)
		return (free_all_before_end(*datas, FAIL));
	i = 0;
	while (++i <= (*datas)->args->nb_of_philos)
		(*datas)->latest_eat_tv[i] = tv_start;
	return (0);
}
