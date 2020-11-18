#include <sc2api/sc2_api.h>
#include "common/Builder.h"

bool Builder::CanBePlaced(const sc2::AbilityID &ability, const sc2::Point2D &target_pos) {
    sc2::QueryInterface* query_;
    return query_->Placement(ability, target_pos);
}

bool Builder::TryBuild(Order *order_, sc2::Point2D point) {
    const sc2::Unit* unit = IdleSCV();
    sc2::ActionInterface* action_;
    if (unit == nullptr) return false;
    if (CanBePlaced(order_->ability_id, point)) {
        action_->UnitCommand(unit, order_->ability_id, point, true);
        return true;
    } else return false;
}

bool Builder::TryBuildWithRandomLocation(Order *order_) {
    const sc2::ObservationInterface* observer_;
    sc2::ActionInterface* action_;
    // Find place to build the structure
    sc2::Point3D base = observer_->GetStartLocation();
    sc2::Point2D point;

    unsigned attempt = 0;
    do {
        point.x = base.x + sc2::GetRandomScalar() * 15.0f;
        point.y = base.y + sc2::GetRandomScalar() * 15.0f;

        if (++attempt > 150) return false;
    } while (CanBePlaced(order_->ability_id, point));

    sc2::Unit* unit = IdleSCV();
    if (unit == nullptr) return false;
    action_->UnitCommand(unit, order_->ability_id, point, true);
    return true;
}

/**
 * Rule:
 * If the scv is not building or have finished the build. choose him
 * If all scvs are working, choose the one who have the greatest progres
 */
sc2::Unit* Builder::IdleSCV() {
    float build_progress = 0;
    size_t index = 0;
    if (scvs.empty()) return nullptr;
    for (size_t i = 0; i < scvs.size(); i++) {
        sc2::Unit* scv = scvs[i];
        if (scv->build_progress == 1.0f || scv->build_progress == 0.0f) {
            return scv;
        } else {
            if (build_progress < scv->build_progress) {
                build_progress = scv->build_progress;
                index = i;
            }
        }
    }
    return scvs[index];
}