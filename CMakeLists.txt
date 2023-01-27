cmake_minimum_required(VERSION 3.2)

project(cpp)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_executable(app src/main.cpp)

target_link_libraries(app PRIVATE SDL2::SDL2) 
target_link_libraries(app PRIVATE $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)
target_link_libraries(app PRIVATE imgui::imgui)
target_link_libraries(app PRIVATE fmt::fmt)

