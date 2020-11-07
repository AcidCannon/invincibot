// The MIT License (MIT)
//
// Copyright (c) 2017-2020 Qian Yu

#include "simple_bot.h"

#include <iostream>
#include <sc2api/sc2_api.h>


void Bot::OnGameStart() {
    // ClientEvents::OnGameStart();
    std::cout << "Hello, World!" << std::endl;
}

void Bot::OnStep() {
    // ClientEvents::OnStep();
    TryBuildSupplyDepot();
    TryBuildBarracks();
    TryAttack();
}

void Bot::OnUnitIdle(const sc2::Unit *unit) {
    switch (unit->unit_type.ToType()) {
        case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER: {
            GatheringPoint = sc2::Point2D(unit->pos.x, unit->pos.y);
            Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_SCV);
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
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_MARINE: {
            if (if_rush) {
                const sc2::GameInfo &game_info = Observation()->GetGameInfo();
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK,
                                       game_info.enemy_start_locations[0]);
            } else {
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::SMART,
                                       GatheringPoint);
            }
            break;
        }
        default: {
            break;
        }
    }
}

size_t Bot::CountUnitType(sc2::UNIT_TYPEID unit_type) {
    size_t count = 0;

    sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
    for (auto &unit : units) {
        if (unit->unit_type == unit_type) {
            count++;
        }
    }
    return count;
}

void Bot::TryAttack() {
    if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_MARINE) > 20) {
        if_rush = true;
    }
}

bool Bot::TryBuildStructure(sc2::ABILITY_ID ability_type_for_structure,
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

bool Bot::TryBuildSupplyDepot() {
    const sc2::ObservationInterface *observation = Observation();

    // If we are not supply capped, don't build a supply depot.
    if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2)
        return false;

    // Try and build a depot. Find a random SCV and give it the order.
    return TryBuildStructure(sc2::ABILITY_ID::BUILD_SUPPLYDEPOT);
}

const sc2::Unit *Bot::FindNearestMineralPatch(const sc2::Point2D &start) {
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

bool Bot::TryBuildBarracks() {
    if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
        return false;
    }

    if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS) > 2) {
        return false;
    }

    return TryBuildStructure(sc2::ABILITY_ID::BUILD_BARRACKS);
}
