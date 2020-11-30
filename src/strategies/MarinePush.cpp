// The MIT License (MIT)
//
// Copyright (c) 2020 Qian Yu, Zijian Xi, Zihao Huang

#include "MarinePush.h"
#include "tools/BuilderOrder.h"
#include <iostream>
#include <sc2api/sc2_api.h>
#include <sc2lib/sc2_lib.h>
#include <sc2api/sc2_unit_filters.h>
#include <vector>



void MarinePush::OnGameStart() {
    // ClientEvents::OnGameStart();
    Actions()->SendChat("Break a leg :)");

    // get start location and expansions
    startLocation_ = Observation()->GetStartLocation();
    expansions_ = sc2::search::CalculateExpansionLocations(Observation(), Query());
    
}

void MarinePush::OnStep() {
    // ClientEvents::OnStep();
    CountUnitNumber();
    TryBuildSupplyDepot();
    TryBuildBarracks();
    TryBuildRefinery();
    CollectVespene();
    TryBuildBarrackTechLab();
    TryBuildFactory();
    TryBuildEngineeringBay();
    TryBuildArmory();
    TryAttack();
    TryLowerSupplyDepot();
    TryUpgradeToOrbitalCommand();
    TryBuildExpansionCommandCenter();
}

void MarinePush::OnUnitIdle(const sc2::Unit *unit) {
    switch (unit->unit_type.ToType()) {
        case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER: {
            GatheringPoint = sc2::Point2D(unit->pos.x, unit->pos.y);
            CommandCentrePoint = sc2::Point2D(unit->pos.x, unit->pos.y);
            // std::cout << "Command centre coord: "<< unit->pos.x << " " << unit->pos.y << std::endl;
            if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_SCV) < 22) {
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_SCV);
            }
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_SCV: {
            const sc2::Unit *mineral_target =
                FindNearestMineralPatch(unit->pos);
            if (!mineral_target) {
                break;
            }
            Actions()->UnitCommand(unit, sc2::ABILITY_ID::SMART,
                                   mineral_target);
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_BARRACKS: {
            Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_MARINE);
            if (IfTrainReaper()) {
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_REAPER);
            } else if (IfUpgradeBarrack()) {
                // Actions()->UnitCommand(unit, sc2::ABILITY_ID::BUILD_REACTOR);
                // Build Marauder
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_MARAUDER);
            }
            else {
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_MARINE);
            }
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_MARINE: {
            if (if_soldier_rush) {
                //priori to attack main structure
                const sc2::Unit* enemy_unit = nullptr;
                if (FindEnemyMainStructure(Observation(), enemy_unit)) {
                    Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK, enemy_unit);
                    break;
                }
                const sc2::GameInfo &game_info = Observation()->GetGameInfo();
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK,
                                       game_info.enemy_start_locations.front());
                if_soldier_rush = false;
            } else {
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::SMART,
                                       GatheringPoint);
            }
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_MARAUDER: {
            if (if_soldier_rush) {
                const sc2::GameInfo &game_info = Observation()->GetGameInfo();
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK,
                                       game_info.enemy_start_locations.front());
                if_soldier_rush = false;
            } else {
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::SMART,
                                       GatheringPoint);
            }
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_REAPER: {
            if (if_soldier_rush) {
                const sc2::GameInfo &game_info = Observation()->GetGameInfo();
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK,
                                       game_info.enemy_start_locations.front());
            } else {
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::SMART,
                                       GatheringPoint);
            }
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_FACTORY: {
            // if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_WIDOWMINE) < 4) {
            //     Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_WIDOWMINE);
            // }
            Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_HELLION);
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_WIDOWMINE: {
            Actions()->UnitCommand(unit, sc2::ABILITY_ID::BURROWDOWN_WIDOWMINE);
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_HELLION: {
            if (if_vehicle_rush) {
                if_vehicle_rush = false;
                //priori to attack main structure
                const sc2::Unit* enemy_unit = nullptr;
                if (FindEnemyMainStructure(Observation(), enemy_unit)) {
                    Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK, enemy_unit);
                    break;
                }
                const sc2::GameInfo &game_info = Observation()->GetGameInfo();
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK,
                                       game_info.enemy_start_locations.front());
            } else {
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::SMART,
                                       GatheringPoint);
            }
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY: {
            Actions()->UnitCommand(unit, sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMOR);
            //Actions()->UnitCommand(unit, sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS);
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_ARMORY: {
            Actions()->UnitCommand(unit, sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS);
            break;
        }
        default: {
            break;
        }
    }
}

