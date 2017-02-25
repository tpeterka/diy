find_path           (DIY_INCLUDE_DIR master.hpp HINTS
                    ${DIY_PREFIX}/include/diy
                    /usr/include/diy
                    /usr/local/include/diy
                    /opt/local/include/diy)

if                  (DIY_INCLUDE_DIR)
    set             (DIY_FOUND 1 CACHE BOOL "Found diy")
    string          (REGEX REPLACE "include/diy" "include" DIY_INCLUDE_DIR ${DIY_INCLUDE_DIR})
else                ()
    set             (DIY_FOUND 0 CACHE BOOL "Did not find diy")
    if              (DIY_FIND_REQUIRED)
        message     (FATAL_ERROR "Diy not found. Please install and/or provide DIY_PREFIX")
    endif           ()
endif               ()

mark_as_advanced    (
                    DIY_INCLUDE_DIR
                    DIY_FOUND)

