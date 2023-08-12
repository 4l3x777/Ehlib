#pragma once
#include <string>

std::string hex_encode(char const c);
std::string url_encode(const std::string& str);
std::string base64_encode(const std::string& in);
std::string generate_random_boundary();