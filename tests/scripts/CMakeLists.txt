install                     (FILES
                            kd-tree.sh
                            merge-swap-reduce.sh
                            simple.sh
                            swap-reduce.sh
                            DESTINATION
                            ${CMAKE_INSTALL_PREFIX}/tests/scripts/
                            PERMISSIONS
                            OWNER_READ OWNER_WRITE OWNER_EXECUTE
                            GROUP_READ GROUP_WRITE GROUP_EXECUTE
                            WORLD_READ WORLD_WRITE WORLD_EXECUTE)


