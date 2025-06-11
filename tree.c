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
				dup2(redir->fd, STDIN_FILENO);
				close(redir->fd);
			}
		}
	    if (redir->type == REDIR_IN)
        {
            fd = open(redir->value, O_RDONLY);
            if (fd == -1)
            {
                printf("minishell: %s : No such file or directory\n", redir->value);
                exit (1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        else if (redir->type == REDIR_OUT)
        {
            fd = open(redir->value, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1)
            {
                printf("minishell: %s : No such file or directory\n", redir->value);
                exit (1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        else if (redir->type == APPEND)
        {
            fd = open(redir->value, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd == -1)
            {
                printf("minishell: %s : No such file or directory\n", redir->value);
                exit (1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        redir = redir->next;
    }
    return 0;
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
void write_error(char *message)
{
    perror(message);
    exit(1);
}

int execute_cmd(char **cmds, char **env, t_tree *node)
{
    pid_t pid;
    int status;
    char *full_path;

    pid = fork();
    if (pid == -1)
        write_error("fork failed");
    if (pid == 0)
    {
				if (node && node->redir)
				{
					write(1, "889888888888\n", 14);
        	        handle_redirection(node);   
				}
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