enable_testing()

file(GLOB TEST_SOURCES *.cpp)

set(TEST_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TEST_OUTPUT_DIR ${TEST_SOURCE_DIR}/bin)
file(MAKE_DIRECTORY ${TEST_OUTPUT_DIR})

foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_name ${test_source} NAME_WE)
    
    add_executable(${test_name} ${test_source})
    
    set_target_properties(${test_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIR}
    )
    
    target_link_libraries(${test_name} PRIVATE UTILS)
    
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()