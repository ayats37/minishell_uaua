/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 10:51:52 by taya              #+#    #+#             */
/*   Updated: 2025/06/13 10:51:53 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int ft_unset(char **cmd, t_env **env_list)
{
    int i;
    t_env *current;
    t_env *prev;
    
    i = 1;
    if (!cmd || !cmd[i])
        return (1);
    while (cmd[i])
    {
        current = *env_list;
        prev = NULL;
        while (current)
        {
            if (strcmp(cmd[i], current->name) == 0)
            {
                if (prev == NULL)
                    *env_list = current->next;
                else
                    prev->next = current->next;
                free(current->name);
                free(current->value);
                free(current);
                break;
            }
            prev = current;
            current = current->next;
        }
        i++;
    }  
    return (1);
}
