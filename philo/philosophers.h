/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/30 03:56:02 by imasayos          #+#    #+#             */
/*   Updated: 2023/08/27 18:56:48 by imasayos         ###   ########.fr       */
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
int		msg_take_fork(t_philo *ph);
int		msg_eating(t_philo *ph);
int		msg_sleeping(t_philo *ph);
int		msg_thinking(t_philo *ph);

// philosopher.c

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
