// The MIT License (MIT)
//
// Copyright (c) 2017-2020 Alexander Kurbatov

#pragma once

#include <string>

#include <sc2api/sc2_gametypes.h>
#include <sc2api/sc2_typeenums.h>
#include <map>
// The collection of type conversion utilities
namespace convert {

sc2::Difficulty StringToDifficulty(const std::string& name_, std::map<std::string, std::string> &output);
sc2::Race StringToRace(const std::string& name_, std::map<std::string, std::string> &output);
std::string StringToMap(const std::string& IndexStr, std::map<std::string, std::string> &output);

}
