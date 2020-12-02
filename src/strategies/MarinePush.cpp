// The MIT License (MIT)
//
// Copyright (c) 2020 Qian Yu, Zijian Xi, Zihao Huang

#include "MarinePush.h"

#include <sc2api/sc2_api.h>
#include <sc2api/sc2_unit_filters.h>
// #include <vector>
// #include "tools/LocationManager.h"

// void MarinePush::OnGameStart() {
//     // ClientEvents::OnGameStart();
//     Actions()->SendChat("Break a leg :)");

//     // get start location and expansions
//     startLocation_ = Observation()->GetStartLocation();
//     expansions_ = sc2::search::CalculateExpansionLocations(Observation(), Query());
// }

// void MarinePush::OnStep() {
//     // ClientEvents::OnStep();
//     CountUnitNumber();
//     TryBuildSupplyDepot();
//     TryBuildBarracks();
//     // TryBuildRefinery();
//     // CollectVespene();
//     ///TryBuildBarrackTechLab();
//     // TryBuildFactory();
//     // TryBuildEngineeringBay();
//     // TryBuildArmory();
//     TryAttack();
//     TryLowerSupplyDepot();
//     // TryUpgradeToOrbitalCommand();
//     // TryBuildExpansionCommandCenter();
//     if (enemyLocations.empty() && enemyFinder != nullptr && !enemyFinder->is_alive) {
//         enemyLocations.push_back(FindNearestEnemyLocation(sc2::Point2D(enemyFinder->pos.x, enemyFinder->pos.y)));
//         std::cout << enemyFinder->pos.x << " " << enemyFinder->pos.y << std::endl;
//     }
// }
#include <sc2lib/sc2_lib.h>

#include <iostream>
#include <vector>
#include "tools/LocationManager.h"
//#include <cmath>

float max3(float a, float b, float c){
    return std::max(std::max(a,b), std::max(a,c));
}

void MarinePush::FindEnemyPlace(const sc2::Unit *unit) {
    const sc2::GameInfo &game_info = Observation()->GetGameInfo();
    if(game_info.enemy_start_locations.size() != 3) return;

    float d0 = sc2::DistanceSquared2D(game_info.enemy_start_locations[0], unit->pos);
    float d1 = sc2::DistanceSquared2D(game_info.enemy_start_locations[1], unit->pos);
    float d2 = sc2::DistanceSquared2D(game_info.enemy_start_locations[2], unit->pos);

    if(d0 == max3(d1,d2,d0)){
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations[1],
                               true);
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations[0],
                               true);
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations[2],
                               true);
    } else if(d1 == max3(d1,d2,d0)){
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations[0],
                               true);
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations[1],
                               true);
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations[2],
                               true);
    } else if(d2 == max3(d1,d2,d0)){
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations[0],
                               true);
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations[2],
                               true);
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations[1],
                               true);
    }
}

