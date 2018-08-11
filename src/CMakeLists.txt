# Initialize ######################################################################################

Include                     ( cotire OPTIONAL )
Include                     ( pedantic OPTIONAL )
Include                     ( strip_symbols OPTIONAL )

Option                      ( BUILD_SHARED_MARIADBPP "Build cppmariadb shared library." OFF )

Set                         ( BUILD_SHARED_LIBS ${BUILD_SHARED_MARIADBPP} )
Set                         ( CMAKE_CXX_STANDARD 17 )
Set                         ( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${PEDANTIC_C_FLAGS}" )
Set                         ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PEDANTIC_CXX_FLAGS}" )

# Dependencies ####################################################################################

Find_Package                ( cpputils REQUIRED )

# Project: cppmariadb #############################################################################

Project                     ( cppmariadb VERSION 1.0.0.0 LANGUAGES CXX )
File                        ( GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )
Add_Library                 ( cppmariadb ${SOURCE_FILES} )
Target_Include_Directories  (
                                cppmariadb
                                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
                            )
Target_Link_Libraries       (
                                cppmariadb
                                cpputils
                            )
If                          ( __COTIRE_INCLUDED )
    Cotire                      ( cppmariadb )
EndIf                       ( )
If                          ( __STRIP_SYMBOLS_INCLUDED )
    Strip_Symbols               ( cppmariadb DBG_FILE )
EndIf                       ()