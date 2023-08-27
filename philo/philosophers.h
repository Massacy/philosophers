/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/30 03:56:02 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/27 19:41:22 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

# include "philo_struct.h"

# define NORMAL 0
# define FAIL 1
# define CONTINUE -1
# define LOOP 2

int		ft_atoi(char *nptr);

// initializer.c
int		init_datas(t_sv *sv, t_args *args);
int		default_allocation(t_sv *sv, t_philo *philos);

// initializer2.c
void	set_args(int argc, char *argv[], t_args *args);
int		init_mutexes(t_sv *sv, int n);

// supervisor.c
void	*check_end(void *v_datas);

// supervisor2.c
void	check_set_priority(t_sv *sv);

// utils.c
long	tv_in_ms(struct timeval tv);
void	free_datas(t_sv *sv);
int		free_all_before_end(t_sv *sv, int is_fail);
int		rtn_n_and_unlock(int n, pthread_mutex_t *mu1, pthread_mutex_t *mu2);

// messages.c
int		msg_sleeping(t_philo *ph);
int		msg_thinking(t_philo *ph);

// eat_process.c
int		msg_take_fork(t_philo *ph);
int		msg_eating(t_philo *ph);
int		eat_process(t_philo *ph);

// philosopher.c

#endif