bool MarinePush::FindEnemyMainStructure(
    const sc2::ObservationInterface *observation,
    const sc2::Unit *&enemy_unit) {
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

bool MarinePush::IfUpgradeBarrack() const {
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

bool MarinePush::TryBuildStructureConcurrent(
    sc2::ABILITY_ID ability_type_for_structure, sc2::UNIT_TYPEID unit_type) {
    const sc2::ObservationInterface *observation = Observation();

    // Get an scv to build the structure.
    const sc2::Unit *unit_to_build = nullptr;
    sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self);
    for (const auto &unit : units) {
        bool skip = false;
        if (enemyFinder != nullptr && unit->tag == enemyFinder->tag) {
            continue;
        }
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

bool MarinePush::TryBuildStructureAt(sc2::ABILITY_ID ability_type_for_structure,
                                     sc2::Point2D where_to_build,
                                     sc2::UNIT_TYPEID unit_type) {
    const sc2::ObservationInterface *observation = Observation();

    const sc2::Unit *unit_to_build = nullptr;
    sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self);
    for (const auto &unit : units) {
        for (const auto &order : unit->orders) {
            if (order.ability_id == ability_type_for_structure) {
                break;
            }
        }

        if (unit->unit_type == unit_type) {
            unit_to_build = unit;
        }
    }

    Actions()->UnitCommand(unit_to_build, ability_type_for_structure, where_to_build);
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
        if (enemyFinder != nullptr && unit->tag == enemyFinder->tag) {
            continue;
        }
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

const sc2::Unit *MarinePush::FindNearestMineralPatch(
    const sc2::Point2D &start) {
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

bool IsVespeneGeyser(const sc2::Unit *unit) {
    // Maybe exist several vespene geysers can be used.
    auto type_of_unit = unit->unit_type.ToType();
    if (type_of_unit == sc2::UNIT_TYPEID::NEUTRAL_VESPENEGEYSER ||
        type_of_unit == sc2::UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER ||
        type_of_unit == sc2::UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER) {
        return true;
    }
    return false;
}

const sc2::Unit *MarinePush::FindNearestVespeneGeyser(
    const sc2::Point2D &start) {
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
    const sc2::Unit *vespene_target =
        FindNearestVespeneGeyser(CommandCentrePoint);
    if (!vespene_target) {
        return false;
    }
    // std::cout << "Vespene Geyser coord: " << vespene_target->pos.x << " "
    // <<vespene_target->pos.y << " " << vespene_target->vespene_contents <<
    // std::endl;
    Actions()->UnitCommand(unit_to_build, sc2::ABILITY_ID::BUILD_REFINERY,
                           vespene_target);

    return true;
}

bool MarinePush::TryBuildBarracks() {
    if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
        return false;
    }

    if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS) > 7) {
        return false;
    }

    if (barrack_locations.empty()) {
        return false;
    }

    for (const auto &loc : barrack_locations) {
        // if can be placed
        // std::cout << loc.pos.x << " " << loc.pos.y << std::endl;
        if (Query()->Placement(sc2::ABILITY_ID::BUILD_BARRACKS, loc.pos)) {
            // std::cout << "Can be placed" << std::endl;
            return TryBuildStructureAt(sc2::ABILITY_ID::BUILD_BARRACKS, loc.pos);
        }
    }

    return false;
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

bool IsCarryingMineral(const sc2::Unit *unit) {
    // Maybe exist several minerals being carried.
    for (const auto &buff : unit->buffs) {
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
            unit->build_progress >= 1.0f && unit->vespene_contents > 0 &&
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

    for (const auto &refinery : refineries) {
        // std::cout << refinery.vespene_contents << std::endl;
        // Assign more SCVs to collect vespene.
        while (refinery.assigned_harvesters < refinery.ideal_harvesters &&
               !mining_scvs.empty()) {
            Actions()->UnitCommand(&mining_scvs.back(),
                                   sc2::ABILITY_ID::HARVEST_GATHER, &refinery);
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

    if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY) > 0) {
        return false;
    }

    return TryBuildStructure(sc2::ABILITY_ID::BUILD_ARMORY);
}

// From HZH
void MarinePush::TryLowerSupplyDepot() {
    const sc2::ObservationInterface *observation = Observation();

    if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT) > 0) {
        sc2::Units supply_depots = observation->GetUnits(
            sc2::Unit::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT));

        for (const auto &supply_depot : supply_depots) {
            Actions()->UnitCommand(supply_depot,
                                   sc2::ABILITY_ID::MORPH_SUPPLYDEPOT_LOWER);
        }
    }
}

void MarinePush::TryUpgradeToOrbitalCommand() {
    const sc2::ObservationInterface *observation = Observation();

    if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS) > 1) {
        sc2::Units bases =
            observation->GetUnits(sc2::Unit::Self, sc2::IsTownHall());

        for (const auto &base : bases) {
            if (base->unit_type == sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER &&
                observation->GetMinerals() > 150) {
                Actions()->UnitCommand(base,
                                       sc2::ABILITY_ID::MORPH_ORBITALCOMMAND);
            }
        }
    }
}

/** Building related functions **/
bool MarinePush::TryBuildExpansionCommandCenter() {
    const sc2::ObservationInterface *observation = Observation();
    sc2::Units bases =
        observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsTownHall());

    if (bases.size() > 1) {
        return false;
    }
    if (bases.empty()) {
        return false;
    }
    return TryExpand(sc2::ABILITY_ID::BUILD_COMMANDCENTER,
                     sc2::UNIT_TYPEID::TERRAN_SCV);
}

