#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "../ft_vox.hpp"
#include "../../src/diagnostics/ApplicationError.hpp"
#include "../../src/config/ApplicationOptions.hpp"
#include "../../src/config/LaunchSettings.hpp"
#include "../../src/config/LaunchSettingsMenu.hpp"
#include "../../src/platform/ApplicationWindow.hpp"
#include "../../src/validators/ApplicationValidator.hpp"
#include "../../src/config/CommandLine.hpp"
#include "../../src/diagnostics/DebugCrashHandler.hpp"
#include "../../src/policy/RenderPolicy.hpp"
#include "../../src/policy/RenderStrategyFactory.hpp"
#include "../../src/render/VoxelRenderer.hpp"
#include "../../src/settings/Settings.hpp"
#include "../../src/menu/MenuController.hpp"
#include "../../src/menu/MenuCanvas.hpp"
#include "../../src/app/GameSession.hpp"
#include "../../src/app/PerfSession.hpp"
#include "../../src/font/FontRenderer.hpp"

class Application
{
  public:
    enum class Phase
    {
        MAIN_MENU,
        LOADING,
        IN_GAME,
        IN_GAME_SETTINGS
    };

    Application();
    Application(const Application &other);
    ~Application();
    Application &operator=(const Application &other);

    static int run(int argc, char **argv);

  private:
    static ft_dumb_keyboard_layout detect_system_layout();
    static void wait_for_escape_release();
    static void clear_pending_quit_messages();
    static MenuCanvas::MenuInput collect_menu_input(ApplicationWindow &w);

    static int run_validators(const ApplicationOptions &options);
    static int setup_launch_config(ApplicationOptions &options, LaunchSettings &launch_settings);
    static int setup_window(ApplicationWindow &window, VoxelRenderer &renderer,
                            const LaunchSettings &launch_settings);

    static Phase tick_menu(Phase phase, ApplicationWindow &window, MenuController &menu,
                           GameSession &session, VoxelRenderer &renderer, int &loading_frames,
                           const RenderDistanceStrategy &strategy);
    static Phase tick_in_game(ApplicationWindow &window, MenuController &menu, GameSession &session,
                              VoxelRenderer &renderer, double dt,
                              const RenderDistanceStrategy &strategy);
    static Phase tick_in_game_settings(ApplicationWindow &window, MenuController &menu,
                                       GameSession &session);
    static void render_frame(Phase phase, ApplicationWindow &window, MenuController &menu,
                             GameSession &session, VoxelRenderer &renderer);
    static void run_game_loop(ApplicationWindow &window, MenuController &menu, GameSession &session,
                              VoxelRenderer &renderer, const RenderDistanceStrategy &strategy);
    static Phase tick_phase(Phase phase, ApplicationWindow &window, MenuController &menu,
                            GameSession &session, VoxelRenderer &renderer, double dt,
                            int &loading_frames, const RenderDistanceStrategy &strategy);
    static int run_game(ApplicationOptions &options, LaunchSettings &launch_settings,
                        const RenderDistanceStrategy &strategy);
};

#endif
