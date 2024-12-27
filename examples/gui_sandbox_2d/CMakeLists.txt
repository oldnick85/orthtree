set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_executable(gui_sandbox_2d
        gui_sandbox_2d.cpp 
        )

find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-4.0) # look into FindPkgConfig.cmake, 
                                                                    # it contains documentation
    # Now the variables GTKMM_INCLUDE_DIRS, GTKMM_LIBRARY_DIRS and GTKMM_LIBRARIES 
    # contain what you expect 
    
target_link_directories(gui_sandbox_2d
        PRIVATE ${GTKMM_LIBRARY_DIRS}  
        )
    
target_include_directories(gui_sandbox_2d
        PRIVATE ${GTKMM_INCLUDE_DIRS}
        )
        
target_link_libraries(gui_sandbox_2d 
        orthtree
        ${GTKMM_LIBRARIES}  
        )