void MarinePush::TryBuildBarrackTechLab() {
    const sc2::ObservationInterface *observation = Observation();
    // sc2::Units barracks_tech = observation->GetUnits(Unit::Self,
    // IsUnit(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB));

    sc2::Units barracks = observation->GetUnits(
        sc2::Unit::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_BARRACKS));
    for (const auto &barrack : barracks) {
        if (IfUpgradeBarrack() &&
            CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS) > 1 &&
            CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB) < 3) {
            float rx = sc2::GetRandomScalar();
            float ry = sc2::GetRandomScalar();
            Actions()->UnitCommand(barrack,
                                   sc2::ABILITY_ID::BUILD_TECHLAB_BARRACKS,
                                   sc2::Point2D(barrack->pos.x + rx * 15.0f,
                                                barrack->pos.y + ry * 15.0f));
        }
    }
}
/** Helper Functions **/
/** Helper Functions **/
/** Helper Functions **/
sc2::Point2D MarinePush::FindNearestEnemyLocation(const sc2::Point2D &start) {
    const sc2::GameInfo &game_info = Observation()->GetGameInfo();

    float distance = std::numeric_limits<float>::max();
    sc2::Point2D target = game_info.enemy_start_locations[0];
    for (const auto u : game_info.enemy_start_locations) {
        float d = DistanceSquared2D(u, start);
        if (d < distance) {
            distance = d;
            target = u;
        }
    }
    return target;
}

bool MarinePush::checkAttackCondition(ArmyType type) {
    if (enemyLocations.empty()) return false;

    if (type == solider && if_soldier_rush) return true;
    if (type == vehicle && if_vehicle_rush) return true;

    return false;
}

bool MarinePush::TryExpand(sc2::AbilityID build_ability,
                           sc2::UNIT_TYPEID unit_type) {
    const sc2::ObservationInterface *observation = Observation();
    float minimum_distance = std::numeric_limits<float>::max();
    sc2::Point3D closest_expansion;
    for (const auto &expansion : expansions_) {
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

void MarinePush::AssignAttackCommands(const sc2::Unit *unit) {
    const sc2::ObservationInterface *observation = Observation();
    sc2::Units enemy_units = observation->GetUnits(sc2::Unit::Alliance::Enemy);

    const sc2::GameInfo &game_info = Observation()->GetGameInfo();

    if (!if_arrive && game_info.enemy_start_locations.size() == 1) {
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK,
                               game_info.enemy_start_locations.front());
        if_arrive = true;
        return;
    }

    int count = 0;
    for (const auto enemy_unit : enemy_units) {
        auto attributes =
            observation->GetUnitTypeData().at(enemy_unit->unit_type);

        // ignore non-building units
        if (!enemy_unit->is_alive || attributes.movement_speed != 0) continue;

        const sc2::Point2D next_detection_location(enemy_unit->pos.x,
                                                   enemy_unit->pos.y);
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK,
                               next_detection_location, true);
        count++;
    }

    if (count != 0) return;

    // no building units
    if (!enemyLocations.empty()) {
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK,
                               enemyLocations.front());
    } else {
        for (auto i : game_info.enemy_start_locations) {
            Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK, i,
                                   true);
        }
    }
}

/** Click Events **/
/** Click Events **/
/** Click Events **/
void MarinePush::OnGameStart() {
    // ClientEvents::OnGameStart();
    Actions()->SendChat("Break a leg :)");

    // get start location and expansions
    startLocation_ = Observation()->GetStartLocation();
    expansions_ =
        sc2::search::CalculateExpansionLocations(Observation(), Query());

    const sc2::GameInfo &game_info = Observation()->GetGameInfo();

    if (enemyFinder == nullptr) {
        const sc2::ObservationInterface *observation = Observation();
        sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self);
        for (auto &unit : units) {
            if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_SCV) {
                enemyFinder = unit;
            }
        }
    }
    FindEnemyPlace(enemyFinder);
    std::cout << game_info.start_locations.front().x << std::endl;
}

