#include "minishell.h"

char *str_join_free(char *s1, const char *s2)
{
    char *res;
    int len = strlen(s1) + strlen(s2);
    res = malloc(len + 1);
    if (!res) return NULL;
    strcpy(res, s1);
    strcat(res, s2);
    free(s1);
    return res;
}

char *expand_token(char *token, t_env *env_list, int last_exit_status)
{
    char *result = strdup("");
    int i = 0;
		int len;
		char *status_str;
		char *var_name;
		char *val;
    while (token[i])
    {
        if (token[i] == '$')
        {
            i++;
            if (token[i] == '?')
            {
                status_str = ft_itoa(last_exit_status);
                result = str_join_free(result, status_str);
                free(status_str);
                i++;
            }
            else if (token[i] && (token[i] == '_' || is_alpha(token[i])))
            {
                int start = i;
                while (token[i] && (isalnum(token[i]) || token[i] == '_'))
                    i++;
                len = i - start;
                var_name = strndup(&token[start], len);
                val = get_env_value(var_name, env_list);
                free(var_name);
                if (val)
                    result = str_join_free(result, val);
            }
            else
                result = str_join_free(result, "$");
        }
        else
        {
            char buf[2] = {token[i], 0};
            result = str_join_free(result, buf);
            i++;
        }
    }
    return result;
}


void expand_variables(char **tokens, int last_exit_status, t_env *env_list)
{
	int i = 0;
    while (tokens[i])
    {
        char *expanded = expand_token(tokens[i], env_list, last_exit_status);
        free(tokens[i]);
        tokens[i] = expanded;
				i++;
    }
}
