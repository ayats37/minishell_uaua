/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 10:51:02 by taya              #+#    #+#             */
/*   Updated: 2025/06/13 10:51:03 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


int handle_redirection(t_tree *node)
{
    t_token *redir;
    int fd;
    
    if (!node || !node->redir)
        return (0);
    redir = node->redir;
    while (redir)
    {
        if (redir->type == HEREDOC)
        {
            if (redir->fd > 0)
            {
                if (dup2(redir->fd, STDIN_FILENO) == -1)
                    write_error(NULL, "dup2 failed");
                close(redir->fd);
            }
        }
        if (redir->type == REDIR_IN)
        {
            fd = open(redir->value, O_RDONLY);
            if (fd == -1)
                write_error(redir->value, "No such file or directory");
            if (dup2(fd, STDIN_FILENO) == -1)
                write_error(NULL, "dup2 failed");
            close(fd);
        }
        else if (redir->type == REDIR_OUT)
        {
            fd = open(redir->value, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1)
                write_error(redir->value, "Permission denied");
            if (dup2(fd, STDOUT_FILENO) == -1)
                write_error(NULL, "dup2 failed");
            close(fd);
        }
        else if (redir->type == APPEND)
        {
            fd = open(redir->value, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd == -1)
                write_error(redir->value, "Permission denied");
            if (dup2(fd, STDOUT_FILENO) == -1)
                write_error(NULL, "dup2 failed");
            close(fd);
        }
        redir = redir->next;
    }
    return 0;
}



int execute_pipe(t_tree *node, char **env, t_env **envlist)
{
    pid_t pid1, pid2;
    int status1, status2;
    int pipe_fd[2];

    if (pipe(pipe_fd) == -1)
    {
        perror("pipe faild");
        return 1;
    }
    pid1 = fork();
    if (pid1 == -1)
    {
        perror("fork failed");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return 1;
    }
    if (pid1 == 0)
    {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
        exit(execute_tree(node->left, env, envlist));
    }
    pid2 = fork();
    if (pid2 == -1)
    {
        perror("fork failed");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        waitpid(pid1, &status1, 0);
        return 1;
    }  
    if (pid2 == 0)
    {
        close(pipe_fd[1]);
        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[0]);
        exit(execute_tree(node->right, env, envlist));
		}
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);
    return (WEXITSTATUS(status2));
}

int execute_tree(t_tree *node, char **env, t_env **envlist)
{
    
    int last_status = 0;
    int status = 0;

    if (!node)
        return (1);
    if (node->type == PIPE)
        return (execute_pipe(node, env, envlist));
    else if (node->type == CMD || node->type == DOUBLE_QUOTE)
    {
        if (is_builtin(node->cmd[0]))
            return (execute_builtin(node, envlist, last_status));
        else
            return (execute_cmd(node->cmd, env, node));
    }
    else if (node->type == AND || node->type == OR)
    {
        status = execute_tree(node->left, env, envlist);
        if (node->type == AND && status == 0)
            return (execute_tree(node->right, env, envlist));
        if (node->type == OR && status != 0)
            return (execute_tree(node->right, env, envlist));
        return (status);
    }
    if (node->redir) 
    {
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork failed");
            return (1);
        }
        if (pid == 0)
        {
            handle_redirection(node);
            exit(0);
        }
        waitpid(pid, &status, 0);
        return (WEXITSTATUS(status));
    }
    return (1);
}
