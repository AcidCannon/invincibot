// The MIT License (MIT)
//
// Copyright (c) 2017-2020 Alexander Kurbatov

#include "strategies/BracketPush.h"
#include "strategies/MarinePush.h"

#include "tools/Converter.h"

#include <sc2api/sc2_coordinator.h>
#include <sc2api/sc2_gametypes.h>
#include <sc2utils/sc2_arg_parser.h>

#include <iostream>
#include <map>
namespace {
struct Options {
  Options() : GamePort(0), StartPort(0), ComputerOpponent(false) {}

  std::string maps;
  int32_t GamePort;
  int32_t StartPort;
  std::string ServerAddress;
  std::string OpponentId;
  bool ComputerOpponent;
  sc2::Difficulty ComputerDifficulty;
  sc2::Race ComputerRace;
};

void initOutput(std::map<std::string, std::string> &output) {
    output["GamePort"] = "0";
    output["StartPort"] = "0";
    output["LadderServer"] = "";
    output["ComputerOpponent"] = "False";
    output["ComputerRace"] = "";
    output["ComputerDifficulty"] = "";
    output["OpponentId"] = "";
    output["Map"] = "BelShirVestigeLE.SC2Map";
}

void ParseArguments(int argc, char *argv[], Options *options_, std::map<std::string, std::string> &output) {
    sc2::ArgParser arg_parser(argv[0]);
    arg_parser.AddOptions(
        {{"-g", "--GamePort", "Port of client to connect to", false},
         {"-o", "--StartPort", "Starting server port", false},
         {"-l", "--LadderServer", "Ladder server address", false},
         {"-c", "--ComputerOpponent", "If we set up a computer opponent", false},
         {"-a", "--ComputerRace", "Race of computer opponent", false},
         {"-d", "--ComputerDifficulty", "Difficulty of computer opponent", false},
         {"-x", "--OpponentId", "PlayerId of opponent", false},
         {"-m", "--Map", "Map to play on against computer opponent", false}
        });

    arg_parser.Parse(argc, argv);

    std::string MapId;
    if (arg_parser.Get("Map", MapId)) {
        output["Map"] = options_->maps = convert::StringToMap(MapId, output);
    }

    std::string CompOpp;
    if (arg_parser.Get("ComputerOpponent", CompOpp)) {
        options_->ComputerOpponent = true;
        output["ComputerOpponent"] = "True";
        std::string CompRace;

        if (arg_parser.Get("ComputerRace", CompRace)) {
            options_->ComputerRace = convert::StringToRace(CompRace, output);
        } else {
            // Random
            options_->ComputerRace = convert::StringToRace(" ", output);;
        }

        std::string CompDiff;
        if (arg_parser.Get("ComputerDifficulty", CompDiff)) {
            options_->ComputerDifficulty =
                convert::StringToDifficulty(CompDiff, output);
        } else {
            // Easy
            options_->ComputerDifficulty =
                convert::StringToDifficulty(" ", output);
        }
    } else {
        std::string GamePortStr;
        if (arg_parser.Get("GamePort", GamePortStr)) {
            output["GamePort"] = GamePortStr;
            options_->GamePort = atoi(GamePortStr.c_str());
        }

        std::string StartPortStr;
        if (arg_parser.Get("StartPort", StartPortStr)) {
            output["GamePort"] = StartPortStr;
            options_->StartPort = atoi(StartPortStr.c_str());
        }

        arg_parser.Get("LadderServer", options_->ServerAddress);
        output["LadderServer"] = options_->ServerAddress;
        std::string OpponentId;
        if (arg_parser.Get("OpponentId", OpponentId)) {
            options_->OpponentId = output["OpponentId"] = OpponentId;
        }
    }
}
}

int main(int argc, char *argv[]) {
    sc2::Coordinator coordinator;
    coordinator.LoadSettings(argc, argv);

    size_t num_agents;
    MarinePush player;
    const sc2::Race PlayerRace = sc2::Race::Terran;

    Options options;
    std::map<std::string, std::string> output;
    initOutput(output);
    ParseArguments(argc, argv, &options, output);

    for (const auto &kv:output) {
        printf("%-20s: %s\n", kv.first.c_str(), kv.second.c_str());
    }

    if (options.ComputerOpponent) {
        num_agents = 1;
        coordinator.SetParticipants({
                                        CreateParticipant(PlayerRace, &player, "Invincibot"),
                                        CreateComputer(options.ComputerRace, options.ComputerDifficulty)});
        coordinator.LoadSettings(1, argv);
        coordinator.LaunchStarcraft();
        coordinator.StartGame(options.maps);
    } else {
        num_agents = 2;
        coordinator.SetParticipants({
                                        CreateParticipant(PlayerRace, &player, "Invincibot")
                                    });
        std::cout << "Connecting to port " << options.GamePort << "....." << std::endl;
        coordinator.Connect(options.GamePort);
        coordinator.SetupPorts(num_agents, options.StartPort, false);
        // coordinator.SetRawAffectsSelection(true);
        coordinator.JoinGame();
    }

    coordinator.SetTimeoutMS(10000);
    std::cout << "Successfully joined game" << std::endl;
    while (coordinator.Update()) {
    }
    return 0;
}


