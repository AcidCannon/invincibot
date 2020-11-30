
// The MIT License (MIT)
//
// Copyright (c) 2020 Qian Yu

#pragma once
#include <sc2api/sc2_api.h>

#include "common/Builder.h"


enum ArmyType{
    solider,
    vehicle
};

class MarinePush : public sc2::Agent {
   public:
    void OnGameStart() final;
    void OnStep() final;
    void OnUnitIdle(const sc2::Unit* unit) final;

   private:
    size_t CountUnitType(sc2::UNIT_TYPEID unit_type);
    void TryAttack();
    bool TryBuildStructure(
        sc2::ABILITY_ID ability_type_for_structure,
        sc2::UNIT_TYPEID unit_type = sc2::UNIT_TYPEID::TERRAN_SCV);
    bool TryBuildStructureConcurrent(
        sc2::ABILITY_ID ability_type_for_structure,
        sc2::UNIT_TYPEID unit_type = sc2::UNIT_TYPEID::TERRAN_SCV);



    void CollectVespene();
    std::vector<sc2::Unit> FindUnderMiningRefinery();
    std::vector<sc2::Unit> FindMiningScvs();

    bool IfTrainReaper();

    bool TryBuildEngineeringBay();
    bool TryBuildArmory();
    bool IfUpgradeBarrack();
    void FindEnemyPlace(const sc2::Unit* unit);


    // From HZH

    void TryLowerSupplyDepot();
    void TryUpgradeToOrbitalCommand();
    bool TryExpand(sc2::AbilityID build_ability, sc2::UNIT_TYPEID unit_type);


    /** Building Related Functions **/
    bool TryBuildSupplyDepot();
    bool TryBuildBarracks();
    bool TryBuildRefinery();
    bool TryBuildRefineryNearby();
    bool TryBuildFactory();
    bool TryBuildExpansionCommandCenter();
    void TryBuildBarrackTechLab();

    /** Helper Functions **/
    bool checkAttackCondition(ArmyType type);
    void CountUnitNumber();
    static bool FindEnemyMainStructure(const sc2::ObservationInterface* observation, const sc2::Unit*& enemy_unit);
    const sc2::Unit* FindNearestMineralPatch(const sc2::Point2D& start);
    const sc2::Unit* FindNearestVespeneGeyser(const sc2::Point2D& start);
    sc2::Point2D FindNearestEnemyLocation(const sc2::Point2D& start);


    /** onUnitIdle **/
    #include "MarinePushIdle.h"

    /** path finder **/
    const sc2::Unit* enemyFinder = nullptr;
    std::vector<sc2::Point2D> enemyLocations;

    /** Private Members **/
    size_t num_of_terran_marine = 0;
    size_t num_of_terran_reaper = 0;
    size_t num_of_terran_scv = 0;
    size_t num_of_terran_widowmine = 0;
    bool if_rush = false;
    sc2::Point2D GatheringPoint;
    sc2::Point2D CommandCentrePoint;
    sc2::Point3D startLocation_;
    std::vector<sc2::Point3D> expansions_;
    bool if_vehicle_rush = false;
    bool if_soldier_rush = false;

};
