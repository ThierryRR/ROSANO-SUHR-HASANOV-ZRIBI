//
// Created by thier on 21/04/2025.
//

#ifndef JEU_H
#define JEU_H
#include "menu.h"
#include <allegro.h>

#define NB_BONUS 5
void gerer_acceleration(float *dragon_speed, int *dragon_acceleration_timer, int *dragon_malus_timer ,int space);
void jeu_niveau_1(BITMAP *fond_final, Joueur *j);
void jeu_niveau_2(BITMAP *fond_final, Joueur *j);//eeeeeee
#endif //JEU_H
