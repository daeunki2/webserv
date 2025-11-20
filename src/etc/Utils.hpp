#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

/* 문자열 관련 */
std::string trim(const std::string &s);
std::string to_lower(const std::string& s);

/* split */
std::vector<std::string> split(const std::string &s);
std::vector<std::string> split(const std::string &s, char delim);

/* 숫자 관련 */
bool isNumber(const std::string &s);
int toInt(const std::string &s);
std::string toString(int n);

#endif
