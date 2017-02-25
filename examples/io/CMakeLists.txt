add_executable          (test-io    test-io.cpp)
target_link_libraries   (test-io    ${libraries})
file                    (COPY        ${CMAKE_CURRENT_SOURCE_DIR}/test.bin
                         DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file                    (COPY        ${CMAKE_CURRENT_SOURCE_DIR}/test.npy
                         DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

install                 (TARGETS
                        test-io
                        DESTINATION
                        ${CMAKE_INSTALL_PREFIX}/examples/io/
                        PERMISSIONS
                        OWNER_READ OWNER_WRITE OWNER_EXECUTE
                        GROUP_READ GROUP_WRITE GROUP_EXECUTE
                        WORLD_READ WORLD_WRITE WORLD_EXECUTE)


