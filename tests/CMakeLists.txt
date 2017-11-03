cmake_minimum_required (VERSION 3.2)

project (tests)

macro (subdir_list result curdir)
    file (GLOB children RELATIVE ${curdir} ${curdir}/*)
    set (dirlist "")
    foreach (child ${children})
        if (IS_DIRECTORY ${curdir}/${child})
            list (APPEND dirlist ${child})
        endif ()
    endforeach ()
    set (${result} ${dirlist})
endmacro ()

subdir_list (SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

foreach (subdir ${SUBDIRS})
  add_subdirectory (${subdir})
endforeach ()