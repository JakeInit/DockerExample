cmake_minimum_required(VERSION 3.12)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

project(dockerExample VERSION 0.0.1 DESCRIPTION "Docker Example")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF) # do not look for boost libraries linked against static C++ std lib

find_package(Boost REQUIRED COMPONENTS filesystem)

# Make Executable(s)
add_executable (${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/main.cpp)
target_compile_definitions(${PROJECT_NAME} PRIVATE PROJECTDIR="${PROJECT_SOURCE_DIR}/")
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} Boost::filesystem)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

#Boost is linked statically since it is required if the target machine does not have
#the right version of Boost pre-installed; this recommendation applies to all dependencies
#pre-installed in the docker image.
