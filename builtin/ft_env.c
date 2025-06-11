#include "minishell.h"

int ft_env(t_env **env_list)
{
    t_env *current = *env_list;
    
    while (current)
    {
        printf("%s=%s\n", current->name, current->value);
        current = current->next;
    }
    return (0);
}
