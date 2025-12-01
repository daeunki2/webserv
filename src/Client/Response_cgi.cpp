/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response_cgi.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/01 18:37:46 by daeunki2          #+#    #+#             */
/*   Updated: 2025/12/01 18:41:41 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response_Builder.hpp"
#include <unistd.h>
#include <sys/wait.h>

bool Response_Builder::isCgiRequest(const Location* loc, const std::string& path) const
{
	
}

char** Response_Builder::buildCgiEnv() const
{
	
}

std::string Response_Builder::handleCgi(const Location* loc)
{
	
}

std::string Response_Builder::buildHttpResponseFromCgi(const std::string& cgiOutput)
{
	
}
