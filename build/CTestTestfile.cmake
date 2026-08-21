# CMake generated Testfile for 
# Source directory: C:/Users/rushi/OneDrive/Documents/NBodySimulation-main
# Build directory: C:/Users/rushi/OneDrive/Documents/NBodySimulation-main/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(TwoBodyOrbitValidation "C:/Users/rushi/OneDrive/Documents/NBodySimulation-main/build/PhysicsValidation.exe")
set_tests_properties(TwoBodyOrbitValidation PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/rushi/OneDrive/Documents/NBodySimulation-main/CMakeLists.txt;76;add_test;C:/Users/rushi/OneDrive/Documents/NBodySimulation-main/CMakeLists.txt;0;")
subdirs("thirdparty/glfw")
subdirs("thirdparty/glm")
