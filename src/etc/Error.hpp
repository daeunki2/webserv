#ifndef ERROR_HPP
#define ERROR_HPP

#include <exception>
#include <string>

class Error : public std::exception
{
private:
    std::string _msg;

public:
    /* Canonical */
    Error();
    Error(const std::string &msg, const char *file, int line);
    Error(const Error &o);
    Error &operator=(const Error &o);
    ~Error() throw();

    /* override */
    const char *what() const throw();

private:
    std::string formatMsg(const std::string &msg, const char *file, int line) const;
    std::string toStringLine(int line) const;
};

#endif
