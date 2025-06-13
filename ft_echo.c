/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 10:51:32 by taya              #+#    #+#             */
/*   Updated: 2025/06/13 10:51:33 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

 int is_valid_n_flag(char *arg)
{
    int i;
    
    if (!arg || arg[0] != '-' || arg[1] != 'n')
        return (0);
    
    i = 2;
    while (arg[i])
    {
        if (arg[i] != 'n')
            return (0);
        i++;
    }
    return (1);
}

int ft_echo(char **cmd, t_env *env_list, int last_status)
{
    int i = 1;
    int newline = 1;
    int j;
    int offset;

    while (cmd[i] && is_valid_n_flag(cmd[i]))
    {
        newline = 0;
        i++;
    }
    while (cmd[i])
    {
        j = 0;
        while (cmd[i][j])
        {
            if (cmd[i][j] == '$')
            {
                offset = handle_variable(&cmd[i][j], env_list, last_status);
                j += offset + 1; 
            }
            else
            {
                printf("%c", cmd[i][j]);
                j++;
            }
        }
        if (cmd[i + 1])
            printf(" ");
        i++;
    }
    if (newline)
        printf("\n");
    return (0);
}
