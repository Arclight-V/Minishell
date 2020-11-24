/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anatashi <anatashi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/20 12:38:26 by anatashi          #+#    #+#             */
/*   Updated: 2020/11/20 16:22:30 by anatashi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"
#include "parser.h"

int			redirects(t_commands *redir)
{
	int			fd;

	while (redir->redir)
	{
		if (redir->type_redir == 1 && !redir->redir->invalid)
		{
			if ((fd = open(redir->redir->cmd, O_RDWR)) == -1)
				return (1);
		}
		else
		{
			if (redir->type_redir == 2 && !redir->redir->invalid)
				fd = open(redir->redir->cmd, O_CREAT |
							O_RDWR | O_TRUNC, S_IRWXU);
			if (redir->type_redir == 3 && !redir->redir->invalid)
				fd = open(redir->redir->cmd, O_CREAT |
							O_RDWR | O_APPEND, S_IRWXU);
		}
		dup2(fd, (redir->type_redir == 1) ? 0 : 1);
		redir = redir->redir;
	}
	return (0);
}

void			give_env(t_data *data)
{
	int			i;

	i = -1;
	while (data->envp[++i])
	{
		if (ft_strchr(data->envp[i], '='))
		{
			ft_putendl(data->envp[i]);
		}
	}
}

void			selection_cmd(t_commands *cmd, t_data *data,
								t_commands *redirect, t_commands *pip)
{
	// #if 0
	(void)data;
	(void)pip;
	// #endif

	if (redirects(redirect))
	{
		return;
	}
	if (cmd->num_cmd == CMD_PWD)
		give_pwd(cmd, data);
	else if (cmd->num_cmd == CMD_ENV)
		give_env(data);
	
}

void			execute_cmd_line(t_commands *cmd, t_data *data)
{
	int			fd[2];
	t_commands	*pip;
	t_commands	*redirect;

	if (cmd->cmd)
	{
		pip = cmd;
		redirect = cmd;
		if (pip->pipe)
		{
			pipe(fd);
			dup2(fd[1], 1);
		}
		selection_cmd(cmd, data, pip, redirect);
	}
}

void	executor(t_commands *syntax_tree, t_data *data)
{
	if (syntax_tree && syntax_tree->invalid != CMD_ERROR)
	{
		while (syntax_tree && syntax_tree->invalid != CMD_ERROR)
		{
			execute_cmd_line(syntax_tree, data);
			syntax_tree = syntax_tree->next;
			dup2(data->fd_1, 1);
			dup2(data->fd_0, 0);
		}
	}
	else
	{
		ft_putendl("Invalid command");
		ft_free_tmp(data->error_string);
		if (!(data->error_string =ft_strdup("127")))
			error_output(syntax_tree, data, NULL);
	}	
}