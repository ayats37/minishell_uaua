/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/12 12:07:43 by ouel-afi          #+#    #+#             */
/*   Updated: 2025/05/23 00:23:58 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_tree(t_tree *node, int depth, const char *side)
{
	if (!node)
		return;
	for (int i = 0; i < depth; i++)
		printf("  ");
	printf("[%s]", side);
	if (node->value)
		printf(" | value: %s", node->value);

	// 🔽 Print cmd if present
	if (node->cmd)
	{
		printf(" | cmd:");
		for (int i = 0; node->cmd[i]; i++)
			printf(" '%s'", node->cmd[i]);
	}
	if (node->redir)
	{
		printf("\nREDIR:    ");
		print_linked_list(node->redir);
	}
	printf("\n");

	print_tree(node->left, depth + 1, "L");
	print_tree(node->right, depth + 1, "R");
}


void	print_linked_list(t_token *token_list)
{
	t_token *current = token_list;
	while (current)
	{
		printf("token->value = %s		token->type = %d			token->has_space = %d\n", current->value, current->type, current->has_space);
		current = current->next;
	}
}

t_lexer	*initialize_lexer(char *input)
{
	t_lexer	*lexer;

	lexer = malloc((sizeof(t_lexer)));
	if (!lexer)
		return (NULL);
	lexer->input = input;
	lexer->lenght = ft_strlen(input);
	lexer->position = 0;
	return (lexer);
}

void	join_tokens(t_token *token)
{
	t_token	*next;
	char	*joined;

	if (!token || !token->next)
		return ;
	next = token->next;
	joined = ft_strjoin(token->value, next->value);
	if (!joined)
		return ;
	token->value = joined;
	token->next = next->next;
}

int	is_space(t_lexer *lexer)
{
	if ((lexer->input[lexer->position] >= 9 
			&& lexer->input[lexer->position] <= 13)
		|| lexer->input[lexer->position] == 32)
		return (1);
	return (0);
}

void	skip_whitespace(t_lexer *lexer)
{
	while (lexer->position < lexer->lenght && is_space(lexer))
		lexer->position++;
}

t_token	*create_token(char *value, char quote, int has_space)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	else
	{
		token->value = ft_strdup(value);
		token->next = NULL;
	}
	if (quote == '\'')
		token->type = 3;
	else if (quote == '"')
		token->type = 4;
	token->has_space = has_space;
	// printf("has_space = %d\n", token->has_space);
	return (token);
}

t_token	*handle_quote(t_lexer *lexer, char quote)
{
	size_t	lenght;
	size_t	start;
	char	*value;

	lexer->position += 1;
	start = lexer->position;
	while (lexer->position < lexer->lenght 
		&& lexer->input[lexer->position] != quote)
		lexer->position++;
	if (lexer->position >= lexer->lenght)
	{
		ft_putstr_fd("bash : syntax error unclosed quote\n", 2);
		return (0);
	}
	lenght = lexer->position - start;
	value = ft_substr(lexer->input, start, lenght);
	lexer->position += 1;
	if (lexer->input[lexer->position] == 32)
		return (create_token(value, quote, 1));
	return (create_token(value, quote, 0));
}

t_token	*handle_operations(t_lexer *lexer, char *oper, int i)
{
	char	*str;

	str = ft_substr(oper, 0, i);
	if (!str)
		return (NULL);
	str[i] = '\0';
	lexer->position += i;
	if (lexer->input[lexer->position] == 32)
		return (create_token(str, str[0], 1));
	return (create_token(str, str[0], 0));
}

// t_token	*handle_word(t_lexer *lexer)
// {
// 	char	*word = NULL;
// 	size_t	start;
// 	size_t	lenght;

// 	start = lexer->position;
// 	while (lexer->position < lexer->lenght && (!is_space(lexer)) 
// 		&& lexer->input[lexer->position] != '\'' 
// 		&& lexer->input[lexer->position] != '"' 
// 		&& lexer->input[lexer->position] != '|' 
// 		&& lexer->input[lexer->position] != '<' 
// 		&& lexer->input[lexer->position] != '>' 
// 		&& lexer->input[lexer->position] != '(' 
// 		&& lexer->input[lexer->position] != ')'
// 		&& lexer->input[lexer->position] != '&')
// 		lexer->position++;
// 	lenght = lexer->position - start;
// 	if (lexer->input[lexer->position] == 32){
// 		write(1, "oui\n", 4);
// 		word = ft_substr(lexer->input, start, lenght);
// 		return (create_token(word, word[0], 1));	
// 	}
// 	write(1, "non\n", 4);
// 	word = ft_substr(lexer->input, start, lenght);
// 	return (create_token(word, word[0], 0));
// }

t_token * handle_word(t_lexer *lexer)
{
    char    *result = ft_strdup("");
    char    *temp = NULL;
    size_t  start;
    int     has_space = 0;
    int     in_quotes = 0;

    while (lexer->position < lexer->lenght && !is_space(lexer))
    {
        if (lexer->input[lexer->position] == '\'' || 
            lexer->input[lexer->position] == '"')
        {
            char quote = lexer->input[lexer->position++];
            start = lexer->position;
            in_quotes = 1;
            
            while (lexer->position < lexer->lenght && 
                   lexer->input[lexer->position] != quote)
                lexer->position++;

            if (lexer->position >= lexer->lenght)
            {
				ft_putstr_fd("bash : syntax error near unexpected token `)'\n", 2);
                free(result);
                return 0;
            }

            temp = ft_substr(lexer->input, start, lexer->position - start);
            char *new_result = ft_strjoin(result, temp);
            free(result);
            free(temp);
            result = new_result;
            lexer->position++;
            in_quotes = 0;
        }
        else if (!in_quotes && ft_strchr("|<>()&", lexer->input[lexer->position])){
			// write(1, "1\n", 2);
			break;
		}
        else
        {
            if (in_quotes)
                break;
            start = lexer->position;
            while (lexer->position < lexer->lenght &&
                   !ft_strchr("\"'|<>()& ", lexer->input[lexer->position]))
                lexer->position++;

            temp = ft_substr(lexer->input, start, lexer->position - start);
            char *new_result = ft_strjoin(result, temp);
            free(result);
            free(temp);
            result = new_result;
        }
    }

    has_space = (lexer->position < lexer->lenght && 
                lexer->input[lexer->position] == ' ');
    return create_token(result, in_quotes ? 4 : 1, has_space);
}

t_token	*get_next_token(t_lexer *lexer)
{
	char	*current;

	skip_whitespace(lexer);
	if (lexer->position >= lexer->lenght)
		return (NULL);
	current = lexer->input + lexer->position;
	if (current[0] == '\'' || current[0] == '"')
		return (handle_quote(lexer, *current));
	if ((lexer->input[lexer->position] == '|' 
			&& lexer->input[lexer->position + 1] == '|') 
		|| (lexer->input[lexer->position] == '&' 
			&& lexer->input[lexer->position + 1] == '&'))
		return (handle_operations(lexer, current, 2));
	if ((lexer->input[lexer->position] == '>' 
			&& lexer->input[lexer->position + 1] == '>') 
		|| (lexer->input[lexer->position] == '<' 
			&& lexer->input[lexer->position + 1] == '<'))
		return (handle_operations(lexer, current, 2));
	if (current[0] == '|' || current[0] == '<' || current[0] == '>' 
		|| current[0] == '(' || current[0] == ')' || current[0] == '&')
		return (handle_operations(lexer, current, 1));
	return (handle_word(lexer));
}

int	cmd_type(t_token *token, int first_cmd)
{
	if (first_cmd == 0)
	{
		first_cmd++;
		return (1);
	}
	else if (token->type == PIPE || token->type == OPEN_PAREN 
		|| token->type == DOUBLE_QUOTE || token->type == OR 
		|| token->type == AND)
		return (1);
	else
		return (0);
}

t_type	token_type(t_token *token)
{
	if (token->type == 3)
		return (SINGLE_QUOTE);
	else if (token->type == 4)
		return (DOUBLE_QUOTE);
	else if (strcmp(token->value, "||") == 0)
		return (OR);
	else if (strcmp(token->value, "&&") == 0)
		return (AND);
	else if (strcmp(token->value, "|") == 0)
		return (PIPE);
	else if (strcmp(token->value, ">>") == 0)
		return (APPEND);
	else if (strcmp(token->value, "<<") == 0)
		return (HEREDOC);
	else if (strcmp(token->value, "<") == 0)
		return (REDIR_IN);
	else if (strcmp(token->value, ">") == 0)
		return (REDIR_OUT);
	else if (strcmp(token->value, "(") == 0)
		return (OPEN_PAREN);
	else if (strcmp(token->value, ")") == 0)
		return (CLOSE_PAREN);
	else
		return (CMD);
}

void append_token(t_token **head, t_token *token)
{
    t_token *tmp = NULL;

    if (!token)
        return;

    if (!*head)
    {
        *head = token;
        return;
    }
    tmp = *head;
    while (tmp->next)
        tmp = tmp->next;
    tmp->next = token;
}

void merge_tokens(t_token **tokens)
{
    t_token *current = *tokens;
    // t_token *tmp = NULL;

    while (current != NULL && current->next != NULL)
    {
        if (current->has_space == 0 && 
            ((current->type == SINGLE_QUOTE || 
             current->type == DOUBLE_QUOTE ||
			 current->type == 1) &&
             (current->next->type == SINGLE_QUOTE ||
             current->next->type == DOUBLE_QUOTE ||
			 current->next->type == 1)))
        {
            char *merged_value = ft_strjoin(current->value, current->next->value);
            current->value = merged_value;
            if (current->type == CMD || current->next->type == CMD)
                current->type = CMD;
            current->has_space = current->next->has_space;
            t_token *to_delete = current->next;
            current->next = to_delete->next;
        }
        else
        {
            // tmp = current;
            current = current->next;
        }
    }
}

t_tree *create_tree_node(t_token *token, char **cmd, t_token *redir)
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

t_token *get_last_pipe(t_token *token)
{
	t_token *pipe = NULL;
	t_token *tmp;
	size_t	paren = 0;

	tmp = token;
	while(tmp->next)
	{
		if (tmp->type == 9)
			paren++;
		else if (tmp->type == 10)
			paren--;
		if (tmp->type == 2 && paren == 0)
			pipe = tmp;
		tmp = tmp->next;
	}
	return (pipe);
}

t_token *get_last_opr(t_token *token)
{
	size_t	paren = 0;
	t_token *opr = NULL;
	t_token *tmp;

	tmp = token;
	while(tmp->next)
	{
		if (tmp->type == 9)
			paren++;
		else if (tmp->type == 10)
			paren--;
		else if ((tmp->type == 11 || tmp->type == 12) && paren == 0)
			opr = tmp;
		tmp = tmp->next;
	}
	return (opr);
}

t_token *sub_left(t_token *token, t_token *opr)
{
	t_token *head = NULL;
	t_token *tmp;
	t_token *new_node;
	t_token *current = token;

	while (current)
	{
		if (current == opr)
			break;
		new_node = malloc(sizeof(t_token));
		if (!new_node)
			return (NULL);
		new_node->value = ft_strdup(current->value);
		new_node->type = current->type;
		new_node->next = NULL;
		if(!head)
			head = new_node;
		else
			tmp->next = new_node;
		tmp = new_node;
		current = current->next;
	}
	return (head);
}

t_tree *parse_cmd(t_token *token)
{
	t_token *redir_tail = NULL;
	t_token *new_redir = NULL;
	t_token *redir = NULL;
	int last_type = 0;
	t_token *tmp = token;
	int nb_cmd = calculate_cmd(tmp);
	char **cmd_args = malloc(sizeof(char *) * (nb_cmd + 1));
	int i = 0;
	if (!cmd_args)
		return NULL;
	while (tmp)
	{
		if (tmp->type == 9 || tmp->type == 10)
		{
			printf("bash : syntax error parenthesis\n");
			return NULL;
		}
		if (tmp->type == 1 || tmp->type == 3 || tmp->type == 4)
		{
			cmd_args[i] = ft_strdup(tmp->value);
			i++;
			tmp = tmp->next;	
		}
		else if (tmp->next)
		{
			last_type = tmp->type;
			tmp = tmp->next;
			new_redir = create_token(tmp->value, 0, 0);
			new_redir->type = last_type;
			new_redir->next = NULL;
			if (!redir)
			{
				redir = new_redir;
				redir_tail = redir;
			}
			else
			{
				redir_tail->next = new_redir;
				redir_tail = new_redir;
			}
			tmp = tmp->next;
		}
	}
	cmd_args[i] = NULL;
	return create_tree_node(token, cmd_args, redir);
}

t_tree	*parse_paren(t_token *token)
{
	// write(1, "3\n", 2);
	t_token *current;
	t_token *tmp = token;
	size_t	paren = 0;
	if (tmp->type != 9){
		// write(1, "4\n", 2);
		return(parse_cmd(token));
	}
	// print_linked_list(tmp);
	while (tmp)
	{
		if (tmp->type == 9)
			paren++;
		else if (tmp->type == 10)
			paren--;
		if (paren == 0)
		{
			current = tmp;
			break ;
		}
		tmp = tmp->next;
	}
	if (paren != 0)
	{
		printf("bash : syntax unmatched parenthesis\n");
		return NULL;
	}
	t_token *sub_token = sub_left(token->next, current);
	// print_linked_list(sub_token);
	if(!sub_token){
		printf("bash: syntax error unclosed quote\n");
		return NULL;
	}

	return(parse_op(sub_token));
}

t_tree	*parse_pipes(t_token *token)
{
	t_token *pipe = get_last_pipe(token);
	t_token *left_token = NULL;
	t_token *right_token = NULL;
	t_token *tmp = token;

	if (pipe)
	{
		t_tree *node = create_tree_node(pipe, NULL, NULL);
		left_token = sub_left(tmp, pipe);
		right_token = pipe->next;
		node->left = parse_pipes(left_token);
		node->right = parse_pipes(right_token);
		return (node);
	}
	return (parse_paren(token));
}

t_tree	*parse_op(t_token *token)
{
	// write(1, "2\n", 2);
	t_token *opr = get_last_opr(token);
	t_token *left_token = NULL;
	t_token *right_token = NULL;
	t_token *tmp = token;

	if (opr)
	{
		t_tree *node = create_tree_node(opr, NULL, NULL);
		left_token = sub_left(tmp, opr);
		right_token = opr->next;
		node->left = parse_op(left_token);
		node->right = parse_op(right_token);
		return (node);
	}
	return (parse_pipes(token));
}

int calculate_cmd(t_token *token)
{
	t_token *tmp = token;
	int count = 0;
	while(tmp)
	{
		if (tmp->type == 1 || tmp->type == 3 || tmp->type == 4)
			count++;
		if (tmp->type == 5 || tmp->type == 6 || tmp->type == 7 || tmp->type == 8)
			tmp = tmp->next;
		tmp = tmp->next;
	}
	return (count);
}
int	check_errors(t_token *token)
{
	t_token *tmp = token;
	// print_linked_list(tmp);
	if (tmp->type == 2 || tmp->type == 11 || tmp->type == 12){
		printf("bash: syntax error near unexpected token `%s'\n", token->value);
		return (1);
	}
	while(tmp)
	{
		if ((tmp->type == 2 || tmp->type == 5 
			|| tmp->type == 6 || tmp->type == 7 
			|| tmp->type == 8 || tmp->type == 9 
			|| tmp->type == 11 || tmp->type == 12) && !tmp->next)
		{
			printf("bash: syntax error near unexpected token `newline'\n");
			return(1);
		}
		if (tmp->type != 1 && tmp->type != 3 && tmp->type != 4 && tmp->type != 10 && tmp->next && tmp->next->type == 10)
		{
			printf("bash: syntax error near unexpected token `)'\n");
			return(1);
		}
		if ((tmp->type == 2 || tmp->type == 5 
			|| tmp->type == 6 || tmp->type == 7 
			|| tmp->type == 8 || tmp->type == 11 
			|| tmp->type == 12 ) && (tmp->next->type == 2 || tmp->next->type == 5 
			|| tmp->next->type == 6 || tmp->next->type == 7 
			|| tmp->next->type == 8 || tmp->next->type == 11 
			|| tmp->next->type == 12 || tmp->next->value[0] == '&'))
			{
				printf("bash: syntax error near unexpected token `%s'\n", tmp->next->value);
				return (1);
			}
		tmp = tmp->next;
	}
	return 0;
}