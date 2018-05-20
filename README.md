# Project ShadowPeople

The purpose of this project is to build a minimal game engine for a 3-D adventure game. It is not a general purpose engine, but does only what is required for this specific task.

## Graphics

The graphics part of the project implements a basic 3-D engine using modern graphics algorithms. The back-end of the engine is separated from the rest of the engine, so that is easier to port it on different platforms. The initial back-end implementation is built on DX11.

The following features are currently planned:
* Supported geometry: height field, basic primitives (box, sphere, cylinder, etc.), custom models (*.obj?)
* Culling: Frustum and custom occlusion culling for height field, objects and clusters; back-face culling for clusters and triangles
* Rendering modes: Solid, alpha-clipped, transparency in limited cases
* Lighting: Physically-based reflection model (Diffuse + GGX), directional light (sun) and local lights, cascaded shadow maps + cube maps local lights, indirect lighting with light probes + SSAO
* Animation: Limited support?
* Modeled phenomena: Light-scattering fog, water, sky with clouds
* Post-processing: Bloom, depth-of-field, tone mapping, temporal anti-aliasing
* Debug and helpers: Line drawing, text drawing
