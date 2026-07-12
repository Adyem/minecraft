# ft_vox

`ft_vox` is the voxel-world project from the Campus 19 subject: a very large
procedural cube world, smooth fullscreen rendering, an FPS-style camera, and a
skybox.

The repository already contains a working prototype built around Libft DUMB for
window/input glue and Libft Voxel/Game modules for chunk storage, deterministic
terrain data, and mesh generation.

The up-to-date task list is kept in [remaining.md](remaining.md). That file is
the source of truth for what is still missing from the mandatory and bonus
requirements.

## Current Focus

The old completed-task checklist has been removed from this README. The main
remaining blockers are:

- move rendering from the current CPU software framebuffer path to an approved
  direct graphics API path such as OpenGL, Vulkan, Metal, WebGPU, or OpenCL;
- verify smooth 160-cube visibility in a real fullscreen run;
- fix and verify Linux FPS mouse capture so the cursor cannot freely leave the
  centered look area;
- validate long-running memory stability and real GUI x20 movement;
- keep the bonus features working only after the mandatory part is solid.

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

For the detailed audit and next steps, see [remaining.md](remaining.md).
