#include "LocationManager.h"
#include <sc2api/sc2_api.h>

std::vector<Slot> LocationManager::GetBarrackSlots2(loc2 loc, gamemap gmap) {
    std::vector<Slot> barrack_slots;
    if (gmap == BelShirVestigeLE) {
        {
            barrack_slots.resize(8);
            barrack_slots[0] = BSLE_BARRACK_SLOT0;
            barrack_slots[1] = BSLE_BARRACK_SLOT1;
            barrack_slots[2] = BSLE_BARRACK_SLOT2;
            barrack_slots[3] = BSLE_BARRACK_SLOT3;
            barrack_slots[4] = BSLE_BARRACK_SLOT4;
            barrack_slots[5] = BSLE_BARRACK_SLOT5;
            barrack_slots[6] = BSLE_BARRACK_SLOT6;
            barrack_slots[7] = BSLE_BARRACK_SLOT7;
        };
        if (loc == U) {
            {};
        } else {
            for (auto &s : barrack_slots) {
                s.pos = calRD2(BSLE_MAPMIDPOINT, s.pos);
            }
        }
    } else if (gmap == ProximaStationLE) {
        {
            barrack_slots.resize(8);
            barrack_slots[0] = PSLE_BARRACK_SLOT0;
            barrack_slots[1] = PSLE_BARRACK_SLOT1;
            barrack_slots[2] = PSLE_BARRACK_SLOT2;
            barrack_slots[3] = PSLE_BARRACK_SLOT3;
            barrack_slots[4] = PSLE_BARRACK_SLOT4;
            barrack_slots[5] = PSLE_BARRACK_SLOT5;
            barrack_slots[6] = PSLE_BARRACK_SLOT6;
            barrack_slots[7] = PSLE_BARRACK_SLOT7;
        };
        if (loc == U) {
            {};
        } else {
            for (auto &s : barrack_slots) {
                s.pos = calRD2(PSLE_MAPMIDPOINT, s.pos);
            }
        }
    }
    return barrack_slots;
}

std::vector<Slot> LocationManager::GetBarrackSlots4(loc4 loc) {
    std::vector<Slot> barrack_slots;
    barrack_slots.resize(8);
    {
        barrack_slots[0] = CVLE_BARRACK_SLOT0;
        barrack_slots[1] = CVLE_BARRACK_SLOT1;
        barrack_slots[2] = CVLE_BARRACK_SLOT2;
        barrack_slots[3] = CVLE_BARRACK_SLOT3;
        barrack_slots[4] = CVLE_BARRACK_SLOT4;
        barrack_slots[5] = CVLE_BARRACK_SLOT5;
        barrack_slots[6] = CVLE_BARRACK_SLOT6;
        barrack_slots[7] = CVLE_BARRACK_SLOT7;
    };
    switch (loc) {
        case LU: {
            std::cout << "GBS4: LU" << std::endl;
            break;        
        }
        case LD: {
            std::cout << "GBS4: LD" << std::endl;
            for (auto &s : barrack_slots) {
                s.pos = calLD4(s.pos);
            }
            break;
        }
        case RU: {
            std::cout << "GBS4: RU" << std::endl;
            for (auto &s : barrack_slots) {
                s.pos = calRU4(s.pos);
            }
            break;
        }
        case RD: {
            std::cout << "GBS4: RD" << std::endl;
            for (auto &s : barrack_slots) {
                s.pos = calRD4(s.pos);
            }
            break;
        }
        default:
            break;
    }
    return barrack_slots;
}

sc2::Point2D LocationManager::calLD4(sc2::Point2D input_point) {
    float x = input_point.x;
    float y = input_point.y;
    float mid_x = CVLE_MAPMIDPOINT.x;
    return sc2::Point2D(2*mid_x - y, x);
}

sc2::Point2D LocationManager::calRU4(sc2::Point2D input_point) {
    float x = input_point.x;
    float y = input_point.y;
    float mid_x = CVLE_MAPMIDPOINT.x;
    return sc2::Point2D(y, 2*mid_x - x);
}

sc2::Point2D LocationManager::calRD4(sc2::Point2D input_point) {
    float x = input_point.x;
    float y = input_point.y;
    float mid_x = CVLE_MAPMIDPOINT.x;
    float mid_y = CVLE_MAPMIDPOINT.y;
    return sc2::Point2D(2*mid_x - x, 2*mid_y - y);
}

