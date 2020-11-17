#include <sc2api/sc2_api.h>
bool Build::CanBePlaced(const AbilityID &ability, const Point2D &target_pos) {
    return QueryInterface::Placement(ability, target_pos);
}

bool Build::TryBuild(Order *order_, sc2::Point2D point) {
    Unit unit = ChoosenUnit();
    if (unit == nullptr) return false;
    if (!CanBePlaced(order_->ability_id, point)) return false;
    Actions()->UnitCommand(unit, order_->ability_id, point, true);
    return true;
}

bool Build::TryBuildWithRandomLocation(Order *order_) {
    Unit unit = ChoosenUnit();
    if (unit == nullptr) return false;
    const sc2::ObservationInterface *observation = Observation();

    // Find place to build the structure
    sc2::Point3D base = observation->StartingLocation();
    sc2::Point2D point;

    unsigned attempt = 0;
    do {
        point.x = base.x + sc2::GetRandomScalar() * 15.0f;
        point.y = base.y + sc2::GetRandomScalar() * 15.0f;

        if (++attempt > 150) return false;
    } while (CanBePlaced(order_->ability_id, point));

    Actions()->UnitCommand(ChoosenUnit, order_->ability_id, point, true);
    return true;
}

void Build::AddSCV(Unit *unit) { scvs.push_back(unit); }

/**
 * Rule:
 * If the scv is not building or have finished the build. choose him
 * If all scvs are working, choose the one who have the greatest progres
 */
Unit Build::ChoosenUnit() {
    float build_progress = 0;
    int index = 0;
    if (scvs.empty()) return nullptr;
    for (int i = 0; i < scvs.size(); i++) {
        Unit scv = scvs[i];
        if (scv.build_progress == 1.0f || scv.build_progress == 0.0f) {
            return scv;
        } else {
            if (progress < scv.build_progress) {
                progress = scv.build_progress;
                index = i;
            }
        }
    }
    return scvs[index];
}