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
        jeu.c
        utilitaire.c
        menu.c
        jeu.h
        "joueur .c"
        joueur.h
        "chekpoint .h"
        checkpoint.c)
target_link_libraries(projetbadland -lmingw32 -lalleg44)