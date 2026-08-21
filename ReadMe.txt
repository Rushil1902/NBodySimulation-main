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

Default scene
-------------
The starting system is a hierarchical three-star system in the XY plane. Two
nearby stars orbit their shared center of mass while the pair and a third star
orbit the system center of mass. All three use the Sun texture so their
individual gravity wells are easy to compare.

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

Run the executable with the build directory as its working directory so the
current shader and texture paths resolve correctly.
