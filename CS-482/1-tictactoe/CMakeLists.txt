cmake_minimum_required (VERSION 2.6)
project (1-tictactoe)

include_directories("${PROJECT_SOURCE_DIR}/include")

add_executable(tictactoe src/tictac.cc src/tictac_support.cc src/tictac_turnin.cc)

#add_executable(grader src/tictac_grader.cc src/tictac_support.cc src/tictac_answer.cc)
