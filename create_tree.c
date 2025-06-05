/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_tree.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ouel-afi <ouel-afi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 20:20:13 by ouel-afi          #+#    #+#             */
/*   Updated: 2025/05/12 20:20:22 by ouel-afi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_tree	*create_tree_node(t_token *token, char **cmd, t_token *redir)
{
	t_tree	*node;

	node = malloc(sizeof(t_tree));
	if (!node)
		return (NULL);
	if (cmd != NULL)
		node->cmd = cmd;
	if (redir)
		node->redir = redir;
	node->has_space = token->has_space;
	node->type = token->type;
	node->value = token->value;
	node->left = NULL;
	node->right = NULL;
	return (node);
}
