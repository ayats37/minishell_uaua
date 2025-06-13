/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 10:33:37 by taya              #+#    #+#             */
/*   Updated: 2025/06/13 10:47:03 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int  is_alphanumeric(int c)
{
    return ((c >= 'A' && c <= 'Z') || 
            (c >= 'a' && c <= 'z') || 
            (c >= '0' && c <= '9'));
}
int  is_alpha(int c)
{
    return ((c >= 'A' && c <= 'Z') || 
            (c >= 'a' && c <= 'z'));
}

int handle_variable(char *str, t_env *env_list, int last_exit_status)
{
    int var_len;
    char *var;
    char *value;

    if (str[1] == '?')
    {
        printf("%d", last_exit_status);
        return (1); 
    }
    var_len = 0;
    while (str[var_len + 1] && (is_alphanumeric(str[var_len + 1]) || str[var_len + 1] == '_'))
        var_len++;
    if (var_len > 0)
    {
        var = ft_substr(str, 1, var_len);
        value = get_env_value(var, env_list);
        if (value)
            printf("%s", value);
        free(var);
        return (var_len);
    }
    else
        return (printf("$"), 0);
}
int is_builtin(char *cmd)
{
    if (!cmd)
        return 0;
    if (strcmp(cmd, "cd") == 0)
        return 1;
    if (strcmp(cmd, "echo") == 0)
        return 1;
    if (strcmp(cmd, "exit") == 0)
        return 1;
    if (strcmp(cmd, "env") == 0)
        return 1;
    if (strcmp(cmd, "export") == 0)
        return 1;
    if (strcmp(cmd, "unset") == 0)
        return 1;
    if (strcmp(cmd, "pwd") == 0)
        return 1;
    return 0;
}

void write_error(char *command, char *message) 
{
    write(STDERR_FILENO, "minishell: ", 11);
    if (command) {
        write(STDERR_FILENO, command, strlen(command));
        write(STDERR_FILENO, ": ", 2);
    }
    write(STDERR_FILENO, message, strlen(message));
    write(STDERR_FILENO, "\n", 1);
    exit(1);
}

