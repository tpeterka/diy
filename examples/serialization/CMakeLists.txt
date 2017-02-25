add_executable          (test-serialization test-serialization.cpp)

install                 (TARGETS
                        test-serialization
                        DESTINATION
                        ${CMAKE_INSTALL_PREFIX}/examples/serialization/
                        PERMISSIONS
                        OWNER_READ OWNER_WRITE OWNER_EXECUTE
                        GROUP_READ GROUP_WRITE GROUP_EXECUTE
                        WORLD_READ WORLD_WRITE WORLD_EXECUTE)


