/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/30 03:57:19 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/27 18:59:01 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

int	can_eat_or_end(t_philo *ph)
{
	pthread_mutex_lock(ph->eat_priority_mutex);
	if (ph->eat_priority == 1)
		return (CONTINUE);
	pthread_mutex_unlock(ph->eat_priority_mutex);
	pthread_mutex_lock(ph->is_end_mutex);
	if (*ph->is_end != 0)
		return (rtn_n_and_unlock(NORMAL, ph->is_end_mutex, NULL));
	pthread_mutex_unlock(ph->is_end_mutex);
	return (LOOP);
}

int	eat_process(t_philo *ph)
{
	int	rtn;

	while (can_eat_or_end(ph) == LOOP)
		;
	pthread_mutex_unlock(ph->eat_priority_mutex);
	pthread_mutex_lock(ph->fork_first);
	rtn = msg_take_fork(ph);
	if (rtn != CONTINUE)
		return (rtn_n_and_unlock(rtn, ph->fork_first, NULL));
	if (ph->fork_second == NULL)
		return (NORMAL);
	pthread_mutex_lock(ph->fork_second);
	rtn = msg_take_fork(ph);
	if (rtn != CONTINUE)
		return (rtn_n_and_unlock(rtn, ph->fork_first, ph->fork_second));
	rtn = msg_eating(ph);
	if (rtn != CONTINUE)
		return (rtn_n_and_unlock(rtn, ph->fork_first, ph->fork_second));
	pthread_mutex_unlock(ph->fork_first);
	pthread_mutex_unlock(ph->fork_second);
	return (CONTINUE);
}

int	philo_routine(t_philo *ph)
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

static int	is_rtn_status_1(void *rtn_status)
{
	if (rtn_status != NULL)
	{
		// printf("rtn val : %lu\n", (uintptr_t)(void **)rtn_status);
		// if (rtn_status == (void *)1)
		// printf("rtn_status address %p\n", rtn_status);
		// printf("rnt_status NULLじゃないとき : %d\n", *(int *)rtn_status);
		return (1);
	}
	return (0);
}

static int	join_part(t_sv *sv)
{
	int	i;
	int	status;

	status = NORMAL;
	if (pthread_join(sv->th, (void **)&sv->rtn_status) != 0)
		status = FAIL;
	if (is_rtn_status_1(sv->rtn_status) != 0)
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
		if (is_rtn_status_1(sv->philo_head[i].rtn_status) != 0)
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
	if (init_datas(&supervisor, &args) != 0)
		return (1);
	return (run_threads(&supervisor));
}