sc2::Point2D LocationManager::calRD2(sc2::Point2D mid_point, sc2::Point2D input_point) {
    float x = input_point.x;
    float y = input_point.y;
    float mid_x = mid_point.x;
    float mid_y = mid_point.y;
    return sc2::Point2D(2*mid_x - x, 2*mid_y - y);
}

bool LocationManager::IsSamePoint(sc2::Point2D p1, sc2::Point2D p2) {
    float abs_delta_x = fabs(p1.x - p2.x);
    float abs_delta_y = fabs(p1.y - p2.y);
    return abs_delta_x < EPSILON && abs_delta_y < EPSILON;
}

loc2 LocationManager::GetLoc2(sc2::Point2D init_base_point, gamemap gmap) {
    loc2 loc = U;
    if (gmap == BelShirVestigeLE) {
        if (IsSamePoint(init_base_point, BSLE_COMMANDCENTER_SLOT0.pos)) {
            {};
        } else {
            loc = D;
        }
    } else if (gmap == ProximaStationLE) {
        if (IsSamePoint(init_base_point, BSLE_COMMANDCENTER_SLOT0.pos)) {
            {};
        } else {
            loc = D;
        }
    }
    return loc;
}

loc4 LocationManager::GetLoc4(sc2::Point2D init_base_point) {
    loc4 loc = LU;
    if (IsSamePoint(init_base_point, CVLE_COMMANDCENTER_SLOT0.pos)) {
        {};
        std::cout << "GetLoc4: LU" << std::endl;
    } else if (IsSamePoint(init_base_point, CVLE_COMMANDCENTER_SLOT1.pos)) {
        loc = LD;
        std::cout << "GetLoc4: LD" << std::endl;
    } else if (IsSamePoint(init_base_point, CVLE_COMMANDCENTER_SLOT2.pos)) {
        loc = RU;
        std::cout << "GetLoc4: RU" << std::endl;
    } else if (IsSamePoint(init_base_point, CVLE_COMMANDCENTER_SLOT3.pos)) {
        loc = RD;
        std::cout << "GetLoc4: RD" << std::endl;
    }
    return loc;
}

std::vector<Slot> LocationManager::GetSupplyDepot2(loc2 loc, gamemap gmap) {
    std::vector<Slot> supply_depot_slots;
    if (gmap == BelShirVestigeLE) {
        {
            supply_depot_slots.resize(2);
            supply_depot_slots[0] = BSLE_SUPPLYDEPOT_SLOT0;
            supply_depot_slots[1] = BSLE_SUPPLYDEPOT_SLOT1;
        };
        if (loc == U) {
            {};
        } else {
            for (auto &s : supply_depot_slots) {
                s.pos = calRD2(BSLE_MAPMIDPOINT, s.pos);
            }
        }
    } else if (gmap == ProximaStationLE) {
        {
            supply_depot_slots.resize(2);
            supply_depot_slots[0] = PSLE_SUPPLYDEPOT_SLOT0;
            supply_depot_slots[1] = PSLE_SUPPLYDEPOT_SLOT1;
        };
        if (loc == U) {
            {};
        } else {
            for (auto &s : supply_depot_slots) {
                s.pos = calRD2(PSLE_MAPMIDPOINT, s.pos);
            }
        }
    }
    return supply_depot_slots;
}

std::vector<Slot> LocationManager::GetSupplyDepot4(loc4 loc) {
    std::vector<Slot> supply_depot_slots;
    {
        supply_depot_slots.resize(2);
        supply_depot_slots[0] = CVLE_SUPPLYDEPOT_SLOT0;
        supply_depot_slots[1] = CVLE_SUPPLYDEPOT_SLOT1;
    };
    switch (loc) {
        case LU: {
            break;        
        }
        case LD: {
            for (auto &s : supply_depot_slots) {
                s.pos = calLD4(s.pos);
            }
            break;
        }
        case RU: {
            for (auto &s : supply_depot_slots) {
                s.pos = calRU4(s.pos);
            }
            break;
        }
        case RD: {
            for (auto &s : supply_depot_slots) {
                s.pos = calRD4(s.pos);
            }
            break;
        }
        default:
            break;
    }
    return supply_depot_slots;
}
