/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initializer2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/20 22:49:46 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/22 17:50:32 by imasayos         ###   ########.fr       */
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

// th[i] は i 番目の哲学者のスレッド

// mutex[i-1] は i 番目の哲学者の右のフォーク
// mutex[i] は i 番目の哲学者の左のフォーク
// latest_eat_tv[i] は i 番目の哲学者が最後に食事をしはじめた時間
// is_end は 0 なら生存中、1 なら死亡or全員規定の数の食事後
// nb_eat[i] は i 番目の哲学者が食事した回数


int init_mutexes(t_sv *sv, int n)
{
	int i;

	i = 0;
	while (i <= 4*(n+1)+2)
	{
		if (pthread_mutex_init(&sv->all_mutex_head[i], NULL) != 0)
		{
			while (i>=0)
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
