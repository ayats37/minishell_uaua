#include "minishell.h"


int handle_redirection(t_tree *node)
{
	t_token *redir;
	int fd;
	
	if (!node->redir)
	return 0;  
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
                return 1;
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        if (redir->type == REDIR_OUT)
        {
            fd = open(redir->value, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1)
                return 1;
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        if (redir->type == APPEND)
        {
            fd = open(redir->value, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd == -1)
                return 1;
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

int execute_cmd(char **cmds, char **env, t_tree *node)
{
    pid_t pid;
    int status;
    char *full_path;
    if (!node && !node->redir)
        handle_redirection(node) == -1;   
    pid = fork();
    if (pid == -1)
    {
        perror("fork faild");
        return 1;
    }   
    if (pid == 0)
    {
        full_path = find_cmd_path(cmds[0], env);
        if (!full_path)
        {
            printf("%s: command not found\n", cmds[0]);
            exit(127);
        } 
        execve(full_path, cmds, env);
        perror("execve failed");
        exit(1);
    }
    waitpid(pid, &status, 0);
    return (WEXITSTATUS(status));
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
    int left_status;

    if (!node)
        return (1);
    if (node->type == PIPE)
        return (execute_pipe(node, env, envlist));
    if (node->type == CMD)
    {
        if (!node->cmd || !node->cmd[0])
            return (1);
        if (is_builtin(node->cmd[0]))
            return (execute_builtin(node, envlist, 0));
        return (execute_cmd(node->cmd, env, node));
    }
    if (node->type == AND)
    {
        left_status = execute_tree(node->left, env, envlist);
        if (left_status == 0)
            return (execute_tree(node->right, env, envlist));
        return (left_status);
    }
    if (node->type == OR)
    {
        left_status = execute_tree(node->left, env, envlist);
        if (left_status != 0)
            return (execute_tree(node->right, env, envlist));
        return (left_status);
    }
    return (1);
}