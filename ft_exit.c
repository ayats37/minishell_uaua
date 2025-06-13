/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 10:51:42 by taya              #+#    #+#             */
/*   Updated: 2025/06/13 15:40:14 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int ft_exit(char **cmd, t_env *env_list)
{
    int exit_status = 0;
    t_env *current = env_list;
    char *tmp;
    int argc = 0;
    while (cmd && cmd[argc])
        argc++;
   if (argc >= 2)
{
    tmp = cmd[1];
    if (!is_num(tmp))
    {
        printf("exit\n");
        printf("bash: exit: %s: numeric argument required\n", tmp);
        exit(255);
    }
    else if (argc > 2)
    {
        printf("exit\n");
        printf("bash: exit: too many arguments\n");
        return 1;
    }
    else
        exit_status = atoi(tmp);
}
    while (current)
    {
        t_env *next = current->next;
        if (current->name)
            free(current->name);
        if (current->value)
            free(current->value);
        free(current);
        current = next;
    }
    printf("exit\n");
    exit(exit_status);
    return 0;
}
