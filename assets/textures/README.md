# ft_vox Textures

`voxel_atlas.bmp` is the runtime 4x4 voxel texture atlas loaded by the software renderer.

The atlas is intentionally original rather than Minecraft-like. It uses a stylized alien terrain palette:

- aurora moss top
- moss side over violet soil
- red clay strata
- blue basalt flecks
- spiralwood log rings
- prism leaves
- ribbed teal cactus
- dry reed shrub
- extra mineral, frost, crystal, shimmer, sand, ember, and night-leaf tiles for future block types

Renderer status:

- The software renderer loads `voxel_atlas.bmp`.
- If the asset is missing or invalid, the renderer falls back to matching non-Minecraft procedural colors instead of crashing.

Asset direction:

Use saturated teal, violet, red-clay, amber, and blue-basalt colors. Avoid grass/dirt/stone textures that resemble Minecraft's default visual language.
