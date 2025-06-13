/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_list.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 10:31:48 by taya              #+#    #+#             */
/*   Updated: 2025/06/13 10:31:49 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_env *create_env_node(char *env_var)
{
    t_env *new_node;
    char *equal_sign;
    
    new_node = malloc(sizeof(t_env));
    if (!new_node)
        return NULL;

    equal_sign = strchr(env_var, '=');
    if (equal_sign)
    {
        new_node->name = strndup(env_var, equal_sign - env_var);
        new_node->value = strdup(equal_sign + 1);
    }
    else
    {
        new_node->name = strdup(env_var);
        new_node->value = NULL;
    }
    new_node->next = NULL;
    return new_node;
}

void add_to_env_list(t_env **head, t_env *new_node)
{
    t_env *tmp;

    if (*head == NULL) 
        *head = new_node;
    else
    {
        tmp = *head;
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = new_node;
    }
}

t_env *init_env(char **envp)
{
    t_env *head = NULL;
    t_env *new_node;
    int i = 0;

    while (envp[i])
    {
        new_node = create_env_node(envp[i]);
        if (!new_node)
            return (NULL); 
        add_to_env_list(&head, new_node);
        i++;
    }
    return (head); 
}

