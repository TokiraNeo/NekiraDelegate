# =====================================================
# Root/CMakeLists.txt
# =====================================================

cmake_minimum_required(VERSION 3.20)

project(NekiraDelegateLib LANGUAGES CXX VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# =====================================================
# NekiraDelegateLib
# =====================================================

# 添加子目录
add_subdirectory(Modules)

# 定义包含的模块
set(Module_Targets SignalSlot Core)

# 收集所有的模块目标
set(NekiraDelegateLib_Modules)
foreach(module ${Module_Targets})
    list(APPEND NekiraDelegateLib_Modules ${module})
endforeach()

# 创建总目标
add_custom_target(NekiraDelegateLib)

# 将所有模块添加到总目标中
add_dependencies(NekiraDelegateLib ${NekiraDelegateLib_Modules})

# =====================================================
# install
# =====================================================

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# 配置文件目录
set(CONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/NekiraDelegateLib)

# 创建版本文件
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NekiraDelegateLibConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# 指定要安装的目标
install(TARGETS ${NekiraDelegateLib_Modules}
        EXPORT NekiraDelegateLibTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# 指定NekiraDelegateLib的Targets.cmake文件
install(EXPORT NekiraDelegateLibTargets
        FILE NekiraDelegateLibTargets.cmake
        NAMESPACE NekiraDelegateLib::
        DESTINATION ${CONFIG_INSTALL_DIR}
)

# 配置NekiraDelegateLibConfig.cmake
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NekiraDelegateLibConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/NekiraDelegateLibConfig.cmake"
    INSTALL_DESTINATION ${CONFIG_INSTALL_DIR}
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR
)

# 安装配置文件
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/NekiraDelegateLibConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/NekiraDelegateLibConfigVersion.cmake"
        DESTINATION ${CONFIG_INSTALL_DIR}
)

# =====================================================
# export
# =====================================================

# 导出NekiraDelegateLib的Targets
export(EXPORT NekiraDelegateLibTargets
       FILE "${CMAKE_CURRENT_BINARY_DIR}/NekiraDelegateLibTargets.cmake"
       NAMESPACE NekiraDelegateLib::
)

# 统一导出为NekiraDelegateLib包
export(PACKAGE NekiraDelegateLib)