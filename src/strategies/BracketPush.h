#include <sc2api/sc2_api.h>
#include <vector>

using namespace sc2;

enum BaseState {
  enemyBase,
  enemySubBase,
  ownBase,
  ownSubBase,
  notDetected,
  empty,
};

struct LocationState{
  BaseState baseState;
  sc2::Point2D location;
};

struct Finder{
  Finder(const Unit* unit_) : unit(unit_) {}
  const Unit* unit;
  std::vector<LocationState> toSearch;
};

class BracketPushBot : public Agent {
 public:

  virtual void OnGameStart() final;
  virtual void OnStep() final;
  virtual void OnUnitIdle(const Unit* unit) final;

 private:

  size_t CountUnitType(UNIT_TYPEID unit_type);
  void TryAttack();
  bool TryBuildStructure(ABILITY_ID ability_type_for_structure,
                         UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV);
  bool TryBuildSupplyDepot();
  const Unit* FindNearestMineralPatch(const Point2D& start);
  bool TryBuildBarracks();

  /** UNIT IDLE **/
  void onSCVIdle(const Unit* unit);
  void FindEnemyPlace(const Unit* unit);

  bool if_rush = false;
  Point2D GatheringPoint;

  /** path finder **/
  const Unit* enemyFinder = nullptr;
  std::vector<Point2D> enemyLocations;

};
