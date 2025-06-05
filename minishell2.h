/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ouel-afi <ouel-afi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 12:58:09 by ouel-afi          #+#    #+#             */
/*   Updated: 2025/05/13 19:55:52 by ouel-afi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

#include "./libft/libft.h"
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <signal.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/types.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>

#define MAX_PIPES 1024

typedef struct s_lexer {
	int position;
	int lenght;
	char *input;
}	t_lexer;

typedef enum s_type {
    CMD = 1,
	PIPE,	
	SINGLE_QUOTE,
	DOUBLE_QUOTE,
    REDIR_IN,
    REDIR_OUT,
    APPEND,
    HEREDOC,
    OPEN_PAREN,
    CLOSE_PAREN,
	OR,		
	AND
} t_type;

typedef struct s_token {
	char	*value;
	t_type		type;
	int			has_space;
	struct s_token *next;
}	t_token;

typedef struct s_tree {
	t_token *redir;
	t_type type;
	int		has_space;
	char *value;
	struct s_tree	*left;
	struct s_tree	*right;
	char **cmd;
}	t_tree;

typedef struct s_env
{
	char *name;
	char *value;
	struct s_env *next;
	char **env;
}	t_env;

//****************************************parse********************************************************************************

t_lexer	*initialize_lexer(char *input);
t_token	*get_next_token(t_lexer *lexer);
t_token	*handle_quote(t_lexer *lexer, char quote);
t_token	*handle_operations(t_lexer *lexer, char *oper, int i);
t_token	*handle_word(t_lexer *lexer);
t_token	*create_token(char *value, char quote, int has_space);
t_type	token_type(t_token *token);
void	append_token(t_token **head, t_token *token);
int		check_parenthesis(t_token *token);
void	merge_tokens(t_token **tokens);
int		check_errors(t_token *token);
t_tree	*parse_op(t_token *token);
int		calculate_cmd(t_token *token);
void	print_linked_list(t_token *token_list);
void	print_tree(t_tree *node, int depth, const char *side);
t_tree	*create_tree_node(t_token *token, char **cmd, t_token *redir);
t_token	*sub_left(t_token *token, t_token *opr);
t_tree	*parse_pipes(t_token *token);
t_tree	*parse_paren(t_token *token);
t_tree	*parse_cmd(t_token *token);
void	print_linked_list(t_token *token_list);
void	print_tree(t_tree *node, int depth, const char *side);
int		is_space(t_lexer *lexer);
int		check_before_open_paren(t_token *token);
void ll();

//***************************************exec**********************************************
void update_env(char *name, char *value, t_env **env_list);
char *get_env_value(char *name, t_env *env_list);
t_env *init_env(char **env);
t_env *create_env_node(char *env_var);
void add_to_env_list(t_env **head, t_env *new_node);
t_env *init_env(char **envp);
int handle_variable(char *str, t_env *env_list);
int ft_echo(t_token *token_list, t_env *env_list);
int ft_pwd();
int ft_cd(t_token *path);
int ft_unset(t_token *token, t_env **env_list);
int ft_exit(t_token *token, t_env *env_list);
int ft_env(t_env *env_list);
int ft_export(t_token *token, t_env **env_list);
int execute_builtin(t_token *token, t_env **envlist);
int handle_variable(char *str, t_env *env_list);
int  is_alphanumeric(int c);
char	*find_cmd_path(char *cmd, char **env);
char	**split_cmd(char *cmd);
char	**ft_split(char const *s, char c);
// int execute_tree(t_tree *node, char **env, t_env *envlist);
int execute_tree(t_tree *node, char **env, t_env *envlist, t_token *token);
int execute_cmds(char **cmds, char **env);
void write_error(char *message);
// int execute_pipe(t_tree *node, char **env, t_env *envlist);
int execute_pipe(t_tree *node, char **env, t_env *envlist, t_token *token);
int is_builtin(char *cmd);

#endif