void MarinePush::OnStep() {
    // ClientEvents::OnStep();
    CountUnitNumber();
    TryBuildSupplyDepot();
    TryBuildBarracks();
    // TryBuildRefinery();
    // CollectVespene();
    // TryBuildBarrackTechLab();
    // TryBuildFactory();
    // TryBuildEngineeringBay();
    // TryBuildArmory();
    TryAttack();
    TryLowerSupplyDepot();
    // TryUpgradeToOrbitalCommand();
    // TryBuildExpansionCommandCenter();

    //    const sc2::ObservationInterface *observation = Observation();
    //    sc2::Units enemy_units =
    //    observation->GetUnits(sc2::Unit::Alliance::Enemy); int count = 0; for
    //    (const auto enemy_unit : enemy_units){
    //        auto attributes =
    //        observation->GetUnitTypeData().at(enemy_unit->unit_type);
    //        if(attributes.movement_speed == 0) count++;
    //    }
    //    if(count == 0){
    //        FindEnemyPlace
    //    }
}

void MarinePush::OnUnitIdle(const sc2::Unit *unit) {
    switch (unit->unit_type.ToType()) {
        case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER: {
            onCommandCenterIdle(unit);
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_SCV: {
            onSCVIdle(unit);
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_BARRACKS: {
            onBarracksIdle(unit);
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_MARINE: {
            onMarineIdle(unit);
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_MARAUDER: {
            onMarauderIdle(unit);
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_REAPER: {
            onReaperIdle(unit);
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_FACTORY: {
            onFactoryIdle(unit);
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_WIDOWMINE: {
            onWidowmineIdle(unit);
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_HELLION: {
            onHellionIdle(unit);
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY: {
            onEngineeringBayIdle(unit);
            // Actions()->UnitCommand(unit,
            // sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS);
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_ARMORY: {
            onArmoryIdle(unit);
            break;
        }
        default: {
            break;
        }
    }
}

void MarinePush::OnUnitCreated(const sc2::Unit *unit) {
    //        ClientEvents::OnUnitCreated(unit);
}

void MarinePush::OnUnitDestroyed(const sc2::Unit *unit) {
    // ClientEvents::OnUnitDestroyed(unit);
    if (enemyFinder != nullptr && enemyLocations.empty()) {
        if ( unit->tag == enemyFinder->tag)
            enemyLocations.push_back(FindNearestEnemyLocation(
                sc2::Point2D(enemyFinder->pos.x, enemyFinder->pos.y)));
    }
}

void MarinePush::OnUnitEnterVision(const sc2::Unit *unit) {
    ClientEvents::OnUnitEnterVision(unit);

    auto attributes = Observation()->GetUnitTypeData().at(unit->unit_type);

    if (enemyLocations.empty() && enemyFinder != nullptr) {
        if (attributes.movement_speed == 0) {
            sc2::Point2D p = FindNearestEnemyLocation(
                sc2::Point2D(enemyFinder->pos.x, enemyFinder->pos.y));
            enemyLocations.push_back(p);
            Actions()->UnitCommand(enemyFinder, sc2::ABILITY_ID::ATTACK_ATTACK, CommandCentrePoint);
        }
    }
}

void MarinePush::OnBuildingConstructionComplete(const sc2::Unit *unit) {
    ClientEvents::OnBuildingConstructionComplete(unit);
}

void MarinePush::OnUpgradeCompleted(sc2::UpgradeID) {
    //    ClientEvents::OnUpgradeCompleted( < unnamed >);
}

void MarinePush::OnGameEnd() {
    std::cout << "Game Over" << std::endl;
    const sc2::ObservationInterface *observation = Observation();

    sc2::Units own_units = observation->GetUnits(sc2::Unit::Alliance::Self);
    size_t own_units_counter = 0;
    for (const auto unit : own_units) {
        auto attributes = observation->GetUnitTypeData().at(unit->unit_type);
        // std::cout << attributes.name << " " << attributes.movement_speed <<
        // std::endl;
        if (attributes.movement_speed == 0) own_units_counter++;
    }
    if (own_units_counter != 0)
        std::cout << "We Win" << std::endl;
    else
        std::cout << "Enemy Wins" << std::endl;
    ClientEvents::OnGameEnd();
}
