/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/30 03:57:19 by imasayos          #+#    #+#             */
/*   Updated: 2023/07/30 12:00:40 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

// struct timeval {
//     time_t tv_sec;            /* Seconds.  */
//     suseconds_t tv_usec;      /* Microseconds.  */
// } t_timeval;
int			mails = 0;

void	*start_routine(void *data)
{
	int				i;
	pthread_mutex_t	*mutex;

	mutex = ((t_data *)data)->mutex;
	// printf("start_routine\n");
	i = 0;
	while (i < 10000000)
	{
		pthread_mutex_lock(&mutex[0]);
		mails++;
		pthread_mutex_unlock(&mutex[0]);
		i++;
	}
	// sleep(2);
	return (NULL);
}



void init_data(int nb_of_philos, t_data *data)
{
	int i;

	data->th = malloc(sizeof(pthread_t) * nb_of_philos);
	if (data->th == NULL)
		exit(1);
	data->mutex = malloc(sizeof(pthread_mutex_t) * nb_of_philos);
	if (data->mutex == NULL)
		exit(1);
	i = 0;
	while (i < nb_of_philos)
	{
		pthread_mutex_init(&data->mutex[i], NULL);
		i++;
	}
}

void init_args(int argc, char *argv[], t_args *args)
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
		args->nb_of_times_each_philo_must_eat = -1;
}

// argv
// 	number_of_philosophers
// 	time_to_die
// 	time_to_eat
// 	time_to_sleep
// 	(option) number_of_times_each_philosopher_must_eat
int	main(int argc, char *argv[])
{
	struct timeval tv_start;
	struct timeval tv_end;
	int i;
	t_args args;
	t_data data;

	init_args(argc, argv, &args);
	init_data(args.nb_of_philos, &data);
	
	if (gettimeofday(&tv_start, NULL) != 0)
		exit(1);
	// printf("time = %ld.%d\n", tv_start.tv_sec, tv_start.tv_usec);
	printf("time start = %ld\n", tv_start.tv_sec * 1000000 + tv_start.tv_usec);
	i = 0;
	while (i < args.nb_of_philos)
	{
		if (pthread_create(&data.th[i], NULL, start_routine, &data) != 0)
			exit(1);
		printf("start routine %d\n", i);
		i++;
	}
	i = 0;
	while (i < args.nb_of_philos)
	{
		if (pthread_join(data.th[i], NULL) != 0)
			exit(1);
		printf("  end routine %d\n", i);
		i++;
	}

	// if (pthread_create(&tid1, NULL, start_routine, &mutex) != 0)
	// 	exit(1);
	// if (pthread_create(&tid2, NULL, start_routine, &mutex) != 0)
	// 	exit(1);

	// if (pthread_join(tid1, NULL) != 0)
	// 	exit(1);
	// if (pthread_join(tid2, NULL) != 0)
	// 	exit(1);
	i = 0;
	while(i < args.nb_of_philos)
	{
		pthread_mutex_destroy(&data.mutex[i]);
		i++;
	}
	if (gettimeofday(&tv_end, NULL) != 0)
		exit(1);
	// printf("time = %ld.%d\n", tv_end.tv_sec, tv_end.tv_usec);
	printf("time   end = %ld\n", tv_end.tv_sec * 1000000 + tv_end.tv_usec);
	printf("time  diff = %ld\n", ((tv_end.tv_sec * 1000000 + tv_end.tv_usec)
				- (tv_start.tv_sec * 1000000 + tv_start.tv_usec)) / 1000);
	printf("mails = %d\n", mails);
}