bool MarinePush::FindEnemyMainStructure(const sc2::ObservationInterface* observation, const sc2::Unit*& enemy_unit) {
    sc2::Units enemy_units = observation->GetUnits(sc2::Unit::Alliance::Enemy);
    for (const auto unit : enemy_units) {
        if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER ||
            unit->unit_type == sc2::UNIT_TYPEID::PROTOSS_NEXUS ||
            unit->unit_type == sc2::UNIT_TYPEID::ZERG_HATCHERY) {
            enemy_unit = unit;
            return true;
        }
    }

    return false;
}

bool MarinePush::IfUpgradeBarrack() {
    const sc2::ObservationInterface *observation = Observation();

    if (num_of_terran_scv < 20) {
        return false;
    }
    return true;
}

bool MarinePush::IfTrainReaper() {
    const sc2::ObservationInterface *observation = Observation();
    if (observation->GetMinerals() <= 500 || num_of_terran_marine < 15) {
        return false;
    }
    return true;
}

size_t MarinePush::CountUnitType(sc2::UNIT_TYPEID unit_type) {
    size_t count = 0;

    sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
    for (auto &unit : units) {
        if (unit->unit_type == unit_type) {
            count++;
        }
    }
    return count;
}

void MarinePush::TryAttack() {
    if (num_of_terran_marine > 40) {
        if_soldier_rush = true;
        if_vehicle_rush = true;
    }
}

bool MarinePush::TryBuildStructureConcurrent(sc2::ABILITY_ID ability_type_for_structure,
                            sc2::UNIT_TYPEID unit_type) {
    const sc2::ObservationInterface *observation = Observation();

    // Get an scv to build the structure.
    const sc2::Unit *unit_to_build = nullptr;
    sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self);
    for (const auto &unit : units) {
        bool skip = false;
        for (const auto &order : unit->orders) {
            if (order.ability_id == ability_type_for_structure) {
                skip = true;
                break;
            }
        }

        if (unit->unit_type == unit_type && !skip) {
            unit_to_build = unit;
        }
    }

    float rx = sc2::GetRandomScalar();
    float ry = sc2::GetRandomScalar();

    Actions()->UnitCommand(unit_to_build, ability_type_for_structure,
                           sc2::Point2D(unit_to_build->pos.x + rx * 15.0f,
                                        unit_to_build->pos.y + ry * 15.0f));

    return true;
}

bool MarinePush::TryBuildStructure(sc2::ABILITY_ID ability_type_for_structure,
                            sc2::UNIT_TYPEID unit_type) {
    const sc2::ObservationInterface *observation = Observation();

    // If a unit already is building a supply structure of this type, do
    // nothing. Also get an scv to build the structure.
    const sc2::Unit *unit_to_build = nullptr;
    sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self);
    for (const auto &unit : units) {
        for (const auto &order : unit->orders) {
            if (order.ability_id == ability_type_for_structure) {
                return false;
            }
        }

        if (unit->unit_type == unit_type) {
            unit_to_build = unit;
        }
    }

    float rx = sc2::GetRandomScalar();
    float ry = sc2::GetRandomScalar();

    Actions()->UnitCommand(unit_to_build, ability_type_for_structure,
                           sc2::Point2D(unit_to_build->pos.x + rx * 15.0f,
                                        unit_to_build->pos.y + ry * 15.0f));

    return true;
}

bool MarinePush::TryBuildSupplyDepot() {
    const sc2::ObservationInterface *observation = Observation();

    // If we are not supply capped, don't build a supply depot.
    if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2)
        return false;

    // Try and build a depot. Find a random SCV and give it the order.
    return TryBuildStructure(sc2::ABILITY_ID::BUILD_SUPPLYDEPOT);
}

const sc2::Unit *MarinePush::FindNearestMineralPatch(const sc2::Point2D &start) {
    sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Neutral);
    float distance = std::numeric_limits<float>::max();
    const sc2::Unit *target = nullptr;
    for (const auto &u : units) {
        if (u->unit_type == sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD) {
            float d = DistanceSquared2D(u->pos, start);
            if (d < distance) {
                distance = d;
                target = u;
            }
        }
    }
    return target;

}

bool IsVespeneGeyser(const sc2::Unit* unit) {
    // Maybe exist several vespene geysers can be used.
    auto type_of_unit = unit->unit_type.ToType();
    if (type_of_unit == sc2::UNIT_TYPEID::NEUTRAL_VESPENEGEYSER ||
        type_of_unit == sc2::UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER ||
        type_of_unit == sc2::UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER) {
        return true;
    }
    return false;
}

