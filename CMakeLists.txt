cmake_minimum_required(VERSION 3.20)

project(NekiraDelegate)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(HEADER_DIRS 
    ${CMAKE_CURRENT_SOURCE_DIR}/CallableInterface
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate
    ${CMAKE_CURRENT_SOURCE_DIR}/FunctionTraits
)
   

add_executable(NekiraDelegateDemo ${CMAKE_CURRENT_SOURCE_DIR}/Demo/NekiraDelegate.cpp)   

add_library(NekiraDelegate INTERFACE)
target_include_directories(NekiraDelegate INTERFACE ${HEADER_DIRS})


target_link_libraries(NekiraDelegateDemo INTERFACE NekiraDelegate)

