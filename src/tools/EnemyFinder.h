#pragma once

#include <vector>
#include <sc2api/sc2_api.h>

using namespace sc2;

enum BaseState {
  enemyBase,
  enemySubBase,
  ownBase,
  ownSubBase,
  notDetected,
  empty,
};

struct LocationState {
  BaseState baseState;
  sc2::Point2D location;
};

struct Finder {
  Finder(const Unit *unit_) : unit(unit_) {}
  const Unit *unit;
  std::vector<LocationState> toSearch;
};

class EnemyFinder : public Agent {
 public:
  EnemyFinder();
  virtual ~EnemyFinder() = default;
  void addFinder(const Unit *unit);
  bool runEnemyFinder();

  size_t size();
 private:
  size_t removeDeadFinder();
  size_t removeFoundBase();
  void searching();
  std::vector<Finder> pathFinder;
  std::vector<LocationState> locationState;
};




