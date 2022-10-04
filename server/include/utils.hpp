#pragma once

#include <string>

#include "responses.hpp"

std::string decodeUrl(const std::string &url);
std::string readFile(const std::string &path);
std::vector<std::string> tokenize(const std::string &s, const std::string &del);
std::string StripQueryParams(const std::string &s);

std::string parseMime(const std::string &s);
std::string notImplemented();
std::string notFound();

std::string head();
std::string getDate();
