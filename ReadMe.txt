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
