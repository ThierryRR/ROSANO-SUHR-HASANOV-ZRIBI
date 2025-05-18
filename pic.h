//
// Created by timur on 18/05/2025.
//


#ifndef PIC_H
#define PIC_H
#include <stdbool.h>
#include"personnage.h"
bool collision_pic(Personnage* p, BITMAP* fond, float screenx);// fonction qui teste si un personnage touche un pic
void gerer_collision_pics_groupe(GrpPersonnages *groupe, BITMAP *fond, float screenx);// applique les collisions entre les personnages et les pics du décor
#endif //PIC_H
