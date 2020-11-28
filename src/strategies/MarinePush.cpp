// The MIT License (MIT)
//
// Copyright (c) 2020 Qian Yu

#include "MarinePush.h"
#include "tools/BuilderOrder.h"
#include <iostream>
#include <sc2api/sc2_api.h>
#include <vector>


void MarinePush::OnGameStart() {
    // ClientEvents::OnGameStart();
    Actions()->SendChat("Break a leg :)");
}

void MarinePush::OnStep() {
    // ClientEvents::OnStep();
    CountUnitNumber();
    TryBuildSupplyDepot();
    TryBuildBarracks();
    TryBuildRefinery();
    CollectVespene();
    TryBuildFactory();
    TryAttack();
}

void MarinePush::OnUnitIdle(const sc2::Unit *unit) {
    switch (unit->unit_type.ToType()) {
        case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER: {
            GatheringPoint = sc2::Point2D(unit->pos.x, unit->pos.y);
            CommandCentrePoint = sc2::Point2D(unit->pos.x, unit->pos.y);
            // std::cout << "Command centre coord: "<< unit->pos.x << " " << unit->pos.y << std::endl;
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
            if (num_of_terran_marine <= 20) {
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_MARINE);
                break;
            } else if (num_of_terran_reaper <= 20) {
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_REAPER);
                break;
            }
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_MARINE: {
            if (if_rush) {
                const sc2::GameInfo &game_info = Observation()->GetGameInfo();
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK,
                                       game_info.enemy_start_locations.front());
            } else {
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::SMART,
                                       GatheringPoint);
            }
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_REAPER: {
            if (if_rush) {
                const sc2::GameInfo &game_info = Observation()->GetGameInfo();
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK,
                                       game_info.enemy_start_locations.front());
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
    if (num_of_terran_reaper > 20 && num_of_terran_marine > 20) {
        if_rush = true;
    }
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

    if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS) > 5) {
        return false;
    }

    return TryBuildStructure(sc2::ABILITY_ID::BUILD_BARRACKS);
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
}

void MarinePush::CollectVespene() {
    sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);

    std::vector<sc2::Unit> refineries;
    
    for (const auto &unit : units) {
        if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_REFINERY) {
            if (unit->assigned_harvesters < unit->ideal_harvesters + 1) {
                refineries.push_back(*unit);
            }
        }
    }

    for(const auto &refinery: refineries) {
        std::cout << refinery.vespene_contents << std::endl;
        // TODO: Assign more SCVs to collect vespene.
    }
}