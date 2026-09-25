N-Body Simulation
=================

The physics engine uses astronomical units (AU), years, and solar masses.
Rendering uses four screen-space units per AU so the camera can still show
systems wider than 80 AU.

Project structure
-----------------
Application  Owns the window, input handling, and main loop
Simulation   Owns simulation time, physics stepping, controls, and diagnostics
Scene        Owns bodies, default initial conditions, and reset state
Renderer     Owns OpenGL resources, camera integration, and drawing

In-app laboratory panel
-----------------------
The panel provides play/pause/reset/step controls, simulation speed and
timestep editing, energy and momentum diagnostics, body selection and
editing, add/remove controls, camera following, texture selection, and
trail/field-slice visibility toggles. Editing physical state pauses the
simulation and establishes a new energy baseline.

Gravity field slice
-------------------
The default scene displays an XY cross-section of the full three-dimensional
gravitational potential. The Slice Z control moves the sampled plane above or
below the orbital plane. The view can be shown as a bounded rubber-sheet graph
or a flat color heatmap, and it can follow the selected body's Z position.
Well depth, width, intensity, surface opacity, and the displayed AU range are
adjustable. The visualization is illustrative and remains finite when a body
crosses a grid vertex.

Procedural planets
------------------
Planet surfaces can use an ordinary texture or one of four seamless,
seed-driven procedural materials: Earth-like, gas giant, lava, and icy moon. The
procedural shader samples three-dimensional noise using each sphere's local
position, avoiding UV seams and keeping the material attached while the body
rotates. Earth-like worlds, gas giants, and icy moons have editable seven-color
palettes plus independent seed and palette randomizers for fantastical
science-fiction worlds. Vibrancy and color-banding controls range from softer
natural blending to bold graphic color regions. Lava keeps its fixed
black-and-orange emissive palette. Surface seed, scale, material settings,
animation speed, colors, and visual size are adjustable without changing the
body's physics.

Procedural stars
----------------
Stars use a seamless animated cellular shader for convection granules, turbulent
plasma, hot cell centers, and darker starspot clusters. A second noise-displaced
additive shell forms the irregular corona and short flare-like protrusions. The
solar, blue-white, red-dwarf, and alien presets each have editable seven-color
palettes, independent seed and palette randomizers, activity and starspot
controls, surface motion, and adjustable corona size and intensity. Stellar
appearance is independent of mass, radius, and gravity. The primary star's hot
surface color also gives planet lighting a subtle matching tint.

Default procedural showcase
---------------------------
The starting system contains a procedural blue-white star and four orbiting
procedural planets demonstrating Earth-like, gas-giant, lava, and icy presets.

Controls
--------
Space  Pause or resume
N      Advance one fixed physics step while paused
R      Reset the original system and pause
- / =  Decrease or increase simulation speed

Build and validate
------------------
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure

The build stages shaders and textures in an assets directory beside the
executable. The simulator resolves assets from the executable location, so it
can be launched from the build directory, repository root, an IDE, or another
working directory. Keep the generated executable and assets directory together
when moving the program. Missing, empty, uncompilable, or un-linkable shaders
now stop startup and report the exact file and graphics-driver diagnostic.
