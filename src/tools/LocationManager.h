#pragma once

#include <sc2api/sc2_api.h>
#include <sc2lib/sc2_lib.h>
#include <vector>
#include <cmath>

using Slot = struct {
    sc2::Point2D pos;
    sc2::UNIT_TYPEID unit_type_id;
};

enum loc2 { U, D };
enum loc4 { LU, LD, RU, RD };
enum gamemap { CactusValleyLE, BelShirVestigeLE, ProximaStationLE };


const float EPSILON = 0.001f;
// Locations for CactusValleyLE
const Slot CVLE_COMMANDCENTER_SLOT0 = { sc2::Point2D(33.5f, 158.5f), sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER };
const Slot CVLE_COMMANDCENTER_SLOT1 = { sc2::Point2D(33.5f, 33.5f), sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER };
const Slot CVLE_COMMANDCENTER_SLOT2 = { sc2::Point2D(158.5f, 158.5f), sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER };
const Slot CVLE_COMMANDCENTER_SLOT3 = { sc2::Point2D(158.5f, 33.5f), sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER };
const Slot CVLE_BARRACK_SLOT0 = { sc2::Point2D(50.5f, 162.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot CVLE_BARRACK_SLOT1 = { sc2::Point2D(25.5f, 149.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot CVLE_BARRACK_SLOT2 = { sc2::Point2D(25.5f, 146.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot CVLE_BARRACK_SLOT3 = { sc2::Point2D(25.5f, 143.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot CVLE_BARRACK_SLOT4 = { sc2::Point2D(25.5f, 140.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot CVLE_BARRACK_SLOT5 = { sc2::Point2D(32.5f, 147.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot CVLE_BARRACK_SLOT6 = { sc2::Point2D(32.5f, 144.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot CVLE_BARRACK_SLOT7 = { sc2::Point2D(32.5f, 141.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot CVLE_SUPPLYDEPOT_SLOT0 = { sc2::Point2D(53.0f, 163.5f), sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT };
const Slot CVLE_SUPPLYDEPOT_SLOT1 = { sc2::Point2D(51.0f, 160.0f), sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT };
const sc2::Point2D CVLE_MAPMIDPOINT = sc2::Point2D(96.0f, 96.0f);
// Locations for BelShirVestigeLE
const Slot BSLE_COMMANDCENTER_SLOT0 = { sc2::Point2D(29.5f, 134.5f), sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER };
const Slot BSLE_COMMANDCENTER_SLOT1 = { sc2::Point2D(114.5f, 25.5f), sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER };
const Slot BSLE_BARRACK_SLOT0 = { sc2::Point2D(43.5f, 132.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot BSLE_BARRACK_SLOT1 = { sc2::Point2D(17.5f, 126.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot BSLE_BARRACK_SLOT2 = { sc2::Point2D(22.5f, 126.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot BSLE_BARRACK_SLOT3 = { sc2::Point2D(17.5f, 123.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot BSLE_BARRACK_SLOT4 = { sc2::Point2D(22.5f, 123.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot BSLE_BARRACK_SLOT5 = { sc2::Point2D(28.5f, 124.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot BSLE_BARRACK_SLOT6 = { sc2::Point2D(33.5f, 124.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot BSLE_BARRACK_SLOT7 = { sc2::Point2D(28.5f, 121.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot BSLE_SUPPLYDEPOT_SLOT0 = { sc2::Point2D(46.0f, 133.0f), sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT };
const Slot BSLE_SUPPLYDEPOT_SLOT1 = { sc2::Point2D(43.0f, 130.0f), sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT };
const sc2::Point2D BSLE_MAPMIDPOINT = sc2::Point2D(72.0f, 80.0f);
// Locations for ProximaStationLE
const Slot PSLE_COMMANDCENTER_SLOT0 = { sc2::Point2D(137.5f, 139.5f), sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER };
const Slot PSLE_COMMANDCENTER_SLOT1 = { sc2::Point2D(62.5f, 28.5f), sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER };
const Slot PSLE_BARRACK_SLOT0 = { sc2::Point2D(148.5f, 118.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot PSLE_BARRACK_SLOT1 = { sc2::Point2D(130.5f, 150.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot PSLE_BARRACK_SLOT2 = { sc2::Point2D(127.5f, 147.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot PSLE_BARRACK_SLOT3 = { sc2::Point2D(128.5f, 134.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot PSLE_BARRACK_SLOT4 = { sc2::Point2D(127.5f, 130.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot PSLE_BARRACK_SLOT5 = { sc2::Point2D(127.5f, 126.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot PSLE_BARRACK_SLOT6 = { sc2::Point2D(133.5f, 130.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot PSLE_BARRACK_SLOT7 = { sc2::Point2D(133.5f, 126.5f), sc2::UNIT_TYPEID::TERRAN_BARRACKS };
const Slot PSLE_SUPPLYDEPOT_SLOT0 = { sc2::Point2D(146.0f, 119.0f), sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT };
const Slot PSLE_SUPPLYDEPOT_SLOT1 = { sc2::Point2D(149.0f, 116.0f), sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT };
const sc2::Point2D PSLE_MAPMIDPOINT = sc2::Point2D(100.0f, 84.0f);

class LocationManager {
public:
    std::vector<Slot> GetBarrackSlots2(loc2 loc, gamemap gmap);
    std::vector<Slot> GetBarrackSlots4(loc4 loc);
    std::vector<Slot> GetSupplyDepot2(loc2 loc, gamemap gmap);
    std::vector<Slot> GetSupplyDepot4(loc4 loc);

    sc2::Point2D calLU4(sc2::Point2D input_point) { return input_point; }
    sc2::Point2D calLD4(sc2::Point2D input_point);
    sc2::Point2D calRU4(sc2::Point2D input_point);
    sc2::Point2D calRD4(sc2::Point2D input_point);
    sc2::Point2D calLU2(sc2::Point2D input_point) { return input_point; }
    sc2::Point2D calRD2(sc2::Point2D mid_point, sc2::Point2D input_point);

    loc2 GetLoc2(sc2::Point2D init_base_point, gamemap gmap);
    loc4 GetLoc4(sc2::Point2D init_base_point);

    bool IsSamePoint(sc2::Point2D p1, sc2::Point2D p2);
};
