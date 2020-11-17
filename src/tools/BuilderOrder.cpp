#include "BuilderOrder.h"

#include <sc2api/sc2_api.h>

BuilderOrder::BuilderOrder()
    : m_minerals(0), m_vespene(0), m_available_food(0.0f) {}

void BuilderOrder::OnStep() {
    m_minerals = Observation()->GetMinerals();
    m_vespene = Observation()->GetVespene();
    m_available_food =
        Observation()->GetFoodCap() - Observation()->GetFoodUsed();

    if (!sequence.empty()) {
        Order cur = sequence.front();
        if (Build(&cur)) {
            sequence.pop();
        }
    }
}

void BuildOrder::ScheduleOrder(sc2::UPGRADE_ID id_) {
    sc2::UnitTypeData typeData = Observation().GetUnitTypeData(id_);
    sequence.push(Order(typeData));
}

bool BuilderOrder::Build(Order *order) {
    if (m_minerals < order->mineral_cost) return false;

    if (m_vespene < order->vespene_cost) return false;

    if (m_available_food < order->food_required) return false;

    if (TryBuildWithRandomLocation(order)) {
        m_minerals -= order->mineral_cost;
        m_vespene -= order->vespene_cost;
        m_available_food -= order->food_required;
        return true;
    }
    return false;
}

std::vector<Order> GetOrders() const { return sequence; }

void PrintOrders() const {
    for (auto order : sequence) {
        cout << order.name << endl;
    }
}