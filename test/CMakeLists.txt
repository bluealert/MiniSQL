enable_testing()

include(ExternalProject)

# gtest
set(GTEST_PREFIX gtest1.8.0)
set(GTEST_URL ${CMAKE_SOURCE_DIR}/third_party/googletest-1.8.0.tar.gz)
message(${GTEST_URL})

ExternalProject_Add(${GTEST_PREFIX}
        PREFIX ${GTEST_PREFIX}
        URL ${GTEST_URL}
        BUILD_IN_SOURCE 1
        INSTALL_COMMAND ""
        LOG_DOWNLOAD 1
        LOG_BUILD 1)

add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock ${GTEST_PREFIX})
add_library(libgmock_main IMPORTED STATIC GLOBAL)
add_dependencies(libgmock_main ${GTEST_PREFIX})

ExternalProject_Get_Property(${GTEST_PREFIX} source_dir binary_dir)

set_target_properties(libgmock PROPERTIES
        "IMPORTED_LOCATION" "${binary_dir}/googlemock/libgmock.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}")
set_target_properties(libgmock_main PROPERTIES
        "IMPORTED_LOCATION" "${binary_dir}/googlemock/libgmock_main.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}")

include_directories("${source_dir}/googlemock/include")
include_directories("${source_dir}/googletest/include")

# test utils
add_library(test_utils utils/TestTable.h utils/RandomInputGenerator.cpp)

macro(add_gtest test_name test_source)
    message(STATUS "add_gtest: ${test_name}, ${test_source}")
    add_executable(${test_name} ${test_source} TestMain.cpp)
    target_link_libraries(${test_name} minisql test_utils libgmock libgmock_main
            ${Boost_LIBRARIES} ${Glog_LIBRARIES})
    add_test(${test_name} bin/${test_name})
endmacro(add_gtest)

set(MINISQL_TEST_DIRS common storage query)

foreach(dir ${MINISQL_TEST_DIRS})
    file(GLOB_RECURSE test_files ${dir}/*Test.cpp)
    foreach(file_path ${test_files})
        get_filename_component(file_name "${file_path}" NAME)
        string(REGEX MATCH ".*/(.*Test).cpp" _ ${file_path})
        add_gtest(${CMAKE_MATCH_1} ${file_path})
    endforeach()
endforeach()