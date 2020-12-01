// The MIT License (MIT)
//
// Copyright (c) 2017-2020 Alexander Kurbatov

#include "tools/Converter.h"
#include <map>

namespace convert {

/** Convert Functions **/
sc2::Race StringToRace(const std::string &name_, std::map<std::string, std::string> &output) {
    std::string race(name_);
    std::transform(race.begin(), race.end(), race.begin(), ::tolower);

    output["ComputerRace"] = race;

    if (race == "terran")
        return sc2::Race::Terran;

    if (race == "protoss")
        return sc2::Race::Protoss;

    if (race == "zerg")
        return sc2::Race::Zerg;

    if (race == "random")
        return sc2::Race::Random;

    output["ComputerRace"] = "Random";
    return sc2::Race::Random;
}

sc2::Difficulty StringToDifficulty(const std::string &name_, std::map<std::string, std::string> &output) {
    output["ComputerDifficulty"] = name_;
    if (name_ == "VeryEasy")
        return sc2::Difficulty::VeryEasy;

    if (name_ == "Easy")
        return sc2::Difficulty::Easy;

    if (name_ == "Medium")
        return sc2::Difficulty::Medium;

    if (name_ == "MediumHard")
        return sc2::Difficulty::MediumHard;

    if (name_ == "Hard")
        return sc2::Difficulty::Hard;

    if (name_ == "HardVeryHard")
        return sc2::Difficulty::HardVeryHard;

    if (name_ == "VeryHard")
        return sc2::Difficulty::VeryHard;

    if (name_ == "CheatVision")
        return sc2::Difficulty::CheatVision;

    if (name_ == "CheatMoney")
        return sc2::Difficulty::CheatMoney;

    if (name_ == "CheatInsane")
        return sc2::Difficulty::CheatInsane;
    output["ComputerDifficulty"] = "Easy";
    return sc2::Difficulty::Easy;
}

std::string StringToMap(const std::string& IndexStr, std::map<std::string, std::string> &output) {
    const std::string maps[3] = {"BelShirVestigeLE.SC2Map", "CactusValleyLE.SC2Map",
                                 "ProximaStationLE.SC2Map"};
    return output["Map"] = maps[atoi(IndexStr.c_str())];
}

}  // namespace convert