const sc2::Unit *MarinePush::FindNearestVespeneGeyser(const sc2::Point2D &start) {
    // Similar to FindNearestMineralPatch.
    sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Neutral);
    float distance = std::numeric_limits<float>::max();
    const sc2::Unit *target = nullptr;
    for (const auto &u : units) {
        if (IsVespeneGeyser(u)) {
            float d = DistanceSquared2D(u->pos, start);
            if (d < distance) {
                distance = d;
                target = u;
            }
        }
    }
    return target;
}

bool MarinePush::TryBuildRefineryNearby() {
    const sc2::ObservationInterface *observation = Observation();

    const sc2::Unit *unit_to_build = nullptr;
    sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self);
    for (const auto &unit : units) {
        for (const auto &order : unit->orders) {
            if (order.ability_id == sc2::ABILITY_ID::BUILD_REFINERY) {
                return false;
            }
        }

        if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_SCV) {
            unit_to_build = unit;
        }
    }
    // find vespene geyser near by command centre
    const sc2::Unit *vespene_target = FindNearestVespeneGeyser(CommandCentrePoint);
    if (!vespene_target) {
        return false;
    }
    // std::cout << "Vespene Geyser coord: " << vespene_target->pos.x << " " <<vespene_target->pos.y << " " << vespene_target->vespene_contents << std::endl;
    Actions()->UnitCommand(unit_to_build, sc2::ABILITY_ID::BUILD_REFINERY, vespene_target);

    return true;
}

bool MarinePush::TryBuildBarracks() {
    if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
        return false;
    }

    if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS) > 3) {
        return false;
    }

    return TryBuildStructureConcurrent(sc2::ABILITY_ID::BUILD_BARRACKS);
}

bool MarinePush::TryBuildRefinery() {
    if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS) < 1) {
        return false;
    }

    if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_REFINERY) > 1) {
        return false;
    }

    return TryBuildRefineryNearby();
}

bool MarinePush::TryBuildFactory() {
    if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_REFINERY) < 2) {
        return false;
    }

    if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_FACTORY) > 0) {
        return false;
    }

    return TryBuildStructure(sc2::ABILITY_ID::BUILD_FACTORY);
}

void MarinePush::CountUnitNumber() {
    // get total count of different units
    num_of_terran_marine = CountUnitType(sc2::UNIT_TYPEID::TERRAN_MARINE);
    num_of_terran_reaper = CountUnitType(sc2::UNIT_TYPEID::TERRAN_REAPER);
    num_of_terran_scv = CountUnitType(sc2::UNIT_TYPEID::TERRAN_SCV);
    num_of_terran_widowmine = CountUnitType(sc2::UNIT_TYPEID::TERRAN_WIDOWMINE);
}

bool IsCarryingMineral(const sc2::Unit* unit) {
    // Maybe exist several minerals being carried.
    for(const auto &buff : unit->buffs) {
        if (buff == sc2::BUFF_ID::CARRYMINERALFIELDMINERALS ||
            buff == sc2::BUFF_ID::CARRYHIGHYIELDMINERALFIELDMINERALS) {
            return true;
        }
    }

    return false;
}

std::vector<sc2::Unit> MarinePush::FindUnderMiningRefinery() {
    sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);

    std::vector<sc2::Unit> refineries;

    // Find all refineries that no more than 5 scvs working on it.
    for (const auto &unit : units) {
        if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_REFINERY &&
            unit->build_progress >= 1.0f &&
            unit->vespene_contents > 0 &&
            unit->assigned_harvesters < unit->ideal_harvesters) {
                refineries.push_back(*unit);
        }
    }

    return refineries;
}

std::vector<sc2::Unit> MarinePush::FindMiningScvs() {
    sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);

    std::vector<sc2::Unit> mining_scvs;

    // Find all scvs currently harvesting minerals.
    for (const auto &unit : units) {
        if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_SCV) {
            for (auto const &order : unit->orders) {
                if (order.ability_id == sc2::ABILITY_ID::HARVEST_GATHER) {
                    mining_scvs.push_back(*unit);
                }
            }
        }
    }

    return mining_scvs;
}

