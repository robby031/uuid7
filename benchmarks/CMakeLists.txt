add_executable(bench_uuid7 bench_uuid7.c)
target_link_libraries(bench_uuid7 PRIVATE uuid7)
target_include_directories(bench_uuid7 PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_executable(bench_compare bench_compare.c)
target_link_libraries(bench_compare PRIVATE uuid7)
target_include_directories(bench_compare PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src)