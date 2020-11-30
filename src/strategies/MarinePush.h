
// The MIT License (MIT)
//
// Copyright (c) 2020 Qian Yu

#pragma once
#include <sc2api/sc2_api.h>

#include "common/Builder.h"

class MarinePush : public sc2::Agent {
   public:
    virtual void OnGameStart() final;
    virtual void OnStep() final;
    virtual void OnUnitIdle(const sc2::Unit* unit) final;

   private:
    size_t CountUnitType(sc2::UNIT_TYPEID unit_type);
    void TryAttack();
    bool TryBuildStructure(
        sc2::ABILITY_ID ability_type_for_structure,
        sc2::UNIT_TYPEID unit_type = sc2::UNIT_TYPEID::TERRAN_SCV);
    bool TryBuildStructureConcurrent(
        sc2::ABILITY_ID ability_type_for_structure,
        sc2::UNIT_TYPEID unit_type = sc2::UNIT_TYPEID::TERRAN_SCV);
    bool TryBuildSupplyDepot();
    const sc2::Unit* FindNearestMineralPatch(const sc2::Point2D& start);
    bool TryBuildBarracks();

    bool if_rush = false;
    sc2::Point2D GatheringPoint;
    sc2::Point2D CommandCentrePoint;
    bool TryBuildRefinery();
    bool TryBuildRefineryNearby();
    const sc2::Unit* FindNearestVespeneGeyser(const sc2::Point2D& start);
    bool TryBuildFactory();
    void CountUnitNumber();
    size_t num_of_terran_marine = 0;
    size_t num_of_terran_reaper = 0;
    size_t num_of_terran_scv = 0;
    size_t num_of_terran_widowmine = 0;
    void CollectVespene();
    std::vector<sc2::Unit> FindUnderMiningRefinery();
    std::vector<sc2::Unit> FindMiningScvs();
    bool if_soldier_rush = false;
    bool IfTrainReaper();
    bool if_vehicle_rush = false;
    bool TryBuildEngineeringBay();
    bool TryBuildArmory();
    bool IfUpgradeBarrack();

    // From HZH
    sc2::Point3D startLocation_;
    std::vector<sc2::Point3D> expansions_;
    void TryLowerSupplyDepot();
    void TryUpgradeToOrbitalCommand();
    bool TryExpand(sc2::AbilityID build_ability, sc2::UNIT_TYPEID unit_type);
    bool TryBuildExpansionCommandCenter();
    
};
