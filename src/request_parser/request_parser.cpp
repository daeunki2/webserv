/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_parser.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 16:13:45 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/25 21:11:45 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request_parser.hpp" 
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdlib> // strtoll 사용을 위해 필요
#include <cerrno>  // strtoll 오류 처리를 위해 필요

