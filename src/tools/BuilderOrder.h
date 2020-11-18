
#pragma once

#include <sc2api/sc2_typeenums.h>
#include <sc2api/sc2_unit.h>
#include <queue>
#include "common/Builder.h"
#include "interface/Order.h"

class BuilderOrder {
 public:
    BuilderOrder();

    void OnStep();

    // void OnUnitCreated(const sc2::Unit& unit_);

    void ScheduleOrder(sc2::UNIT_TYPEID id_);

    std::queue<Order> GetOrders() const;

    void PrintOrders() const;

    void AddSCV(sc2::Unit* unit);
    // int64_t CountScheduledOrders(sc2::UNIT_TYPEID id_) const;

   private:
    bool Build(Order* order_);

    uint32_t m_minerals;
    uint32_t m_vespene;

    Builder build_;
    unsigned int m_available_food;

    std::queue<Order> sequence;
};