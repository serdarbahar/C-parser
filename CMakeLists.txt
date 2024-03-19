cmake_minimum_required(VERSION 3.27)
project(Interpreter_Project C)

set(CMAKE_C_STANDARD 23)

add_executable(Interpreter_Project main.c
        Structs.c
        parsing.c
        Evaluators.c
)
