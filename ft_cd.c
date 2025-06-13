/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 10:51:19 by taya              #+#    #+#             */
/*   Updated: 2025/06/13 10:51:20 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int ft_cd(char **cmd, t_env *envlist)
{
    char *cwd;
    char *oldpwd;
    char *path;
    char *env_name;

    path = NULL;
    cwd = getcwd(NULL, 0);
    if (!cwd)
    return (printf("minishell: cd: error retrieving current directory\n"), 1);
    oldpwd = ft_strdup(cwd);
    free(cwd);
    if (!cmd[1] || strcmp(cmd[1], "~") == 0)
    {
        path = getenv("HOME");
        if (!path)
            return (printf("minishell: cd: HOME not set\n"), free(oldpwd), 1);
    }
    else if (strcmp(cmd[1], "-") == 0)
    {
        path = getenv("OLDPWD");
        if (!path)
            return (printf("minishell: cd: OLDPWD not set\n"), free(oldpwd), 1);
        printf("%s\n", path);
    }
    else if (cmd[1][0] == '$')
    {
        env_name = cmd[1] + 1;
        path = getenv(env_name);
        if (!path)
            return (printf("minishell: cd: %s: undefined variable\n", cmd[1]), free(oldpwd), 1);
    }
    else
        path = cmd[1];
    if (chdir(path) != 0)
        return (printf("minishell: cd: %s: %s\n", path, strerror(errno)), free(oldpwd), 1);
    cwd = getcwd(NULL, 0);
    if (cwd)
    {
        update_env("OLDPWD", oldpwd, &envlist);
        update_env("PWD", cwd, &envlist);
        free(cwd);
    }
    free(oldpwd);
    return (0);
}

