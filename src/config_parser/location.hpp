/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 15:56:05 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/30 14:07:49 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*============================================================================*/
/*                  store the information in location scoope                  */
/*============================================================================*/

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <string>
# include <vector>
# include <map>
# include <iostream>

class location
{
	private:
	std::string                     m_path;
	std::string                     m_root;
	std::vector<std::string>        m_index_files;
	std::vector<std::string>        m_allowed_methods;
	bool                            m_autoindex;
	bool                           m_upload_enable;
	std::string                     m_upload_store;
	std::map<std::string, std::string> m_cgi_extensions;
	std::string                     m_cgi_pass;
	std::string                     m_auth_basic;
	std::string                     m_auth_user_file;

public:
	location();
	location(const location& src);
	locarion& operator=(const location& src)
	~location();

	// set_ter Methods
	void set_Path(const std::string& p);
	void set_Root(const std::string& r);
	void set_Autoindex(bool state);
	void set_UploadEnable(bool state);
	void set_UploadStore(const std::string& path);
	void set_CgiPass(const std::string& pass);
	void set_AuthBasic(const std::string& state);
	void set_AuthUserFile(const std::string& file);
	
	void addIndexFile(const std::string& f);
	void add_AllowedMethod(const std::string& method);
	void add_CgiExtension(const std::string& ext, const std::string& path);
	bool is_AutoindexEnabled() const;
	bool is_UploadEnabled() const;
	
	// Get_ter Methods (const)
	const std::string& get_Path() const;
	const std::string& get_Root() const;
	const std::vector<std::string>& get_IndexFiles() const;
	const std::vector<std::string>& get_AllowedMethods() const;
	const std::string& get_UploadStore() const;
	const std::map<std::string, std::string>& get_CgiExtensions() const;
	const std::string& get_CgiPass() const;
	const std::string& get_AuthBasic() const;
	const std::string& get_AuthUserFile() const;
};

#endif