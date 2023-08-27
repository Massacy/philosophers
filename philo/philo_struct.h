/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_struct.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/27 17:54:44 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/27 19:31:38 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_STRUCT_H
# define PHILO_STRUCT_H

# include <limits.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_args
{
	int				nb_of_philos;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				nb_of_times_each_philo_must_eat;

}					t_args;

typedef struct s_philo
{
	t_args			*args;

	pthread_t		*th;
	int				rtn_status_int;
	int				*rtn_status;
	struct timeval	*latest_eat_tv;

	pthread_mutex_t	*eat_tv_mutex;
	pthread_mutex_t	*fork_first;
	pthread_mutex_t	*fork_second;

	pthread_mutex_t	*is_end_mutex;
	int				*is_end;
	pthread_mutex_t	*eat_nb_mutex;
	int				eat_nb;
	pthread_mutex_t	*print_mutex;
	int				eat_priority;
	pthread_mutex_t	*eat_priority_mutex;

	int				my_index;

}					t_philo;

typedef struct s_supervisor
{
	t_philo			*philo_head;

	pthread_t		th;
	int				rtn_status_int;
	int				*rtn_status;
	int				is_end;

	pthread_mutex_t	*is_end_mutex;
	pthread_mutex_t	*print_mutex;

	pthread_mutex_t	*fork_mutex;
	pthread_mutex_t	*eat_tv_mutex;
	pthread_mutex_t	*eat_nb_mutex;
	pthread_mutex_t	*eat_priority_mutex;

	pthread_mutex_t	*all_mutex_head;

}					t_sv;

#endif
