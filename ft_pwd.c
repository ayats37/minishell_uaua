/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_pwd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 10:51:49 by taya              #+#    #+#             */
/*   Updated: 2025/06/13 10:51:50 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
