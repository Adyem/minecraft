# ft_minecraft

The repository is now the base for **`ft_minecraft`**, the follow-up project:
diverse biomes, rivers, caves with ore clusters, monsters, multiplayer, and
advanced rendering (lighting, shadows, SSAO, fog). The active task list,
owner-tagged milestones, and bonus scope are in
[MILESTONES.md](MILESTONES.md) — that is now the source of truth for
day-to-day work.

The prototype is built around Libft DUMB for window/input glue and Libft
Voxel/Game modules for chunk storage, deterministic terrain data, and mesh
generation.

## Current Focus

Building `ft_minecraft` on top of the working `ft_vox` engine. See
[MILESTONES.md](MILESTONES.md) for the full plan; in short:

- lock shared contracts (persistence format, network protocol, entity state)
  before splitting off into parallel tracks;
- Track A (bvangene): world/biome overhaul, caves & ore clusters, vegetation,
  player mechanics (sprint/fly/swim), monsters, and the gameplay bonus
  features (growing plants, crafting, bow & arrow, villages, nether portal,
  water simulation);
- Track B (rperez-t): persistence, advanced rendering (lighting, shadows,
  SSAO, fog, clouds), server-authoritative multiplayer netcode, interface
  and audio, and cross-platform packaging.

## Build

Current build command:

```sh
make
```

Submodule initialization:

```sh
make submodule_init
```

Submodule update:

```sh
make submodule_update
```

Clean build files:

```sh
make fclean
```

For the active `ft_minecraft` plan and milestones, see [MILESTONES.md](MILESTONES.md).
