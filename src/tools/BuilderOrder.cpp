#include "BuilderOrder.h"
#include "common/Builder.h"
#include <sc2api/sc2_api.h>

BuilderOrder::BuilderOrder()
    : m_minerals(0), m_vespene(0), m_available_food(0.0f) {}

void BuilderOrder::OnStep() {
    const sc2::ObservationInterface* observer_;
    m_minerals = observer_->GetMinerals();
    m_vespene = observer_->GetVespene();
    m_available_food =
        observer_->GetFoodCap() - observer_->GetFoodUsed();

    if (!sequence.empty()) {
        Order cur = sequence.front();
        if (Build(&cur)) {
            sequence.pop();
        }
    }
}

void BuilderOrder::ScheduleOrder(sc2::UNIT_TYPEID id_) {
    const sc2::ObservationInterface* observer_;
    sc2::UnitTypeData typeData = observer_->GetUnitTypeData()[static_cast<sc2::UnitTypeID>(id_)];
    sequence.push(Order(typeData));
}

void BuilderOrder::AddSCV(sc2::Unit *unit) { build_.scvs.push_back(unit); }

bool BuilderOrder::Build(Order *order) {
    if (m_minerals < order->mineral_cost) return false;

    if (m_vespene < order->vespene_cost) return false;

    if (m_available_food < order->food_required) return false;

    if (build_.TryBuildWithRandomLocation(order)) {
        m_minerals -= order->mineral_cost;
        m_vespene -= order->vespene_cost;
        m_available_food -= order->food_required;
        return true;
    }
    return false;
}

std::queue<Order> BuilderOrder::GetOrders() const { return sequence; }

void BuilderOrder::PrintOrders() const {
    std::queue<Order> orders(sequence);
    while(!orders.empty()) {
        Order order = orders.front();
        orders.pop();
        std::cout << order.name << std::endl;
    }
}

