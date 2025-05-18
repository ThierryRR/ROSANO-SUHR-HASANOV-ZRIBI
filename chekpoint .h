//
// Created by thier on 18/05/2025.
//

#ifndef CHEKPOINT_H
#define CHEKPOINT_H
#include <allegro.h>
#include "jeuniv1.h"
typedef struct {
    int x, y; // position du checkpoint
    int largeur, hauteur; // dimensions
    BITMAP *sprite[2]; // sprite[0] = drapeau rouge, sprite[1] = drapeau vert
    int active; // 0 = pas activé, 1 = activé
} checkpoint;

checkpoint creer_checkpoint(int x, int y, const char* path_rouge, const char* path_vert); // création checkpoint
void afficher_checkpoint(BITMAP *buffer, checkpoint cp, int screenx); // affiche checkpoint
int collision_checkpoint(checkpoint *cp, GrpPersonnages *groupe, int *reprendre_x, int *reprendre_y, float screenx); // collision avec checkpoint
#endif //CHEKPOINT_H
