#include "EnemyFinder.h"
#include <sc2api/sc2_api.h>
using namespace sc2;

size_t EnemyFinder::removeDeadFinder() {
    std::vector<Finder>::iterator it = pathFinder.begin();
    size_t size = pathFinder.size();
    while(it != pathFinder.end()){
        if(((*it).unit)->is_alive){
            it++;
        } else {
            it = pathFinder.erase(it);
            size --;
        }
    }
    return size;
}

EnemyFinder::EnemyFinder() {
    const GameInfo &game_info = Observation()->GetGameInfo();
    size_t num_of_enemy_location = game_info.enemy_start_locations.size();
    locationState.resize(num_of_enemy_location);

    for (size_t kI = 0; kI < num_of_enemy_location; ++kI) {
        locationState[kI].location = game_info.enemy_start_locations[kI];
        locationState[kI].baseState = notDetected;
    }
}

size_t EnemyFinder::size() {
    return removeDeadFinder();
}

/** TODO: multiple finders not support **/
bool EnemyFinder::runEnemyFinder() {
    pathFinder[0].toSearch = locationState;
    removeDeadFinder();

    if(pathFinder.size() == 0 || locationState.size() == 0){
        return false;
    }

    searching();
}

void EnemyFinder::searching() {
    std::vector<LocationState>::iterator it = pathFinder[0].toSearch.begin();
    while(it != pathFinder[0].toSearch.end()){
        if((*it).baseState == notDetected){
            it++;
        } else {
            it = locationState.erase(it);
        }
    }

    for (auto dest : pathFinder[0].toSearch){
        Actions()->UnitCommand(pathFinder[0].unit, ABILITY_ID::SMART,
                               dest.location);
    }
}

