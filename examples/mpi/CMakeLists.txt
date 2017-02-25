add_executable          (test-mpi test-mpi.cpp)
target_link_libraries   (test-mpi ${libraries})

install                 (TARGETS
                        test-mpi
                        DESTINATION
                        ${CMAKE_INSTALL_PREFIX}/examples/mpi/
                        PERMISSIONS
                        OWNER_READ OWNER_WRITE OWNER_EXECUTE
                        GROUP_READ GROUP_WRITE GROUP_EXECUTE
                        WORLD_READ WORLD_WRITE WORLD_EXECUTE)


