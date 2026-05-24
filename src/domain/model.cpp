#include "domain/model.h"

#include <stdexcept>

namespace model {
using namespace std::literals;

namespace {

struct RoadBounds {
    double min_x = 0;
    double max_x = 0;
    double min_y = 0;
    double max_y = 0;
};

RoadBounds GetRoadBounds(const Road& road) {
    const Point road_start = road.GetStart();
    const Point road_end = road.GetEnd();
    const double half_width = ROAD_WIDTH / 2;

    RoadBounds bounds;
    bounds.min_x = (road_start.x < road_end.x ? road_start.x : road_end.x) - half_width;
    bounds.max_x = (road_start.x > road_end.x ? road_start.x : road_end.x) + half_width;
    bounds.min_y = (road_start.y < road_end.y ? road_start.y : road_end.y) - half_width;
    bounds.max_y = (road_start.y > road_end.y ? road_start.y : road_end.y) + half_width;

    return bounds;
}

bool IsPositionOnRoad(const Vector2& position, const Road& road) {
    const RoadBounds bounds = GetRoadBounds(road);

    return position.x >= bounds.min_x && position.x <= bounds.max_x &&
           position.y >= bounds.min_y && position.y <= bounds.max_y;
}

std::vector<Road> GetPositionRoads(const Vector2& position, const Map& map) {
    std::vector<Road> picked_roads;

    for (const Road& road : map.GetRoads()) {
        if (IsPositionOnRoad(position, road)) {
            picked_roads.emplace_back(road);
        }
    }

    return picked_roads;
}

Vector2 ClampMovementToRoads(const Vector2& position, Vector2 target, const std::vector<Road>& roads) {
    if (target.x != position.x) {
        bool found_interval = false;
        double min_x = 0;
        double max_x = 0;

        for (const Road& road : roads) {
            const RoadBounds bounds = GetRoadBounds(road);
            if (position.y >= bounds.min_y && position.y <= bounds.max_y) {
                if (!found_interval) {
                    min_x = bounds.min_x;
                    max_x = bounds.max_x;
                    found_interval = true;
                } else {
                    if (bounds.min_x < min_x) {
                        min_x = bounds.min_x;
                    }
                    if (bounds.max_x > max_x) {
                        max_x = bounds.max_x;
                    }
                }
            }
        }

        if (found_interval) {
            if (target.x < min_x) {
                target.x = min_x;
            } else if (target.x > max_x) {
                target.x = max_x;
            }
        } else {
            target.x = position.x;
        }
    }

    if (target.y != position.y) {
        bool found_interval = false;
        double min_y = 0;
        double max_y = 0;

        for (const Road& road : roads) {
            const RoadBounds bounds = GetRoadBounds(road);
            if (position.x >= bounds.min_x && position.x <= bounds.max_x) {
                if (!found_interval) {
                    min_y = bounds.min_y;
                    max_y = bounds.max_y;
                    found_interval = true;
                } else {
                    if (bounds.min_y < min_y) {
                        min_y = bounds.min_y;
                    }
                    if (bounds.max_y > max_y) {
                        max_y = bounds.max_y;
                    }
                }
            }
        }

        if (found_interval) {
            if (target.y < min_y) {
                target.y = min_y;
            } else if (target.y > max_y) {
                target.y = max_y;
            }
        } else {
            target.y = position.y;
        }
    }

    return target;
}

}  // namespace

bool operator==(const Vector2 & v1, const Vector2 & v2) {
    return v1.x == v2.x && v1.y == v2.y;
}

bool operator!=(const Vector2 & v1, const Vector2 & v2) {
    return !(v1==v2);
}

std::vector<Road> GetDogStandRoads(const Dog & dog, const Map & map) {
    return GetPositionRoads(dog.GetPosition(), map);
}

void Map::AddOffice(Office office) {
    if (warehouse_id_to_index_.contains(office.GetId())) {
        throw std::invalid_argument("Duplicate warehouse");
    }

    const size_t index = offices_.size();
    Office& o = offices_.emplace_back(std::move(office));
    try {
        warehouse_id_to_index_.emplace(o.GetId(), index);
    } catch (...) {
        offices_.pop_back();
        throw;
    }
}

void Game::AddMap(Map map) {
    const size_t index = maps_.size();
    if (auto [it, inserted] = map_id_to_index_.emplace(map.GetId(), index); !inserted) {
        throw std::invalid_argument("Map with id "s + *map.GetId() + " already exists"s);
    } else {
        try {
            maps_.emplace_back(std::move(map));
        } catch (...) {
            map_id_to_index_.erase(it);
            throw;
        }
    }
}

void GameSession::Update(unsigned int delta_time) {
    for (Dog & dog : dogs_) {
        Vector2 position = dog.GetPosition();
        Vector2 speed = dog.GetSpeed();

        const std::vector<Road> current_roads = GetPositionRoads(position, *map_);
        if (!current_roads.empty()) {
            Vector2 new_position = position;
            new_position.x += speed.x * (delta_time / MILLISECONDS_IN_SECOND);
            new_position.y += speed.y * (delta_time / MILLISECONDS_IN_SECOND);

            if (!GetPositionRoads(new_position, *map_).empty()) {
                position = new_position;
            } else {
                position = ClampMovementToRoads(position, new_position, current_roads);
                dog.SetSpeed(Vector2{0, 0});
            }
        }

        dog.SetPosition(position);
    
    }
}

}  // namespace model
