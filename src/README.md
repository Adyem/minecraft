# Source Layout

This folder is organized by file responsibility using shallow, single-word folder names. Each folder lives directly under `src`, and class filenames should make it obvious why they belong there.

## Runtime Flow

`main.cpp` calls `Application::run()`. From there, the application owns the window, input, player controller, world, render policy, and renderer. A frame is roughly:

1. `app` runs the high-level loop.
2. `config` parses CLI options.
3. `platform` talks to the DUMB window/input/framebuffer layer.
4. `player`, `movement`, `physics`, and `spawn` update the camera/player state.
5. `interaction` converts player actions into block edits.
6. `validators` contains all command-line and integration checks.
7. `policy` decides render distance and generation budget.
8. `world`, `chunks`, `coordinates`, and `queries` provide terrain data.
9. `edits` applies command-pattern block changes.
10. `render` coordinates the render pass through the render helper folders.

## Runtime terrain customization

The application can configure terrain before starting a `GameSession`; the
configuration is copied into `World` and passed to every generated chunk.
Libft keeps the low-level generator and built-in templates, while the game
chooses the policy:

```cpp
terrain_generation_config terrain = terrain_default_generation_config();
terrain.sea_level = 68;
terrain.water_chance_percent = 55;
terrain.biomes[0].profile.height_variation = 10; // uneven plains
terrain.biomes[0].tree_chance_percent = 8;
terrain.biomes[0].tree_template = &terrain_large_oak_tree_template();

GameSession session;
session.set_terrain_generation_config(terrain);
session.start(seed, window, renderer);
```

Custom biome slots are selected with `biome_selector`; custom tree/object
templates are supplied through `terrain_feature_rule`. Validate a policy with
`terrain_generation_config_is_valid` before starting generation. The
integration check is available through `--validate-terrain-configuration`.

## Folders

- `app`: `Application`, the application facade and main loop entry point.
- `config`: `ApplicationOptions` and `CommandLine` parsing.
- `platform`: window and input integration.
- `diagnostics`: errors, crash handling, and framebuffer hashing.
- `camera`: camera state and raw camera input data.
- `player`: player controller facade.
- `movement`: player movement intent and camera motion.
- `physics`: collision, ground checks, and player geometry.
- `spawn`: spawn placement rules.
- `interaction`: block selection, placement, and deletion from player actions.
- `validators`: validation facade plus camera, collision, block edit, and visible-distance checks.
- `policy`: render-distance strategy and budget policy classes.
- `render`: `VoxelRenderer`, the renderer facade.
- `frame`: render target, frame cache, and color helpers.
- `geometry`: render vertex and triangle texture data.
- `assets`: texture atlas ownership.
- `debug`: debug view, overlay, and debug payloads.
- `sky`: skybox rendering.
- `meshes`: chunk mesh facade, culling, clipping, projection, and rasterizer strategies.
- `world`: `World`, the world facade.
- `chunks`: world chunk data, storage, and loading.
- `coordinates`: world/chunk coordinate conversion helpers.
- `queries`: block lookup and raycasting.
- `edits`: command-pattern block edit operations.

## Folder Rule

Folder names are single words. Group files by the responsibility visible in the class/file name first, then by the domain facade they support. For example, `ChunkMeshRenderer` belongs in `meshes`, while `WorldChunkLoader` belongs in `chunks`.
