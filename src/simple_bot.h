// The MIT License (MIT)
//
// Copyright (c) 2017-2020 Qian Yu

#include <sc2api/sc2_api.h>

class Bot : public sc2::Agent {
   public:
    virtual void OnGameStart() final;
    virtual void OnStep() final;
    virtual void OnUnitIdle(const sc2::Unit* unit) final;

   private:
    size_t CountUnitType(sc2::UNIT_TYPEID unit_type);
    void TryAttack();
    bool TryBuildStructure(sc2::ABILITY_ID ability_type_for_structure,
                           sc2::UNIT_TYPEID unit_type = sc2::UNIT_TYPEID::TERRAN_SCV);
    bool TryBuildSupplyDepot();
    const sc2::Unit* FindNearestMineralPatch(const sc2::Point2D& start);
    bool TryBuildBarracks();


    bool if_rush = false;
    sc2::Point2D GatheringPoint;
};
