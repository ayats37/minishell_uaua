#include "minishell.h"

t_token *create_token_fd(char *value, t_type type)
{
    t_token *token = malloc(sizeof(t_token));
    if (!token)
        return NULL;
    
    token->value = ft_strdup(value);
    token->type = type;
    token->has_space = 0;
    token->fd = -1;
    token->next = NULL;
    
    return token;
}

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
                perror("pipe");
                redir = redir->next;
                continue;
            }
            pid = fork();
            if (pid == -1)
            {
                perror("fork");
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
