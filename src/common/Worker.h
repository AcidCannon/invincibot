// The MIT License (MIT)
//
// Copyright (c) 2017-2020 Alexander Kurbatov

#pragma once

#include <sc2api/sc2_gametypes.h>
#include <sc2api/sc2_unit.h>

#include <vector>

struct Hub {
    Hub(sc2::Race current_race_, const Expansions& expansions_);

    void OnStep();

    void OnUnitCreated(const sc2::Unit& unit_);

    void OnUnitDestroyed(const sc2::Unit& unit_);

    void OnUnitIdle(const sc2::Unit& unit_);

    bool IsOccupied(const sc2::Unit& unit_) const;

    bool IsTargetOccupied(const sc2::UnitOrder& order_) const;

    void ClaimObject(const sc2::Unit& unit_);

    sc2::Race GetCurrentRace() const;

    Worker* GetClosestFreeWorker(const sc2::Point2D& location_);

    sc2::UNIT_TYPEID GetCurrentWorkerType() const;

    bool AssignRefineryConstruction(Order* order_, const sc2::Unit* geyser_);

    bool AssignBuildTask(Order* order_, const sc2::Point2D& point_);

    void AssignVespeneHarvester(const sc2::Unit& refinery_);

    bool AssignLarva(Order* order_);

    const Cache<GameObject>&  GetLarvas() const;

    const Expansions& GetExpansions() const;

    const sc2::Point3D* GetNextExpansion();

 private:
    vector<Worker> m_busy_workers;
    vector<Worker> m_free_workers;
};

extern std::unique_ptr<Hub> gHub;
