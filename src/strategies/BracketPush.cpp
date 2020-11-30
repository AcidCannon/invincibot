#include "BracketPush.h"
#include <iostream>
#include <sc2api/sc2_api.h>

using namespace sc2;

void BracketPushBot::OnGameStart() {
    // ClientEvents::OnGameStart();
    std::cout << "Hello, World!" << std::endl;

}

void BracketPushBot::OnStep() {
    // ClientEvents::OnStep();
    TryBuildSupplyDepot();
    TryBuildBarracks();
    TryAttack();
    if (enemyLocations.size() == 0 && enemyFinder != nullptr && !enemyFinder->is_alive) {
        enemyLocations.push_back(Point2D(enemyFinder->pos.x, enemyFinder->pos.y));
        std::cout << enemyFinder->pos.x << " " <<  enemyFinder->pos.y << std::endl;
    }
}

void BracketPushBot::OnUnitIdle(const Unit *unit) {
    switch (unit->unit_type.ToType()) {
        case UNIT_TYPEID::TERRAN_COMMANDCENTER: {
            GatheringPoint = Point2D(unit->pos.x, unit->pos.y);
            Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
            break;
        }
        case UNIT_TYPEID::TERRAN_SCV: {
            onSCVIdle(unit);
            break;
        }
        case UNIT_TYPEID::TERRAN_BARRACKS: {
            Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
            break;
        }
        case UNIT_TYPEID::TERRAN_MARINE: {
            if (if_rush && enemyLocations.size() > 0) {
                const GameInfo &game_info = Observation()->GetGameInfo();
                Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK,
                                       enemyLocations[0]);
            } else {
                Actions()->UnitCommand(unit, ABILITY_ID::SMART,
                                       GatheringPoint);
            }
            break;
        }
        default: {
            break;
        }
    }
}

void BracketPushBot::onSCVIdle(const Unit *unit) {
    // Task1: Find enemy place
    if( CountUnitType(UNIT_TYPEID::TERRAN_SCV) > 16 && enemyFinder == nullptr){
        enemyFinder = unit;
        FindEnemyPlace(unit);
        return;
    }

    // Task2: Find Nearest Mineral Patch
    const Unit *mineral_target =
        FindNearestMineralPatch(unit->pos);
    if (mineral_target) {
        Actions()->UnitCommand(unit, ABILITY_ID::SMART,
                               mineral_target);
    }
}

void BracketPushBot::FindEnemyPlace(const Unit* unit) {
    const GameInfo &game_info = Observation()->GetGameInfo();
    std::cout << "find" << std::endl;
    for(auto dest : game_info.enemy_start_locations){
        Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK,
                               dest, true);
    }
}

size_t BracketPushBot::CountUnitType(UNIT_TYPEID unit_type) {
    size_t count = 0;

    Units units = Observation()->GetUnits(Unit::Alliance::Self);
    for (auto &unit : units) {
        if (unit->unit_type == unit_type) {
            count++;
        }
    }
    return count;
}

void BracketPushBot::TryAttack() {
    if (CountUnitType(UNIT_TYPEID::TERRAN_MARINE) > 20) {
        if_rush = true;
    }
}

bool BracketPushBot::TryBuildStructure(ABILITY_ID ability_type_for_structure,
                            UNIT_TYPEID unit_type) {
    const ObservationInterface *observation = Observation();

    // If a unit already is building a supply structure of this type, do
    // nothing. Also get an scv to build the structure.
    const Unit *unit_to_build = nullptr;
    Units units = observation->GetUnits(Unit::Alliance::Self);
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

    float rx = GetRandomScalar();
    float ry = GetRandomScalar();

    Actions()->UnitCommand(unit_to_build, ability_type_for_structure,
                           Point2D(unit_to_build->pos.x + rx * 15.0f,
                                   unit_to_build->pos.y + ry * 15.0f));

    return true;
}

bool BracketPushBot::TryBuildSupplyDepot() {
    const ObservationInterface *observation = Observation();

    // If we are not supply capped, don't build a supply depot.
    if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2)
        return false;

    // Try and build a depot. Find a random SCV and give it the order.
    return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT);
}

const Unit *BracketPushBot::FindNearestMineralPatch(const Point2D &start) {
    Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
    float distance = std::numeric_limits<float>::max();
    const Unit *target = nullptr;
    for (const auto &u : units) {
        if (u->unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD) {
            float d = DistanceSquared2D(u->pos, start);
            if (d < distance) {
                distance = d;
                target = u;
            }
        }
    }
    return target;
}


bool BracketPushBot::TryBuildBarracks() {
    if (CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
        return false;
    }

    if (CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 2) {
        return false;
    }

    return TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
}
