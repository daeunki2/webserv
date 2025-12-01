/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 15:56:05 by daeunki2          #+#    #+#             */
/*   Updated: 2025/12/01 14:41:55 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
/*                  store the information in location scoope                  */
/*============================================================================*/

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <vector>

class Location
{
private:
    std::string _path;
    std::string _root;
    std::vector<std::string> _methods;  // GET, POST, DELETE
    std::string _index;
    bool _autoindex;
    std::string _uploadPath;

    // redirect
    bool _isRedirect;
    int _redirectCode;
    std::string _redirectUrl;

	size_t _clientMaxBodySize;
	bool   _hasClientMaxBodySize;
    // CGI
    std::string _cgiExtension;
    std::string _cgiPath;

public:
    /* Canonical form */
    Location();
    Location(const std::string &path);
    Location(const Location &o);
    Location &operator=(const Location &o);
    ~Location();

    /* Setters */
    void setRoot(const std::string &r);
    void addMethod(const std::string &m);
    void setIndex(const std::string &idx);
    void setAutoindex(bool enable);
    void setUploadPath(const std::string &p);

	void setClientMaxBodySize(size_t size);


    void setRedirect(int code, const std::string &url);
    void setCgi(const std::string &ext, const std::string &path);

    /* Getters */
    const std::string &getPath() const;
    const std::string &getRoot() const;
    const std::vector<std::string> &getMethods() const;
    const std::string &getIndex() const;
    bool getAutoindex() const;
    const std::string &getUploadPath() const;

    bool isRedirect() const;
    int getRedirectCode() const;
    const std::string &getRedirectUrl() const;

	bool   hasClientMaxBodySize() const;
	size_t getClientMaxBodySize() const;
	
    const std::string &getCgiExtension() const;
    const std::string &getCgiPath() const;
};

#endif
