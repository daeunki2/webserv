/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 15:56:01 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/30 14:20:18 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
/*                  store the information in server scoope                    */
/*============================================================================*/

#ifndef SERVER_HPP
# define SERVER_HPP

# include "location.hpp"
# include <string>
# include <vector>
# include <map>
# include <iostream>

class server
{
private:
    int                             m_port;
    std::string                     m_host;
    std::string                     m_server_name;
    std::string                     m_root;
    std::vector<std::string>        m_index_files;
    std::map<int, std::string>      m_error_pages; // code -> path
    long                            m_client_max_body_size; // bytes
    std::vector<location>     m_locations;

public:
    server();
	server(const server& erc);
	server& operator&(const server& src);
    ~server();

    void set_Port(int p);
    void set_Host(const std::string& h);
    void set_ServerName(const std::string& name);
    void set_Root(const std::string& r);
    void addIndexFile(const std::string& f);
    void set_ErrorPage(int code, const std::string& path);
    void set_ClientMaxBodySize(long size);
    void addLocation(const LocationConfig& loc);
    
    void set_Listen(const std::string& val); 

    int get_Port() const;
    const std::string& get_Host() const;
    const std::string& get_ServerName() const;
    const std::string& get_Root() const;
    const std::vector<std::string>& get_IndexFiles() const;
    const std::map<int, std::string>& get_ErrorPages() const;
    long get_ClientMaxBodySize() const;
    const std::vector<LocationConfig>& get_Locations() const;
};

#endif