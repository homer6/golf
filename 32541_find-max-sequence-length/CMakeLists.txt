cmake_minimum_required( VERSION "2.8.0" )
set( MAIN_EXECUTABLE program )

project( ${MAIN_EXECUTABLE} CXX C )

add_executable( count count_main.cc )
add_executable( mkrandom mkrandom_main.cc )

if( CMAKE_COMPILER_IS_GNUCXX )
    message( STATUS "GCC detected, adding compile flags" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -std=gnu++0x -g -pg" )
    #set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -std=gnu++0x" )
endif( CMAKE_COMPILER_IS_GNUCXX )

