add_executable              (simple simple.cpp)
target_link_libraries       (simple     ${libraries})

add_executable              (read-blocks read-blocks.cpp)
target_link_libraries       (read-blocks     ${libraries})

add_executable              (until-done until-done.cpp)
target_link_libraries       (until-done     ${libraries})

add_executable              (iexchange iexchange.cpp)
target_link_libraries       (iexchange     ${libraries})

install                 (TARGETS
                        simple
                        read-blocks
                        until-done
                        iexchange
                        DESTINATION
                        ${CMAKE_INSTALL_PREFIX}/examples/simple/
                        PERMISSIONS
                        OWNER_READ OWNER_WRITE OWNER_EXECUTE
                        GROUP_READ GROUP_WRITE GROUP_EXECUTE
                        WORLD_READ WORLD_WRITE WORLD_EXECUTE)


