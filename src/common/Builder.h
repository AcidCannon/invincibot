#pragma once
#include "interface/Order.h"

class Builder {
   public:
    bool CanBePlaced(const sc2::AbilityID& ability, const sc2::Point2D& target_pos);
    bool TryBuild(Order* order_, sc2::Point2D point);
    bool TryBuildWithRandomLocation(Order* order_);

    sc2::Unit* IdleSCV();

    std::vector<sc2::Unit*> scvs;
};
