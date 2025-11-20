/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 12:28:23 by daeunki2          #+#    #+#             */
/*   Updated: 2025/11/20 10:20:24 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <ctime>
# include "Server.hpp"
# include "http_request.hpp"
# include "request_parser.hpp"
# include "Response_Builder.hpp"

class Client
{
public:
    enum State
    {
        READING,
        REQUEST_COMPLETE,
        SENDING_RESPONSE,
        CONNECTION_CLOSE
    };

    enum ParsingState
    {
        PARSING_INCOMPLETE = 0,
        PARSING_COMPLETED  = 1,
        PARSING_ERROR      = -1
    };

    Client();
    Client(int fd, Server *serverConfig);
    Client(const Client &o);
    Client &operator=(const Client &o);
    ~Client();

    ParsingState handle_recv_data(const char *data, size_t len);
    void         build_response();

    const std::string &get_response_buffer() const;
    size_t            &get_sent_bytes();

    State get_state() const;
    void  update_state(State s);

    void reset();

    int  getFd() const;

    time_t last_active_time;

private:
    int            _fd;
    Server        *_server;
    RequestParser  _parser;
    HttpRequest    _request;

    std::string    _responseBuffer;
    size_t         _sentBytes;

    State          _state;
};

#endif
