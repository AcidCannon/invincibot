
#pragma once

#include <sc2api/sc2_typeenums.h>
#include <sc2api/sc2_unit.h>
#include <queue>

#include "interface/Order.h"

class BuilderOrder {
 public:
    BuilderOrder();

    void OnStep();

    // void OnUnitCreated(const sc2::Unit& unit_);

    void ScheduleOrder(sc2::UPGRADE_ID id_);

    std::vector<Order> GetOrders() const;

    void PrintOrders() const;

    // int64_t CountScheduledOrders(sc2::UNIT_TYPEID id_) const;

   private:
    bool Build(Order* order_);

    uint32_t m_minerals;
    uint32_t m_vespene;

    float m_available_food;

    std::queue<Order> sequence;
};