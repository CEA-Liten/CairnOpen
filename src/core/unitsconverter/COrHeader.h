#pragma once
#include <string>
#include <vector>
#include <variant>
#include <map>
#include <fstream>
#include <iostream>

#include "json.hpp"
using json = nlohmann::json;
using ojson = nlohmann::ordered_json;

typedef std::map<std::string, int> t_mapKey;
typedef std::map<int, int> t_mapKeyIndex;