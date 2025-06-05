#include "minishell.h"

void free_tokens(t_token *tokens)
{
    t_token *current = tokens;
    t_token *next;

    while (current != NULL)
    {
        next = current->next;
        free(current->value);
        free(current);
        current = next;
    }
}

void free_tree(t_tree *node)
{
    if (node == NULL)
        return;
    
    free_tree(node->left);
    free_tree(node->right);
    
    if (node->cmd)
    {
        for (int i = 0; node->cmd[i]; i++)
            free(node->cmd[i]);
        free(node->cmd);
    }
    
    free(node->value);
    free(node);
}

void	handler(int sig)
{
	(void)sig;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

int main(int argc, char **argv, char **env)
{
    char *input;
    t_lexer *lexer;
    t_token *token;
    t_token *token_list;
    t_tree *node;
    t_env *envlist;
		(void)argc;
		(void)argv;

    envlist = init_env(env);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, handler);
    rl_catch_signals = 0;
    
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
			token = get_next_token(lexer);
			if (!token)
			    continue;
			token->type = token_type(token);
			append_token(&token_list, token);
		}
        merge_tokens(&token_list);
        node = parse_op(token_list);
		// print_tree(node, 0, "NODE");
        process_heredocs_tree(node); 
        execute_tree(node, env, &envlist);  
        free(input);
    }
    
    return 0;
}