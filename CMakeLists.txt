cmake_minimum_required(VERSION 3.16)

set(KF_VERSION "5.85.0") # handled by release scripts
project(KCodecs VERSION ${KF_VERSION})

include(FeatureSummary)
find_package(ECM 5.85.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)


set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(KDEGitCommitHooks)
include(ECMQtDeclareLoggingCategory)

set(REQUIRED_QT_VERSION 5.15.0)
find_package(Qt5Core ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE)

find_package(Gperf REQUIRED)

include(ECMGenerateExportHeader)
include(CMakePackageConfigHelpers)
include(ECMSetupVersion)
include(ECMGenerateHeaders)

include(ECMPoQmTools)
include(ECMAddQch)

set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 0 CACHE STRING "Control the range of deprecated API excluded from the build [default=0].")

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

ecm_setup_version(PROJECT VARIABLE_PREFIX KCODECS
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kcodecs_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5CodecsConfigVersion.cmake"
                        SOVERSION 5)

ecm_install_po_files_as_qm(po)
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050f00)

add_subdirectory(src)

if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5Codecs")

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS KF5Codecs_QCH
        FILE KF5CodecsQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KF5CodecsQchTargets.cmake\")")
endif()

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/KF5CodecsConfig.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/KF5CodecsConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                              )

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/KF5CodecsConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/KF5CodecsConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel )

install(EXPORT KF5CodecsTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5CodecsTargets.cmake NAMESPACE KF5:: )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kcodecs_version.h
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel )

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
