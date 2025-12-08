/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 17:41:51 by daeunki2          #+#    #+#             */
/*   Updated: 2025/12/05 17:56:13 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

std::string trim(const std::string &s);
std::string to_lower(const std::string& s);

std::vector<std::string> split(const std::string &s);
std::vector<std::string> split(const std::string &s, char delim);

char *ft_strdup(const char *str);

bool isNumber(const std::string &s);
int toInt(const std::string &s);
long long toLLong(const std::string &s);
std::string toString(int n);

#endif
