# GUI example with GTKMM
find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-4.0)

if(GTKMM_FOUND)
    add_executable(gui_sandbox_2d 
        gui_sandbox_2d.cpp
        )
       
    target_link_directories(gui_sandbox_2d
        PRIVATE ${GTKMM_LIBRARY_DIRS}  
        )

    target_include_directories(gui_sandbox_2d
        SYSTEM PRIVATE 
            ${GTKMM_INCLUDE_DIRS}
    )
    
    target_link_libraries(gui_sandbox_2d 
        PRIVATE 
            orthtree::orthtree
            ${GTKMM_LIBRARIES}
    )
    
else()
    message(WARNING "GTKMM-4.0 not found, skipping gui_sandbox_2d example")
endif()