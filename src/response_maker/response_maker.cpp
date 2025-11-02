/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response_maker.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 09:48:00 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/30 13:20:42 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response_maker.hpp"

// ******************************************************
//              Constructors & Destructor
// ******************************************************

response_maker::response_maker()
: m_state(BUILD_PENDING)
{

}

response_maker::response_maker(const response_maker& src)
: m_state(src.m_state)
{
	
}

response_maker& response_maker::operator=(const response_maker& src)
{
    if (this != &src)
    {
        m_state = src.m_state;
    }
    return *this;
}

response_maker::~response_maker()
{

}


// ******************************************************
//                        Getter
// ******************************************************

// 현재 응답 생성 상태를 반환합니다.
BuildState response_maker::get_state() const
{
    return m_state;
}

// ******************************************************
//               핵심 로직 (다음 단계에서 구현)
// ******************************************************

