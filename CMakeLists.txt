cmake_minimum_required(VERSION 3.16)
project(WCSim)

# Paths
set(DEPENDENCIES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependencies")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Setup the project's languages
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_CUDA_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
find_package(CUDA REQUIRED)
project(WCSim VERSION 0.0.1 LANGUAGES CUDA CXX)

# Link files
file(GLOB_RECURSE PROJECT_SOURCES "${SOURCE_DIR}/*.cpp" "${SOURCE_DIR}/*.cu")
file(GLOB_RECURSE PROJECT_HEADERS "${SOURCE_DIR}/*.h" " ${SOURCE_DIR}/*.hpp")
file(GLOB_RECURSE PROJECT_SHADERS "${SOURCE_DIR}/shaders/**")

set (PROJECT_INCLUDE_DIRS "")
foreach (_headerFile ${PROJECT_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND PROJECT_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES Foo_INCLUDE_DIRS)
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})


# Link dependencies

# find_package(OpenGL REQUIRED)
# target_link_libraries(${PROJECT_NAME} OpenGL::GL)

## GLFW
set(glfw3_DIR "${DEPENDENCIES_DIR}/glfw")
add_subdirectory("${glfw3_DIR}")
# find_package(glfw3 REQUIRED)
target_link_libraries(${PROJECT_NAME} glfw)
target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_INCLUDEDIR})


## GLAD
set(GLAD_DIR "${DEPENDENCIES_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include" ${CMAKE_DL_LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad")

## GLM
set(GLM_DIR "${DEPENDENCIES_DIR}/glm")
add_subdirectory("${GLM_DIR}")
target_link_libraries(${PROJECT_NAME} glm)
target_include_directories(${PROJECT_NAME} PRIVATE ${GLM_INCLUDEDIR})

## DEBUGING variables
# get_cmake_property(_variableNames VARIABLES)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()
