/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imasayos <imasayos@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/07 16:47:58 by imasayos          #+#    #+#             */
/*   Updated: 2023/07/30 03:56:43 by imasayos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include <stdio.h>

static int	ft_isspace(int c)
{
	if ((9 <= c && c <= 13) || c == 32)
		return (1);
	else
		return (0);
}

static int	ft_check_overflow(long long num, char c, int sign)
{
	int	flg;

	flg = 0;
	if (num >= 922337203685477581 && '0' <= c && c <= '9')
	{
		if (sign == -1)
			flg = -1;
		else
			flg = 1;
	}
	else if (num >= 922337203685477580)
	{
		if ('7' <= c && c <= '9' && sign == 1)
			flg = 1;
		else if ('8' <= c && c <= '9' && sign == -1)
			flg = -1;
	}
	return (flg);
}

int	ft_atoi(char *nptr)
{
	int			sign;
	long long	num;

	sign = 1;
	while (ft_isspace(*nptr))
		nptr++;
	if (*nptr == '+')
		nptr++;
	else if (*nptr == '-')
	{
		sign = -1;
		nptr++;
	}
	num = 0;
	while ('0' <= *nptr && *nptr <= '9')
	{
		if (ft_check_overflow(num, *nptr, sign) == 1)
			return ((int)LONG_MAX);
		else if (ft_check_overflow(num, *nptr, sign) == -1)
			return ((int)LONG_MIN);
		num = num * 10 + (*nptr - '0');
		nptr++;
	}
	return ((int)(sign * num));
}
