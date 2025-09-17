# =====================================================
# NekiraDelegate/CMakeLists.txt
# =====================================================

include(GNUInstallDirs)




# =====================================================
# SignalSlot Module
# =====================================================

# headers
file(GLOB_RECURSE SIGNALSLOT_HEADERS "SignalSlot/*.hpp")
# source files
file(GLOB_RECURSE SIGNALSLOT_SOURCES "${CMAKE_SOURCE_DIR}/source/SignalSlot/*.cpp")

# SignalSlot library
add_library(SignalSlot STATIC ${SIGNALSLOT_HEADERS} ${SIGNALSLOT_SOURCES})

# include directories
target_include_directories(SignalSlot
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>

    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/SignalSlot
)

# install headers
install(FILES ${SIGNALSLOT_HEADERS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/NekiraDelegate/SignalSlot
)



# =====================================================
# Core Module
# =====================================================

# headers
file(GLOB_RECURSE CORE_HEADERS "Core/*.hpp")
file(GLOB_RECURSE CORE_SOURCES "${CMAKE_SOURCE_DIR}/source/Core/*.cpp")

# Core library
add_library(DelegateCore STATIC ${CORE_HEADERS} ${CORE_SOURCES})

# include directories
target_include_directories(DelegateCore
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>

    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Core
)

# link libraries
target_link_libraries(DelegateCore PUBLIC SignalSlot)

# install headers
install(FILES ${CORE_HEADERS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/NekiraDelegate/Core
)



