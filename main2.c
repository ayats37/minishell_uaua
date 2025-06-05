/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ouel-afi <ouel-afi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 18:28:40 by ouel-afi          #+#    #+#             */
/*   Updated: 2025/05/13 20:26:29 by ouel-afi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// void free_tokens(t_token *tokens)
// {
//     t_token *current = tokens;
//     t_token *next;

//     while (current != NULL)
//     {
//         next = current->next;
//         free(current->value);
//         free(current);
//         current = next;
//     }
// }

// void free_tree(t_tree *node)
// {
//     if (node == NULL)
//         return;
    
//     free_tree(node->left);
//     free_tree(node->right);
    
//     if (node->cmd)
//     {
//         for (int i = 0; node->cmd[i]; i++)
//             free(node->cmd[i]);
//         free(node->cmd);
//     }
    
//     free(node->value);
//     free(node);
// }

void	handler(int sig)
{
	(void)sig;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void ll(){
	system("leaks minishell");
}

int main(int argc, char **argv, char **env)
{
    char    *input;
    t_lexer *lexer;
    t_token *token;
    t_token *token_list = NULL;
    t_tree  *node = NULL;

    (void)argc;
    (void)argv;
	(void)env;
    signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, handler);
    rl_catch_signals = 0;
	atexit(ll);
    while (1)
    {
        input = readline("minishell> ");
        if (!input)
        {
            write(1, "exit\n", 5);
            exit(0);
        }
        if (input[0] == '\0')
        {
            free(input);
            continue;
        }
        add_history(input);
        lexer = initialize_lexer(input);
        token_list = NULL;
        while (lexer->position < lexer->lenght)
        {
			// ll();
            token = get_next_token(lexer);
            if (token == NULL)
                break ;
            token->type = token_type(token);
            append_token(&token_list, token);
        }
		if (!token_list)
			continue ;
		if (check_parenthesis(token_list) != 0)
			continue;
        merge_tokens(&token_list);
		if (check_errors(token_list) == 1)
			continue;
		print_linked_list(token_list);
        node = parse_op(token_list);
		if(!node)
			continue;
    }
    return (0);
}