/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 10:58:04 by taya              #+#    #+#             */
/*   Updated: 2025/06/13 12:02:34 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void handle_heredoc_input(char *delimiter, int write_fd)
{
    char *line;
    
    while (1)
    {
        line = readline("> ");
        if (!line)
            break;
        if (strcmp(line, delimiter) == 0)
        {
            free(line);
            break;
        }
        write(write_fd, line, ft_strlen(line));
        write(write_fd, "\n", 1);
        free(line);
    }
}
void process_heredocs_tree(t_tree *node)
{
    t_token *redir;
    int pipe_fd[2];
    pid_t pid;
    int status;
    
    if (!node)
        return;
    if (node->left)
        process_heredocs_tree(node->left);  
    if (node->right)
        process_heredocs_tree(node->right);
    redir = node->redir;
    while (redir)
    {
        if (redir->type == HEREDOC)
        {
            if (pipe(pipe_fd) == -1)
            {
                perror("pipe failed");
                redir = redir->next;
                continue;
            }
            pid = fork();
            if (pid == -1)
            {
                perror("fork failed");
                close(pipe_fd[0]);
                close(pipe_fd[1]);
                redir = redir->next;
                continue;
            }
            if (pid == 0)
            {
                close(pipe_fd[0]);
                handle_heredoc_input(redir->value, pipe_fd[1]);
                close(pipe_fd[1]);
                exit(0);
            }
            close(pipe_fd[1]);
            waitpid(pid, &status, 0);
            redir->fd = pipe_fd[0]; 
        }
        redir = redir->next;
    }
}
