/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 17:41:51 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/29 17:41:58 by daeunki2         ###   ########.fr       */
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

bool isNumber(const std::string &s);
int toInt(const std::string &s);
std::string toString(int n);

#endif
