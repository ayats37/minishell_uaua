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

int handle_variable(char *str, t_env *env_list)
{
    int var_len;
    char *var;
    char *value;

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
    {
        printf("$");
        return (0);
    }
}

int execute_builtin(t_tree *node, t_env **envlist)
{
    int stdout_backup;
    int result;
    
    stdout_backup = dup(STDOUT_FILENO);// kandir copy l terminal hit fach testet kayb9a ga3 dakchi ytktb f outfile donc kayb9a forever
    if (node->redir)
        handle_redirection(node);    
    result = 0;
    if (strcmp(node->cmd[0], "echo") == 0)
        result = ft_echo(node->cmd, *envlist);
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
