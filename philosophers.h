/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/30 03:56:02 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/20 23:50:36 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

# include <limits.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

# define NORMAL 0
# define FAIL 1
# define CONTINUE -1

typedef struct s_args
{
	int				nb_of_philos;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				nb_of_times_each_philo_must_eat;

}					t_args;

typedef struct s_data
{
	t_args			*args;

	pthread_t		*th;
	void			*rtn_status;
	struct timeval	*latest_eat_tv;

	pthread_mutex_t	*eat_tv_mutex;
	pthread_mutex_t	*fork_first;
	pthread_mutex_t	*fork_second;

	pthread_mutex_t	*is_end_mutex;
	int				*is_end;
	pthread_mutex_t	*eat_nb_mutex;
	int				eat_nb;
	int				my_index;

	// pthread_mutex_t *
	// pthread_mutex_t *
}					t_philo;

typedef struct s_supervisor
{
	t_philo			*philo_head;

	pthread_t		th;
	void			*rtn_status;
	int				is_end;
	pthread_mutex_t	is_end_mutex;

	pthread_mutex_t *fork_mutex;   //free
	pthread_mutex_t *eat_tv_mutex; //free
	pthread_mutex_t *eat_nb_mutex; //free

}					t_sv;

// typedef struct s_all_head
// {
// 	t_args			*args;

// 	pthread_t		*th;
// 	void			*rtn_status;

// 	pthread_mutex_t	*fork_head;
// 	int				*is_end;

// 	pthread_mutex_t *eat_tv_head_head;
// 	struct timeval *latest_eat_tv_head;
// } t_all_head;

int					ft_atoi(char *nptr);

// initializer.c
int	init_datas(t_sv *sv, t_args *args); //updated

// initializer2.c
void				set_args(int argc, char *argv[], t_args *args); //ok
int					init_mutexes(t_sv *sv, int n); // new

// supervisor.c
void				*check_end(void *v_datas);

// utils.c
long				tv_in_ms(struct timeval tv);
void	free_datas(t_sv *sv); // updated
int					free_all_before_end(t_sv *sv, int is_fail); //updated

// messages.c
int					msg_take_fork(t_data *data);
int					msg_eating(t_data *data);
int					msg_sleeping(t_data *data);
int					msg_thinking(t_data *data);

#endif

// 最初の5つはargsなので共有。
// 各dataごとにに1つのth(philoの行動)にする
// dataをN個分mallocして、th, mutexもまとめてmallocして、各dataにset, 対応indexもsetする。

// th[1 ~ N] のphilo thread
// mutex[0 ~ N-1] のfork用 mutex
// th[i]とth[i+1]が操作
// [1~N] のlatest_eat_tv
// th[i]のみが操作
// [1~N] のnb_eat (iが何回食べたか。N全員がnb_of_times_each_philo_must_eatを超えたら終了)
// th[i]のみが操作

// 監視用のthはth[0], th[args->nb_of_philos + 1]にする。
// th[0] 死亡監視用のthread
// th[N+1]が全員のnb_eatを監視。

// (1つを共有)is_end (継続は0。誰かが死んだら1にする。全員がnb_of_times_each_philo_must_eatを超えたら1にする。)
//	th[0]とth[N+1]が操作
// mutex[0] をis_endのmutexとして使う。
