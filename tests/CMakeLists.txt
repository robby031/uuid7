add_executable(test_uuid7 test_uuid7.c)
target_link_libraries(test_uuid7 PRIVATE uuid7)
target_include_directories(test_uuid7 PRIVATE ${PROJECT_SOURCE_DIR}/include)
add_test(NAME uuid7_unit COMMAND test_uuid7)

add_executable(test_clock_backward test_clock_backward.c)
target_link_libraries(test_clock_backward PRIVATE uuid7)
target_include_directories(test_clock_backward PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src)
add_test(NAME uuid7_clock_backward COMMAND test_clock_backward)

add_executable(test_stress test_stress.c)
target_link_libraries(test_stress PRIVATE uuid7)
target_include_directories(test_stress PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src)

add_executable(test_rfc9562 test_rfc9562.c)
target_link_libraries(test_rfc9562 PRIVATE uuid7)
target_include_directories(test_rfc9562 PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src)
add_test(NAME uuid7_rfc9562 COMMAND test_rfc9562)

add_executable(test_stress_mt test_stress_mt.c)
target_link_libraries(test_stress_mt PRIVATE uuid7 pthread)
target_include_directories(test_stress_mt PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src)
add_test(NAME uuid7_stress_mt COMMAND test_stress_mt)