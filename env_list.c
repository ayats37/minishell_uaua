#include "minishell.h"

t_env *create_env_node(char *env_var)
{
    t_env *new_node;
    char *equal_sign;
    
    new_node = malloc(sizeof(t_env));
    if (!new_node)
        return NULL;

    equal_sign = strchr(env_var, '=');
    if (equal_sign)
    {
        new_node->name = strndup(env_var, equal_sign - env_var);
        new_node->value = strdup(equal_sign + 1);
    }
    else
    {
        new_node->name = strdup(env_var);
        new_node->value = NULL;
    }
    new_node->next = NULL;
    return new_node;
}

void add_to_env_list(t_env **head, t_env *new_node)
{
    t_env *tmp;

    if (*head == NULL) 
        *head = new_node;
    else
    {
        tmp = *head;
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = new_node;
    }
}

t_env *init_env(char **envp)
{
    t_env *head = NULL;
    t_env *new_node;
    int i = 0;

    while (envp[i])
    {
        new_node = create_env_node(envp[i]);
        if (!new_node)
            return (NULL); 
        add_to_env_list(&head, new_node);
        i++;
    }
    return (head); 
}

void update_env(char *name, char *value, t_env **env_list)
{
    t_env *current = *env_list;
    t_env *new_env;

    while (current)
    {
        if ((strcmp(current->name, name) == 0))
        {
            free(current->value);
            current->value = ft_strdup(value);
            return;
        }
        current = current->next;
    }
    new_env = malloc(sizeof(t_env));
    new_env->name = ft_strdup(name);
    new_env->value = ft_strdup(value);
    new_env->next = *env_list;
    *env_list = new_env;
}
char *get_env_value(char *name, t_env *env_list)
{
    t_env *current = env_list;

    while (current)
    {
        if (strcmp(current->name, name) == 0)
            return (current->value);
        current = current->next;
    }
    return(NULL);
}

t_env *find_env_var(char *name, t_env *env_list)
{
	t_env *current;

	current = env_list;
	while (current)
	{
		if (strcmp(current->name, name) == 0)
			return (current);
		current = current->next;
	}
	return (NULL);
}

void env_append(char *name, char *value, t_env **env_list)
{
	t_env *found_var;
	char *new_value;
	char *old_value;

	found_var = find_env_var(name, *env_list);
	if (!found_var)
	{
		update_env(name, value, env_list);
		return;
	}
	old_value = get_env_value(name, *env_list);
	if (old_value)
	{
		new_value = ft_strjoin(old_value, value);
		update_env(name, new_value, env_list);
		free(new_value);
	}
	else
		update_env(name, value, env_list);
}