void MarinePush::CollectVespene() {
    sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);

    std::vector<sc2::Unit> refineries = FindUnderMiningRefinery();
    std::vector<sc2::Unit> mining_scvs = FindMiningScvs();

    if (refineries.empty() || mining_scvs.empty()) {
        return;
    }

    for(const auto &refinery: refineries) {
        // std::cout << refinery.vespene_contents << std::endl;
        // Assign more SCVs to collect vespene.
        while (refinery.assigned_harvesters < refinery.ideal_harvesters &&
               !mining_scvs.empty()) {
            Actions()->UnitCommand(&mining_scvs.back(), sc2::ABILITY_ID::HARVEST_GATHER, &refinery);
            return;
        }
    }    
}

bool MarinePush::TryBuildEngineeringBay() {
    if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_FACTORY) < 1) {
        return false;
    }

    if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY) > 0) {
        return false;
    }

    return TryBuildStructure(sc2::ABILITY_ID::BUILD_ENGINEERINGBAY);
}

bool MarinePush::TryBuildArmory() {
    if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_FACTORY) < 1) {
        return false;
    }

    if(CountUnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY) > 0) {
        return false;
    }

    return TryBuildStructure(sc2::ABILITY_ID::BUILD_ARMORY);
}


// From HZH
void MarinePush::TryLowerSupplyDepot() {
    const sc2::ObservationInterface *observation = Observation();

    if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT) > 0) {
        sc2::Units supply_depots = observation->GetUnits(sc2::Unit::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT));

        for (const auto& supply_depot : supply_depots) {
            Actions()->UnitCommand(supply_depot, sc2::ABILITY_ID::MORPH_SUPPLYDEPOT_LOWER);
        }
    }

}

void MarinePush::TryBuildBarrackTechLab() {
    const sc2::ObservationInterface *observation = Observation();
    // sc2::Units barracks_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB));

    sc2::Units barracks = observation->GetUnits(sc2::Unit::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_BARRACKS));
    for (const auto& barrack : barracks) {
        if(IfUpgradeBarrack() && CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS) > 1 && CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB) < 3) {
            float rx = sc2::GetRandomScalar();
            float ry = sc2::GetRandomScalar();
            Actions()->UnitCommand(barrack, sc2::ABILITY_ID::BUILD_TECHLAB_BARRACKS,
                                   sc2::Point2D(barrack->pos.x + rx * 15.0f,
                                                barrack->pos.y + ry * 15.0f));
        } 
    }
}

void MarinePush::TryUpgradeToOrbitalCommand() {
    const sc2::ObservationInterface *observation = Observation();

    if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS) > 1) {
        sc2::Units bases = observation->GetUnits(sc2::Unit::Self, sc2::IsTownHall());

        for (const auto& base : bases) {
            if (base->unit_type == sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER && observation->GetMinerals() > 150) {

                Actions()->UnitCommand(base, sc2::ABILITY_ID::MORPH_ORBITALCOMMAND);
            }
            
        }
    }

}

bool MarinePush::TryExpand(sc2::AbilityID build_ability, sc2::UNIT_TYPEID unit_type) {
    const sc2::ObservationInterface* observation = Observation();
    float minimum_distance = std::numeric_limits<float>::max();
    sc2::Point3D closest_expansion;
    for (const auto& expansion : expansions_) {
        float current_distance = Distance2D(startLocation_, expansion);
        if (current_distance < .01f) {
            continue;
        }

        if (current_distance < minimum_distance) {
            if (Query()->Placement(build_ability, expansion)) {
                closest_expansion = expansion;
                minimum_distance = current_distance;
            }
        }
    }

    // If a unit already is building, do nothing

    const sc2::Unit *unit_to_build = nullptr;
    sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self);
    for (const auto &unit : units) {
        for (const auto &order : unit->orders) {
            if (order.ability_id == build_ability) {
                return false;
            }
        }

        if (unit->unit_type == unit_type) {
            unit_to_build = unit;
        }
    }
    
    // Check to see if unit can build there
    if (Query()->Placement(build_ability, closest_expansion)) {
        Actions()->UnitCommand(unit_to_build, build_ability, closest_expansion);
        return true;
    }
    
    return false;

}

bool MarinePush::TryBuildExpansionCommandCenter() {
    const sc2::ObservationInterface* observation = Observation();
    sc2::Units bases = observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsTownHall());
    
    if (bases.size() > 1) {
        return false;
    }
    if (bases.size() < 1) {
        return false;
    }
    return TryExpand(sc2::ABILITY_ID::BUILD_COMMANDCENTER, sc2::UNIT_TYPEID::TERRAN_SCV);
}
