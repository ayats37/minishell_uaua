/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 10:51:14 by taya              #+#    #+#             */
/*   Updated: 2025/06/13 11:20:42 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int execute_builtin(t_tree *node, t_env **envlist, int last_status)
{
    int stdout_backup;
    int result;
    
    stdout_backup = dup(STDOUT_FILENO);// kandir copy l terminal hit fach testet kayb9a ga3 dakchi ytktb f outfile donc kayb9a forever
    if (node && node->redir)
        handle_redirection(node);    
    result = 0;
    if (strcmp(node->cmd[0], "echo") == 0)
        result = ft_echo(node->cmd, *envlist, last_status);
    else if (strcmp(node->cmd[0], "cd") == 0)
        result = ft_cd(node->cmd, *envlist);
    else if (strcmp(node->cmd[0], "pwd") == 0)
        result = ft_pwd();
    else if (strcmp(node->cmd[0], "export") == 0)
        result = ft_export(node->cmd, envlist);
    else if (strcmp(node->cmd[0], "unset") == 0)
        result = ft_unset(node->cmd, envlist);
    else if (strcmp(node->cmd[0], "env") == 0)
        result = ft_env(envlist);
    else if (strcmp(node->cmd[0], "exit") == 0)
        result = ft_exit(node->cmd, *envlist);
    if (node->redir)
    {
        dup2(stdout_backup, STDOUT_FILENO);// hna kan3awd nrj3 l terminalmn ba3d makaytktb lia dakchi f outfile 
        close(stdout_backup);
    }   
    return (result);
}

int execute_cmd(char **cmds, char **env, t_tree *node) 
{
    pid_t pid;
    int status;
    char *full_path;
    
    pid = fork();
    if (pid == -1)
        write_error(cmds[0], "fork failed");
    if (pid == 0) 
    {
        if (node && node->redir)
            handle_redirection(node); 
        full_path = find_cmd_path(cmds[0], env);
        if (!full_path)
            write_error(cmds[0], "command not found");
        execve(full_path, cmds, env);
        free(full_path);
        write_error(cmds[0], "execve failed");
    }
    else 
    {
        waitpid(pid, &status, 0);
        return (WEXITSTATUS(status));
    }
    return (0);
}


