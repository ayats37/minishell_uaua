/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 12:58:09 by ouel-afi          #+#    #+#             */
/*   Updated: 2025/05/22 21:58:20 by taya             ###   ########.fr       */
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
#include <errno.h>

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


int	check_errors(t_token *token);
int calculate_cmd(t_token *token);
void merge_tokens(t_token **tokens);
void	print_linked_list(t_token *token_list);
void join_tokens(t_token *token);
void	print_tree(t_tree *node, int depth, const char *side);
//****************************************parse********************************************************************************
t_lexer	*initialize_lexer(char *input);
int	is_space(t_lexer *lexer);
void	skip_whitespace(t_lexer *lexer);
t_token	*create_token(char *value, char quote, int has_space);
t_token	*handle_quote(t_lexer *lexer, char quote);
t_token	*handle_operations(t_lexer *lexer, char *oper, int i);
t_token	*handle_word(t_lexer *lexer);
t_token	*get_next_token(t_lexer *lexer);
int	cmd_type(t_token *token, int first_cmd);
t_type	token_type(t_token *token);
t_token	*get_next_token(t_lexer *lexer);
void	skip_whitespace(t_lexer *lexer);
t_token	*handle_quote(t_lexer *lexer, char quote);
void	append_token(t_token **head, t_token *token);
t_tree	*parse_op(t_token *token);
//***************************************exec**********************************************
void update_env(char *name, char *value, t_env **env_list);
char *get_env_value(char *name, t_env *env_list);
t_env *init_env(char **env);
t_env *create_env_node(char *env_var);
void add_to_env_list(t_env **head, t_env *new_node);
t_env *init_env(char **envp);
int handle_variable(char *str, t_env *env_list);
int ft_echo(char **cmd, t_env *env_list);
int ft_pwd();
int ft_cd(char **cmd, t_env *envlist);
int ft_unset(char **cmd, t_env **env_list);
int ft_exit(char **cmd, t_env *env_list);
int ft_env(t_env **env_list);
int ft_export(char **cmd, t_env **env_list);
int execute_builtin(t_tree *node, t_env **envlist);
int handle_variable(char *str, t_env *env_list);
int  is_alphanumeric(int c);
char	*find_cmd_path(char *cmd, char **env);
char	**split_cmd(char *cmd);
char	**ft_split(char const *s, char c);
int execute_tree(t_tree *node, char **env, t_env **envlist);
int execute_cmds(char **cmds, char **env, t_tree *node);
void write_error(char *message);
int execute_pipe(t_tree *node, char **env, t_env *envlist);
int is_builtin(char *cmd);
int handle_redirection(t_tree *node);
int  is_alpha(int c);
void env_append(char *name, char *value, t_env **env_list);
t_env *find_env_var(char *name, t_env *env_list);
void    handle_heredoc(t_tree *node);

#endif