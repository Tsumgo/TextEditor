cmake_minimum_required(VERSION 3.26.4)

set(CMAKE_GENERATOR "MinGW Makefiles")

project(TextEditor)

add_subdirectory(libgraphics)

aux_source_directory(./src ALL_SRC)
add_executable(TextEditor ${ALL_SRC})

target_link_libraries(TextEditor PUBLIC libgraphics)
# target_link_libraries(TextEditor PUBLIC SRC)

target_include_directories(TextEditor PUBLIC
                          ${PROJECT_BINARY_DIR}
                          ${PROJECT_SOURCE_DIR}/libgraphics
                          )
target_include_directories(TextEditor PUBLIC
                          ${PROJECT_BINARY_DIR}
                          ${PROJECT_SOURCE_DIR}/src)