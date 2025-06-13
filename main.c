#include "minishell.h"

void setup_shell_terminal(void)
{
    pid_t shell_pgid;

    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    shell_pgid = getpid();
    if (setpgid(shell_pgid, shell_pgid) < 0)
    {
        perror("Couldn't put the shell in its own process group");
        exit(1);
    }
    if (tcsetpgrp(STDIN_FILENO, shell_pgid) < 0)
    {
        perror("Couldn't set terminal control");
        exit(1);
    }
}

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
    if (node->redir)
        free_tokens(node->redir);

    free(node);
}

void free_lexer(t_lexer *lexer)
{
    if (!lexer)
        return;
    free(lexer);
}


void	handler(int sig)
{
	(void)sig;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

// void vv()
// {
//     system("leaks minishell");
// }
void free_resources(char *input, t_lexer *lexer, t_token *token_list, t_tree *node)
{
    if (input)
        free(input);
    if (lexer)
        free_lexer(lexer);
    if (token_list)
        free_tokens(token_list);
    if (node)
        free_tree(node);
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
    input = NULL;
    lexer = NULL;
    node = NULL;
    token_list = NULL;
    // atexit(vv);
    envlist = init_env(env);
    setup_shell_terminal();
    signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, handler);
    rl_catch_signals = 0;
    
    while (1)
    {
        input = readline("minishell> ");
        if (!input)
        {
            write(1, "exit", 5);
            break;
        }
        if (input[0] == '\0')
        {
            free(input);
            continue;
        }  
        add_history(input);
        lexer = initialize_lexer(input);
		while (lexer->position < lexer->lenght)
		{
			token = get_next_token(lexer);
			if (!token)
			    continue;
			token->type = token_type(token);
			append_token(&token_list, token);
		}
        if (!token_list)
        {
            free(input);
			continue ;
        }
		if (check_parenthesis(token_list) != 0)
			continue;
        merge_tokens(&token_list);
        if (check_errors(token_list) == 1)
			continue;
        node = parse_op(token_list);
        if(!node)
			continue;
        process_heredocs_tree(node); 
        execute_tree(node, env, &envlist);  
        free(input);
    }
    free_resources(input, lexer, token_list, node);
    return (0);
}
