#include "minishell.h"

int ft_pwd(void)
{
    char *cwd;
    
    cwd = getcwd(NULL, 0);
    if (!cwd)
        return (perror("minishell: pwd"), 1);
    printf("%s\n", cwd);
    free(cwd);
    return 0;
}
