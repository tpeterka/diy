add_executable          (regular-decomposer-short regular-decomposer-short.cpp)
target_link_libraries   (regular-decomposer-short ${libraries})
add_executable          (regular-decomposer-long  regular-decomposer-long.cpp)
target_link_libraries   (regular-decomposer-long  ${libraries})
add_executable          (test-decomposition       test-decomposition.cpp)
target_link_libraries   (test-decomposition       ${libraries})
add_executable          (test-direct-master       test-direct-master.cpp)
target_link_libraries   (test-direct-master       ${libraries})

install                 (TARGETS
                        regular-decomposer-short
                        regular-decomposer-long
                        test-decomposition
                        test-direct-master
                        DESTINATION
                        ${CMAKE_INSTALL_PREFIX}/examples/decomposition/
                        PERMISSIONS
                        OWNER_READ OWNER_WRITE OWNER_EXECUTE
                        GROUP_READ GROUP_WRITE GROUP_EXECUTE
                        WORLD_READ WORLD_WRITE WORLD_EXECUTE)

