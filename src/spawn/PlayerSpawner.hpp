#ifndef PLAYER_SPAWNER_HPP
#define PLAYER_SPAWNER_HPP

#include "../ft_vox.hpp"
#include "../../src/physics/PlayerCollision.hpp"

class PlayerSpawner
{
  public:
    PlayerSpawner();
    PlayerSpawner(const PlayerSpawner &other);
    ~PlayerSpawner();
    PlayerSpawner &operator=(const PlayerSpawner &other);

    static void spawn_player_on_ground(Camera *camera, const World &world);
};

#endif
