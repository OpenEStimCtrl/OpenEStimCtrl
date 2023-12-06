cmake_minimum_required(VERSION 3.25)
project(OpenEStimCtrl)
project(OpenEStimCtrlStatic)
set(CMAKE_CXX_STANDARD 17)

add_library(OpenEStimCtrl SHARED src/YokoNexES01.cpp src/YokoNexES01.h src/DGLabESTIM01.cpp src/DGLabESTIM01.h src/OpenEStimCtrl.h wavegen/DGLabESTIM01WaveGen.cpp wavegen/DGLabESTIM01WaveGen.h)
add_library(OpenEStimCtrlStatic STATIC src/YokoNexES01.cpp src/YokoNexES01.h src/DGLabESTIM01.cpp src/DGLabESTIM01.h src/OpenEStimCtrl.h wavegen/DGLabESTIM01WaveGen.cpp wavegen/DGLabESTIM01WaveGen.h)
set_target_properties(OpenEStimCtrlStatic PROPERTIES OUTPUT_NAME OpenEStimCtrlStatic ARCHIVE_OUTPUT_DIRECTORY library_static)