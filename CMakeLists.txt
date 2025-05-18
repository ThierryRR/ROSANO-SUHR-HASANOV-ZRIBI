cmake_minimum_required(VERSION 3.29)
project(projetbadland C)

set(CMAKE_C_STANDARD 11)

add_executable(projetbadland  main.c
        scroll.c
        menu.c
        menu.h
        scroll.c
        personnage.c
        scroll.c
        bonus.c
        jeuniv1.c
        utilitaire.c
        menu.c
        jeuniv1.h
        "joueur .c"
        joueur.h
        "chekpoint .h"
        checkpoint.c
        pic.c
        pic.h
        jeuniv2.c
        jeuniv2.h
        jeuniv3.c
        jeuniv3.h)
target_link_libraries(projetbadland -lmingw32 -lalleg44)