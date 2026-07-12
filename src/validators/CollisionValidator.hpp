#ifndef COLLISION_VALIDATOR_HPP
# define COLLISION_VALIDATOR_HPP

#include "../ft_vox.hpp"
#include "../../src/validators/IValidator.hpp"
#include "../../src/diagnostics/ApplicationError.hpp"
#include "../../src/platform/InputReader.hpp"
#include "../../src/player/PlayerController.hpp"

class World;
class Camera;

class CollisionValidator : public IValidator
{
    private:
        struct Metrics
        {
            double start_x;
            double start_z;
            double movement_delta_z;
            double wall_delta_x;
            double climb_delta_y;
            double tunnel_delta_z;
            double drop_delta_y;
        };

        static int  test_movement(World &world, Camera &camera, Metrics &m);
        static int  test_wall(World &world, Camera &camera, Metrics &m);
        static int  setup_step_blocks(World &world, Camera &camera,
                                      int32_t &step_x, int32_t &step_y,
                                      int32_t &step_z);
        static bool run_climb_loop(Camera &camera, World &world, double start_y,
                                   double &climb_delta_y);
        static int  test_step_climb(World &world, Camera &camera, Metrics &m);
        static void setup_tunnel(World &world, Camera &camera, int32_t &tx,
                                 int32_t &ty, int32_t &tz);
        static int  test_tunnel(World &world, Camera &camera, Metrics &m);
        static void setup_drop(World &world, Camera &camera, double &drop_start_y);
        static int  test_drop(World &world, Camera &camera, Metrics &m);

    public:
        CollisionValidator();
        CollisionValidator(const CollisionValidator &other);
        ~CollisionValidator();
        CollisionValidator &operator=(const CollisionValidator &other);

        virtual int validate() const override;
};

#endif
