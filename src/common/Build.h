#pragma once
#include "interface/Order.h"

class Build {
   public:
    bool CanBePlaced(const AbilityID& ability, const Point2D& target_pos);
    bool TryBuild(Order* order_);
    bool TryBuildWithRandomLocation(Order* order_);
    void AddSCV(Unit* unit);
    Unit* ChoosenUnit();

   private:
    vector<Unit*> scvs;
}