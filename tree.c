#include "minishell.h"

int is_builtin(char *cmd)
{
    if (!cmd)
        return (0);
    return (!strcmp(cmd, "cd")
        || !strcmp(cmd, "echo")
        || !strcmp(cmd, "exit")
        || !strcmp(cmd, "env")
        || !strcmp(cmd, "export")
        || !strcmp(cmd, "unset")
        || !strcmp(cmd, "pwd"));
}

void write_error(char *message)
{
    perror(message);
    exit(1);
}

int execute_cmds(char **cmds, char **env, t_tree *node)
{
    pid_t pid;
    int status;
    char *full_path;

    pid = fork();
    if (pid == -1)
        write_error("fork failed");
    if (pid == 0)
    {
        if (node->redir)
            handle_redirection(node);
        full_path = find_cmd_path(cmds[0], env);
        if (!full_path)
            write_error("command not found");
        execve(full_path, cmds, env);
        free(full_path);
        write_error("execve failed");
    }
    else
    {
        waitpid(pid, &status, 0);
        return (WEXITSTATUS(status));
    }
    return (0);
}

int execute_pipe(t_tree *node, char **env, t_env *envlist)
{
    pid_t pid1;
    pid_t pid2;
    int status1;
    int status2;
    int pipe_fd[2];

    if (pipe(pipe_fd) == -1)
        write_error("pipe failed");
    pid1 = fork();
    if (pid1 == -1)
        write_error("fork failed");
    if (pid1 == 0)
    {
        if (node->redir)
            handle_redirection(node);
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
        execute_tree(node->left, env, &envlist);
        exit(0);
    }
    pid2 = fork();
    if (pid2 == -1)
        write_error("fork failed");
    if (pid2 == 0)
    {
        if (node->redir)
            handle_redirection(node);
        close(pipe_fd[1]);
        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[0]);
        execute_tree(node->right, env, &envlist);
        exit(0);
    }
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);
    
    return (WEXITSTATUS(status2));
}

int handle_redirection(t_tree *node)
{
    int fd;
    t_token *current_redir;
    int last_heredoc_fd = -1;

    if (!node || !node->redir)
        return (0);
    current_redir = node->redir;
    while (current_redir)
    {
        if (current_redir->type == HEREDOC)
        {
            if (last_heredoc_fd != -1)
                close(last_heredoc_fd);
            last_heredoc_fd = handle_heredoc_redirection(current_redir);
        }
        current_redir = current_redir->next;
    }
    if (last_heredoc_fd != -1)
    {
        dup2(last_heredoc_fd, STDIN_FILENO);
        close(last_heredoc_fd);
    }
    current_redir = node->redir;
    while (current_redir)
    {
        if (!current_redir->value && current_redir->type != HEREDOC)
        {
            write(2, "Invalid filename\n", 18);
            return (1);
        }
        if (current_redir->type == REDIR_IN)
        {
            fd = open(current_redir->value, O_RDONLY);
            if (fd == -1)
            {
                printf("minishell: %s : No such file or directory\n", current_redir->value);
                exit (1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        else if (current_redir->type == REDIR_OUT)
        {
            fd = open(current_redir->value, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1)
            {
                printf("minishell: %s : No such file or directory\n", current_redir->value);
                exit (1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        else if (current_redir->type == APPEND)
        {
            fd = open(current_redir->value, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd == -1)
            {
                printf("minishell: %s : No such file or directory\n", current_redir->value);
                exit (1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        current_redir = current_redir->next;
    }
    return (0);
}

int handle_heredoc_redirection(t_token *heredoc_token)
{
    char *line;
    int pipe_fd[2];
    char *delimiter;

    if (pipe(pipe_fd) == -1)
        write_error("pipe failed");
    delimiter = heredoc_token->value;
    while (1)
    {
        line = readline("> ");
        if (!line || strcmp(line, delimiter) == 0)
        {
            free(line);
            break;
        }
        write(pipe_fd[1], line, strlen(line));
        write(pipe_fd[1], "\n", 1);
        free(line);
    }
    close(pipe_fd[1]);
    return (pipe_fd[0]);
}

void heredoc(t_tree *node)
{
    t_token *redir = node->redir;
    char *line;
    char *delimiter;
    
    while (redir)
    {
        if (redir->type == HEREDOC)
        {
            delimiter = redir->value;
            while (1)
            {
                line = readline("> ");
                if (!line || strcmp(line, delimiter) == 0)
                {
                    free(line);
                    break;
                }
                free(line);
            }
        }
        redir = redir->next;
    }
}

int execute_tree(t_tree *node, char **env, t_env **envlist)
{
    int status = 0;
		int last_status = 0;

    if (!node)
        return (1);
    if (node->redir && (!node->cmd || !node->cmd[0]))
    {
        heredoc(node);
        return (0);
		}
    if (node->type == PIPE)
        return (execute_pipe(node, env, *envlist));
    else if (node->type == CMD || node->type == DOUBLE_QUOTE)
    {
        if (!node->cmd || !node->cmd[0])
            return (1);
            
        if (is_builtin(node->cmd[0]))
            return (execute_builtin(node, envlist, last_status));
        else
            return (execute_cmds(node->cmd, env, node));
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
    return (1);
}