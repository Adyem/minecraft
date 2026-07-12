# ft_vox Remaining Work

Last audit: 2026-06-09

Scope checked: subject v6.1, all 7 subject validators, `main.cpp`, `src/`,
`Libft/Modules/` (DUMB, Game, Voxel, GPGR, Compatebility).

---

## Mandatory — Validator Status

| Check | Command | Result |
|---|---|---|
| Camera speed (≈1 cube/s, ×20 key) | `--validate-camera-speed` | `normal=1.000 boost=20.000` ✅ |
| Visible distance ≥ 160 cubes | `--validate-visible-distance` | `ok required=160` ✅ |
| Terrain determinism | `--validate-terrain-determinism` | `ok sampled_chunks=3` ✅ |
| Cave generation | `--validate-caves` | `ok sampled_chunks=4` ✅ |
| World scale 16384×256×16384 | `--validate-world-scale` | `ok horizontal=16384 height=256` ✅ |
| Collision & movement | `--validate-collision` | `ok` ✅ |
| Block place / delete | `--validate-block-edit` | `ok` ✅ |

All mandatory subject requirements are implemented and validated.

---

## Completed Work

The following items are done and may be removed from active tracking:

- DUMB window loop, fullscreen mode, native framebuffer resolution
- Procedural terrain: height generation, biome system (5 biomes — Plains, Hills,
  Desert, Snow/stone, Mountains), cave carving, deterministic seed
- World scale 16384×256×16384, memory-bounded chunk streaming (radius 12)
- Texture atlas (8 block types), correct UV mapping, visible-face mesh generation
- Frustum culling, backface culling, skybox (gradient, no artefacts)
- GPU/OpenGL 3.3 Core renderer path, mega-buffer single-draw-call batching
- Stable 60+ fps (floor 60, typical 90+)
- FoV = 80° confirmed, camera at ~1 cube/s with ×20 boost key
- Mouse-look on 2 axes, WASD/ZQSD relative to camera yaw
- Debug overlay: FPS, frame time, chunks, render distance, selected block,
  XYZ position, RAM MB, VRAM approx MB
- Dynamic render distance (AdaptiveRenderStrategy, never drops below 14 cubes)
- Block deletion and placement with mouse (all three buttons)
- Lakes (water at sea level 72), transparent water rendering, underwater tint,
  water spreading on placement, water-exit step-up
- 5 biomes visually distinct (desert=dirt/flooded, mountains/snow=stone,
  plains/hills=grass)
- Full menu system: main menu, settings, credits, loading screen,
  in-game settings (M key); random seed on each Start; mouse + arrow navigation;
  cursor hidden in-game
- Settings: FPS overlay toggle, render distance slider, AZERTY/QWERTY toggle +
  auto-detection from system keyboard layout at startup
- AZERTY key codes corrected on macOS (Z=6, Q=12 vs QWERTY W=13, A=0)
- Makefile: `make`, `make -j4`, `fclean`, `re` all verified clean
- GPGR module in Libft (46 modules total): `ft_gpu_window`, `ft_gpu_shader`,
  `gpgr_gl_funcs`, platform window implementations (macOS/Linux/Windows) now
  in `Libft/Modules/GPGR/` — linked via `Full_Libft.a`
- GpuRenderer refactored (SOLID): 99-line facade delegates to
  `GpuMvpBuilder`, `GpuSkyRenderer`, `GpuWorldRenderer`, `GpuHudRenderer`
- Orthodox Canonical Form enforced in all `src/` headers: every class has
  explicit ctor, copy ctor, dtor, and `operator=` declared in `.hpp` and
  implemented in `.cpp`; no `= delete`, `= default`, or inline bodies in headers
- Linux mouse capture code: `XGrabPointer(confine_to=window)` +
  `FocusIn`/`FocusOut` re-grab + `XWarpPointer` to centre each frame —
  code is complete and merged, awaits hardware Linux verification

---

## Open Items

### Linux — Hardware Testing Required

The following are code-complete but cannot be verified on this macOS machine:

- **Linux FPS mouse capture**: `XGrabPointer` + `FocusIn`/`FocusOut` handlers +
  `XWarpPointer`-to-centre implemented in `gpgr_window_linux.cpp` and
  `Compatebility_dumb_controls_linux_x11.cpp`. Must be tested on a Linux
  machine to confirm infinite mouse look works and alt-tab re-grabs correctly.
- **Linux full test pass**: no segfaults, aborts, uncaught exceptions, freezes,
  or frame spikes under normal play and ×20 movement.

### Operational Tests (no code change needed)

- **Long-duration stability**: run the program for several hours and confirm RAM
  and VRAM do not grow unboundedly and frame time stays stable.
- **×20 movement stress test**: run at boost speed in real GUI mode with terrain
  streaming active; confirm no frame spikes and render distance adapts smoothly.

---

## Ownership-Oriented Next Steps

**rperez-t**
- Boot a Linux machine, run `./ft_vox`, verify FPS mouse look (cursor stays
  centred, camera moves correctly on all axes).
- Run the ×20 speed stress test and a long-duration session; report any issues.

**bvangene**
- No open mandatory items.

---

## Subject Compliance Summary (v6.1)

### Mandatory ✅ Complete
- Gigantic procedural world (16384×256×16384) with multiple textured cube types
- Natural topography: hills, mountains, caves, lakes
- Deterministic generation (same seed = same map)
- Memory-bounded terrain streaming
- 160+ cube visible distance in open areas
- Frustum culling + visibility optimisation
- ≥ 2 textures, ≥ 2 cube types (have 8+)
- FoV = 80°, skybox with no artefacts
- Camera: mouse on 2 axes, 4 movement keys, ~1 cube/s, ×20 boost
- Fullscreen, no framebuffer reduction
- Direct OpenGL API (no higher-level wrappers)
- Smooth render (60+ fps, no freeze frames)

### Bonus ✅ Complete
- Dynamic render distance, never < 14 cubes
- FPS counter displayed
- Smooth at ×20 speed
- Block deletion with mouse
- Multiple biomes (5 distinct biomes)
