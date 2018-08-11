# Initialize ######################################################################################

Include                     ( cotire OPTIONAL )
Include                     ( pedantic OPTIONAL )

Set                         ( CMAKE_CXX_STANDARD 17 )
Set                         ( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${PEDANTIC_C_FLAGS}" )
Set                         ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PEDANTIC_CXX_FLAGS}" )

# Dependencies ####################################################################################

Find_Package                ( cpputils REQUIRED )

# Project: mariadb ################################################################################

Project                     ( mariadb VERSION 1.0.0.0 LANGUAGES CXX )
File                        ( GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )
Add_Library                 ( mariadb ${SOURCE_FILES} )
Target_Include_Directories  (
                                mariadb
                                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
                            )
Target_Link_Libraries       (
                                mariadb
                                cpputils
                            )
If                          ( __COTIRE_INCLUDED )
    Cotire                      ( mariadb )
EndIf                       ( )