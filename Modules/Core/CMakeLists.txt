# =====================================================
# Core/CMakeLists.txt
# =====================================================
include(GNUInstallDirs)

# headers
file(GLOB_RECURSE CORE_HEADERS "Public/*.hpp")
# sources
file(GLOB_RECURSE CORE_SOURCES "Private/*.cpp")

# Core library
add_library(Core SHARED ${CORE_HEADERS} ${CORE_SOURCES})

# include directories
target_include_directories(Core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Core>

    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public    
)

# =====================================================
# install
# =====================================================

install(FILES ${CORE_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Core
)