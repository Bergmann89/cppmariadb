# Initialize ######################################################################################

Include                     ( cotire OPTIONAL )
Include                     ( pedantic OPTIONAL )
Include                     ( cmake_tests OPTIONAL )

Set                         ( CMAKE_CXX_STANDARD 17 )
Set                         ( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${PEDANTIC_C_FLAGS}" )
Set                         ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PEDANTIC_CXX_FLAGS}" )

# Project: test_mariadbpp #########################################################################

Project                     ( test_mariadbpp )
File                        ( GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )
Add_Executable              ( test_mariadbpp EXCLUDE_FROM_ALL ${SOURCE_FILES} )
Target_Link_Libraries       (
                                test_mariadbpp
                                cppmariadb
                                cpputils
                                gtest
                                gmock
                                gmock_main
                                pthread
                            )
If                          ( __COTIRE_INCLUDED )
    Cotire                      ( test_mariadbpp )
EndIf                       ( )
If                          ( __CMAKE_TESTS_INCLUDED )
    Add_CMake_Test             ( tsoutils test_mariadbpp )
EndIf                       ( )
