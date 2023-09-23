###############################################################################
cmake_minimum_required (VERSION 3.18)

project (Stechuhr VERSION 0.0.1
                 DESCRIPTION "A time tracking utility"
                 LANGUAGES CXX)
###############################################################################


# set output directories to "bin"
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# compiler settings
###############################################################################
# C++17 is required for QT
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# warning level
if (MSVC)
    add_compile_options (/W3)
else()
    add_compile_options (-Wall -Wextra -Wpedantic -fstack-protector-all)
endif()

# check dependencies
###############################################################################
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets LinguistTools)
qt_standard_project_setup()

# preprocessor definitions
###############################################################################
add_compile_definitions (APP_VERSION="${PROJECT_VERSION}")

# generate build numbers
###############################################################################
execute_process (COMMAND git rev-parse --short HEAD    OUTPUT_VARIABLE GIT_COMMIT_SHORT ERROR_QUIET)

if (NOT "${GIT_COMMIT_SHORT}" STREQUAL "")
    execute_process (COMMAND git describe --exact-match --tags OUTPUT_VARIABLE GIT_TAG ERROR_QUIET)
    execute_process (COMMAND git rev-parse --abbrev-ref HEAD OUTPUT_VARIABLE GIT_BRANCH)
    execute_process (COMMAND git rev-parse HEAD OUTPUT_VARIABLE GIT_COMMIT)
    execute_process(
        COMMAND git diff --quiet --exit-code
        RESULT_VARIABLE GIT_HAS_LOCAL_CHANGES
        )

    if (${GIT_HAS_LOCAL_CHANGES} EQUAL 1)
        set (GIT_HAS_LOCAL_CHANGES "+")
    endif ()

    string (STRIP "${GIT_COMMIT_SHORT}" GIT_COMMIT_SHORT)
    string (STRIP "${GIT_COMMIT}" GIT_COMMIT)
    string (STRIP "${GIT_TAG}" GIT_TAG)
    string (STRIP "${GIT_BRANCH}" GIT_BRANCH)

    set (GIT_COMMIT_SHORT ${GIT_COMMIT_SHORT}${GIT_HAS_LOCAL_CHANGES})
    set (GIT_COMMIT ${GIT_COMMIT}${GIT_HAS_LOCAL_CHANGES})

    add_compile_definitions (GIT_COMMIT_SHORT="${GIT_COMMIT_SHORT}")
    add_compile_definitions (GIT_TAG="${GIT_TAG}")
    add_compile_definitions (GIT_BRANCH="${GIT_BRANCH}")
    add_compile_definitions (GIT_COMMIT="${GIT_COMMIT}")
endif()

string(TIMESTAMP BUILD_TIME "%Y-%m-%d %H:%M" UTC)
add_compile_definitions(BUILD_TIME="${BUILD_TIME}")
add_compile_definitions (BUILD_TYPE="${CMAKE_BUILD_TYPE}")

message (STATUS "Build infos:")
message (STATUS "  type       = ${CMAKE_BUILD_TYPE}")
message (STATUS "  timestamp  = ${BUILD_TIME}")
message (STATUS "  git tag    = ${GIT_TAG}")
message (STATUS "  git branch = ${GIT_BRANCH}")
message (STATUS "  git commit = ${GIT_COMMIT}")


# main target
###############################################################################
set (SOURCE_DIR src)
set (TS_DIR ${SOURCE_DIR}/I10n)
set (SOURCES
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/mainDialog.ui
    ${SOURCE_DIR}/maindialog.h
    ${SOURCE_DIR}/maindialog.cpp
    ${SOURCE_DIR}/stechuhr.h
    ${SOURCE_DIR}/stechuhr.cpp
    ${SOURCE_DIR}/stechuhr.qrc
)
set(CMAKE_AUTORCC ON)
qt_add_executable (stechuhr ${SOURCES})
qt_add_translations(stechuhr
    TS_FILES ${TS_DIR}/stechuhr_de.ts)

target_include_directories (stechuhr
    PRIVATE ${SOURCE_DIR})
target_link_libraries(stechuhr PRIVATE Qt6::Widgets)

set_target_properties(stechuhr PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

# deploy target
###############################################################################
install(TARGETS stechuhr
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
qt_generate_deploy_app_script(
    TARGET stechuhr
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
