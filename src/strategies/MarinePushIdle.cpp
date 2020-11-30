#include "MarinePush.h"

/** building **/
void MarinePush::onCommandCenterIdle(const sc2::Unit *unit) {
    GatheringPoint = sc2::Point2D(unit->pos.x, unit->pos.y);
    CommandCentrePoint = sc2::Point2D(unit->pos.x, unit->pos.y);
    // std::cout << "Command centre coord: "<< unit->pos.x << " " << unit->pos.y << std::endl;
    if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_SCV) < 22) {
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_SCV);
    }
}

void MarinePush::onBarracksIdle(const sc2::Unit *unit) {
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
}

void MarinePush::onFactoryIdle(const sc2::Unit* unit) {
    // if (CountUnitType(sc2::UNIT_TYPEID::TERRAN_WIDOWMINE) < 4) {
    //     Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_WIDOWMINE);
    // }
    Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_HELLION);
}

void MarinePush::onEngineeringBayIdle(const sc2::Unit* unit) {
    Actions()->UnitCommand(unit, sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMOR);
}

void MarinePush::onArmoryIdle(const sc2::Unit* unit) {
    Actions()->UnitCommand(unit, sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS);
}

/** non-building units **/
void MarinePush::onSCVIdle(const sc2::Unit *unit) {
    // Task: Find Nearest Mineral Patch
    const sc2::Unit *mineral_target =
        FindNearestMineralPatch(unit->pos);
    if (mineral_target) {
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::SMART,
                               mineral_target);
    }
}

void MarinePush::onMarineIdle(const sc2::Unit *unit) {
    if (checkAttackCondition(ArmyType::solider)) {
        //priori to attack main structure
        const sc2::Unit* enemy_unit = nullptr;
        if (FindEnemyMainStructure(Observation(), enemy_unit)) {
            Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK, enemy_unit);
            return;
        }
        const sc2::GameInfo &game_info = Observation()->GetGameInfo();
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK,
                               enemyLocations[0]);
        if_soldier_rush = false;
    } else {
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::SMART,
                               GatheringPoint);
    }
}

void MarinePush::onMarauderIdle(const sc2::Unit *unit) {
    if (checkAttackCondition(ArmyType::solider)) {
        const sc2::GameInfo &game_info = Observation()->GetGameInfo();
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK,
                               enemyLocations[0]);
        if_soldier_rush = false;
    } else {
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::SMART,
                               GatheringPoint);
    }
}

void MarinePush::onReaperIdle(const sc2::Unit *unit) {
    if (checkAttackCondition(ArmyType::solider)) {
        const sc2::GameInfo &game_info = Observation()->GetGameInfo();
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK,
                               enemyLocations[0]);
    } else {
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::SMART,
                               GatheringPoint);
    }
}

void MarinePush::onWidowmineIdle(const sc2::Unit* unit) {
    Actions()->UnitCommand(unit, sc2::ABILITY_ID::BURROWDOWN_WIDOWMINE);
}

void MarinePush::onHellionIdle(const sc2::Unit* unit) {
    if (checkAttackCondition(ArmyType::vehicle)) {
        if_vehicle_rush = false;
        //priori to attack main structure
        const sc2::Unit* enemy_unit = nullptr;
        if (FindEnemyMainStructure(Observation(), enemy_unit)) {
            Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK, enemy_unit);
            return;
        }
        const sc2::GameInfo &game_info = Observation()->GetGameInfo();
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK_ATTACK,
                               enemyLocations[0]);
    } else {
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::SMART,
                               GatheringPoint);
    }
}

