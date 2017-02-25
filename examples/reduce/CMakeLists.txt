set                     (targets    swap-reduce
                                    merge-reduce
                                    kd-tree
                                    sort
                                    sample-sort
                                    all-to-all
                                    all-done)

foreach                 (t ${targets})
  add_executable        (${t} ${t}.cpp)
  target_link_libraries (${t} ${libraries})
endforeach              ()

install                 (TARGETS
                        ${targets}
                        DESTINATION
                        ${CMAKE_INSTALL_PREFIX}/examples/reduce/
                        PERMISSIONS
                        OWNER_READ OWNER_WRITE OWNER_EXECUTE
                        GROUP_READ GROUP_WRITE GROUP_EXECUTE
                        WORLD_READ WORLD_WRITE WORLD_EXECUTE)

