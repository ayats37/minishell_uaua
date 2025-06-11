#include "minishell.h"

int ft_exit(char **cmd, t_env *env_list)
{
    int exit_status;
    t_env *current;
    int i;
    
    current = env_list;
    exit_status = 0;
    i = 1;
    if (cmd && cmd[i])
        exit_status = atoi(cmd[i]);
    while (current)
    {
        free(current->name);
        free(current->value);
        free(current);
        current = current->next;
    }
    printf("exit\n");
    exit(exit_status);
    return (0);
}
