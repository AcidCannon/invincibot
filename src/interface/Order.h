#pragma once

#include <sc2api/sc2_data.h>
#include <sc2api/sc2_unit.h>

struct Order {

    explicit Order(const sc2::UnitTypeData& data_,
                   const sc2::Unit* assignee_ = nullptr)
        : name(data_.name),
          mineral_cost(data_.mineral_cost),
          vespene_cost(data_.vespene_cost),
          food_required(data_.food_required),
          unit_type_id(data_.unit_type_id),
          ability_id(data_.ability_id),
          assignee(0) {
        if (assignee_) assignee = assignee_->tag;
    }

    std::string name;

    uint32_t mineral_cost;
    uint32_t vespene_cost;

    unsigned int food_required;

    sc2::UnitTypeID unit_type_id;
    sc2::AbilityID ability_id;
    
    sc2::Tag assignee;
};
