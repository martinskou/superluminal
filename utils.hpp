//
// Created by Martin Drewes on 18/10/2020.
//

#ifndef SUPERLUMINAL_UTILS_HPP
#define SUPERLUMINAL_UTILS_HPP

#include <vector>
#include <iostream>
#include <map>


std::string to_upper(std::string strToConvert);
std::string to_lower(std::string strToConvert);

void trim(std::string &s);
std::string trim_copy(std::string s);

std::vector<std::string> split(const std::string& s, char seperator);
std::vector<std::string> split(const std::string& s, std::string seperator);


#endif //SUPERLUMINAL_UTILS_HPP
