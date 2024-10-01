#version minimale de cmake nécessaire
cmake_minimum_required(VERSION 3.6)

enable_testing()
#Nom du projet
project (Mosaic_A15 LANGUAGES C)
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
include(CTest)

#positionne la variable
set(CMAKE_C_FLAGS "-std=c99 -Wall")
set(CMAKE_C_FLAGS_DEBUG "-g -O0 --coverage")
set(CMAKE_C_FLAGS_RELEASE "-O3")

link_directories(${PROJECT_SOURCE_DIR})

# Find SDL2
include(sdl2.cmake)
message(STATUS "SDL2 include dir: ${SDL2_ALL_INC}")
message(STATUS "SDL2 all libraries: ${SDL2_ALL_LIBS}")
set(SDL2_ALL_INC ${SDL2_INCLUDE_DIR})
set(SDL2_ALL_LIBS ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY})
include_directories(${SDL2_ALL_INC})


#Creation des éxecutables:
include_directories(${SDL2_ALL_INC})
add_executable(game_text ${PROJECT_SOURCE_DIR}/game_text.c)
add_executable(game_test_olatestere ${PROJECT_SOURCE_DIR}/game_test_olatestere.c)
add_executable(game_test_yhannachi ${PROJECT_SOURCE_DIR}/game_test_yhannachi.c)
add_executable(game_test_maitissad ${PROJECT_SOURCE_DIR}/game_test_maitissad.c)
add_executable(game_solve ${PROJECT_SOURCE_DIR}/game_solve.c)
add_executable(game_sdl ${PROJECT_SOURCE_DIR}/game_sdl.c ${PROJECT_SOURCE_DIR}/model.c ${PROJECT_SOURCE_DIR}/button.c)
add_executable(model ${PROJECT_SOURCE_DIR}/game_sdl.c ${PROJECT_SOURCE_DIR}/model.c ${PROJECT_SOURCE_DIR}/button.c)



#Creation de libgame
add_library(game ${PROJECT_SOURCE_DIR}/game.c ${PROJECT_SOURCE_DIR}/game_aux.c ${PROJECT_SOURCE_DIR}/game_ext.c ${PROJECT_SOURCE_DIR}/queue.c ${PROJECT_SOURCE_DIR}/game_tools.c)

#Liaison des executables avec libgame
target_link_libraries(game_sdl ${SDL2_ALL_LIBS} game m)
target_link_libraries(model ${SDL2_ALL_LIBS} game m)
target_link_libraries(game_text game)
target_link_libraries(game_test_olatestere game)
target_link_libraries(game_test_yhannachi game)
target_link_libraries(game_test_maitissad game)
target_link_libraries(game_solve game)
target_link_libraries(game_text m)
target_link_libraries(game_test_maitissad m)
target_link_libraries(game_test_yhannachi m)
target_link_libraries(game_test_olatestere m)
target_link_libraries(game_solve m)

#Tests d'Olivier:
add_test(test_olatestere_dummy ./game_test_olatestere dummy)
add_test(test_olatestere_new_empty ./game_test_olatestere new_empty)
add_test(test_olatestere_empty ./game_test_olatestere new)
add_test(test_olatestere_copy ./game_test_olatestere copy)
add_test(test_olatestere_print ./game_test_olatestere print)
add_test(test_olatestere_default ./game_test_olatestere default)
add_test(test_olatestere_default_solution ./game_test_olatestere default_solution)
add_test(test_olatestere_new_empty_ext ./game_test_olatestere new_empty_ext)
add_test(test_olatestere_new_ext ./game_test_olatestere new_ext)
add_test(test_olatestere_undo ./game_test_olatestere undo)
add_test(test_olatestere_redo ./game_test_olatestere redo)
add_test(test_olatestere_load ./game_test_olatestere load)

#Tests de Yazid:
add_test(test_yhannachi_dummy ./game_test_yhannachi dummy)
add_test(test_yhannachi_game_equal ./game_test_yhannachi game_equal)
add_test(test_yhannachi_game_delete ./game_test_yhannachi game_delete)
add_test(test_yhannachi_game_set_color ./game_test_yhannachi game_set_color)
add_test(test_yhannachi_game_set_constraint ./game_test_yhannachi game_set_constraint)
add_test(test_yhannachi_game_get_color ./game_test_yhannachi game_get_color)
add_test(test_yhannachi_game_get_constraint ./game_test_yhannachi game_get_constraint)
add_test(test_yhannachi_game_save ./game_test_yhannachi game_save)
#Tests de Mouh:
add_test(test_maitissad_dummy ./game_test_maitissad dummy)
add_test(test_maitissad_game_restart ./game_test_maitissad game_restart)
add_test(test_maitissad_game_play_move ./game_test_maitissad game_play_move)
add_test(test_maitissad_game_won ./game_test_maitissad game_won)
add_test(test_maitissad_game_nb_neighbors ./game_test_maitissad game_nb_neighbors)
add_test(test_maitissad_game_get_status ./game_test_maitissad game_get_status)
add_test(test_maitissad_game_get_next_square ./game_test_maitissad game_get_next_square)
add_test(test_maitissad_game_is_wrapping ./game_test_maitissad game_is_wrapping)
add_test(test_maitissad_game_nb_cols ./game_test_maitissad game_nb_cols)
add_test(test_maitissad_game_nb_rows ./game_test_maitissad game_nb_rows)
add_test(test_maitissad_game_get_neighbourhood ./game_test_maitissad game_get_neighbourhood)

file(COPY res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})