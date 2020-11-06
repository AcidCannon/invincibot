#include <sc2api/sc2_api.h>

using namespace sc2;

class Bot : public Agent {
   public:
    virtual void OnGameStart() final;
    virtual void OnStep() final;
    virtual void OnUnitIdle(const Unit* unit) final;

   private:
    size_t CountUnitType(UNIT_TYPEID unit_type);
    void TryAttack();
    bool TryBuildStructure(ABILITY_ID ability_type_for_structure,
                           UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV);
    bool TryBuildSupplyDepot();
    const Unit* FindNearestMineralPatch(const Point2D& start);
    bool TryBuildBarracks();


    bool if_rush = false;
    Point2D GatheringPoint;
};
