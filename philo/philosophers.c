/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/30 03:57:19 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/27 20:28:04 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

static int	philo_routine(t_philo *ph)
{
	int	rtn;

	rtn = msg_thinking(ph);
	if (rtn != CONTINUE)
		return (rtn);
	rtn = eat_process(ph);
	if (rtn != CONTINUE)
		return (rtn);
	rtn = msg_sleeping(ph);
	if (rtn != CONTINUE)
		return (rtn);
	return (CONTINUE);
}

void	*start_routine(void *v_philo)
{
	t_philo	*ph;
	int		rtn;

	ph = (t_philo *)v_philo;
	while (1)
	{
		rtn = philo_routine(ph);
		if (rtn == FAIL)
			return ((void *)1);
		else if (rtn == NORMAL)
			return (NULL);
	}
}

static int	join_part(t_sv *sv)
{
	int	i;
	int	status;

	status = NORMAL;
	if (pthread_join(sv->th, (void **)&sv->rtn_status) != 0)
		status = FAIL;
	if (sv->rtn_status != NULL)
		status = FAIL;
	i = 0;
	while (++i <= sv->philo_head->args->nb_of_philos)
	{
		if (pthread_join(*(sv->philo_head[i].th),
				(void **)&sv->philo_head[i].rtn_status) != 0)
		{
			status = FAIL;
			continue ;
		}
		if (sv->philo_head[i].rtn_status != NULL)
			status = FAIL;
	}
	return (status);
}

int	run_threads(t_sv *sv)
{
	int	i;

	if (pthread_create(&sv->th, NULL, check_end, sv) != 0)
		return (free_all_before_end(sv, FAIL));
	i = 0;
	while (++i <= sv->philo_head->args->nb_of_philos)
	{
		if (pthread_create(sv->philo_head[i].th, NULL, start_routine,
				&sv->philo_head[i]) != 0)
			return (free_all_before_end(sv, FAIL));
	}
	if (join_part(sv) != 0)
		return (free_all_before_end(sv, FAIL));
	return (free_all_before_end(sv, NORMAL));
}

// __attribute__((destructor)) static void destructor()
// {
// 	system("leaks -q philo");
// }

// argv
// 	number_of_philosophers
// 	time_to_die
// 	time_to_eat
// 	time_to_sleep
// 	(option) number_of_times_each_philosopher_must_eat
int	main(int argc, char *argv[])
{
	t_args	args;
	t_sv	supervisor;

	if (argc < 5 || argc > 6)
		return (1);
	set_args(argc, argv, &args);
	if (args.nb_of_philos < 1 || args.time_to_die < 1 \
	|| args.time_to_eat < 1 || args.time_to_sleep < 1 \
	|| args.nb_of_times_each_philo_must_eat < 1)
		return (0);
	if (init_datas(&supervisor, &args) != 0)
		return (1);
	return (run_threads(&supervisor));
}
