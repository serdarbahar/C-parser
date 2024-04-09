cmake_minimum_required(VERSION 3.27)
project(Interpreter_Project C)

set(CMAKE_C_STANDARD 23)

add_executable(Interpreter_Project src/main.c
        src/Structs.c
        src/parsing.c
        src/Evaluators.c
        src/Evaluators.h
        src/parsing.h
        src/main.c
        src/main.c
)
