/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response_cgi.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/01 18:37:46 by daeunki2          #+#    #+#             */
/*   Updated: 2025/12/02 16:45:36 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response_Builder.hpp"
#include <unistd.h>
#include <sys/wait.h>

bool Response_Builder::isCgiRequest(const Location* loc, const std::string& path) const
{
	if (loc != nullptr && loc->hasCgi() && loc->getCgiPath() != nullptr)
		return true;
	return false;
}

char** Response_Builder::buildCgiEnv() const
{
	char **env = (char**)malloc(8 * sizeof(char*)); // modifi the number

	env[0] = ft_strdup(""); // value(str) from location, or http request. 
	env[1] = ft_strdup("");
	env[2] = ft_strdup("");
	env[3] = ft_strdup("");
	env[4] = ft_strdup("");
	env[5] = ft_strdup("");
	env[6] = ft_strdup("");
	env[7] = NULL;

	return env;
}

void Response_Builder::freeEnv(char **envp) const
{
    for (int i = 0; envp[i] != NULL; i++)
	{
        free(envp[i]);
    }
    free(envp);
}

std::string Response_Builder::handleCgi(const Location* loc)
{
	/*
	fork 
	pipe
	child
		execv(char ** path)
		{
			free()
		}
	father
	{
		send a response
	}
	*/
}

std::string Response_Builder::buildHttpResponseFromCgi(const std::string& cgiOutput)
{
	/*
		write in "HTTP" msg.
	*/
}
