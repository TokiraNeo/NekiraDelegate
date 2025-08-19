# =====================================================
# SignalSlot/CMakeLists.txt
# =====================================================
include(GNUInstallDirs)

# headers
file(GLOB_RECURSE SIGNALSLOT_HEADERS "Public/*.hpp")
# sources
file(GLOB_RECURSE SIGNALSLOT_SOURCES "Private/*.cpp")

# SignalSlot library
add_library(SignalSlot SHARED ${SIGNALSLOT_HEADERS} ${SIGNALSLOT_SOURCES})

# include directories
target_include_directories(SignalSlot
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/SignalSlot>

    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public    
)

# =====================================================
# install
# =====================================================

install(FILES ${SIGNALSLOT_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SignalSlot
)