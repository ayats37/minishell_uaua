/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 10:51:42 by taya              #+#    #+#             */
/*   Updated: 2025/06/13 11:56:14 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int ft_exit(char **cmd, t_env *env_list)
{
    int exit_status;
    t_env *current;
    int i;
    
    current = env_list;
    exit_status = 0;
    i = 1;
    if (cmd && cmd[i])
        exit_status = atoi(cmd[i]);
    // ma3deha ma3na 
    // while (current)
    // {
    //     if (current->name)
    //         free(current->name);
    //     if (current->value)
    //         free(current->value);
    //     // free(current);
    //     current = current->next;
    // }
    printf("exit\n");
    exit(exit_status);
    return (0);
}
