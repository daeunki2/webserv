/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_parser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 16:13:32 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/18 20:42:58 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*

server {
    listen 8080;
    host 0.0.0.0;
    server_name localhost;
    root www/localhost;
    
    error 404 404.html;
    client_max_body_size 1M;

    location {
        path /;
        index index.html;
        listing true;
        method GET,POST;
    }
}

*/


/*
logic
0. there is 3 status. in_server, in_location, none
1. read from the start to eod.
2. when I find "server {", change status_in_server and start a small function to store server info.
2-1. when I find "location {" start small funtion for store locaion info.
2-2. store informations.
2-3. when i find"}" stop a function for location and chage status to in_server
2-4. when I find "}", stop a function for server and change status to none
3. reapat this till i see eof.

idea.
1. i need a big class parser, and store the value in server and location 
2. in server class, i have location as a list or vector(advanced array).
*/