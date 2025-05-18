//
// Created by timur on 18/05/2025.
//

#include "pic.h"

bool collision_pic(Personnage* p, BITMAP* fond, float screenx) {
    float offset_x = screenx;
    if (p->timer_pic > 0) // Si bonus actif, on ignore les pics rouge
        return false;
    if ((int)screenx >= fond->w - SCREEN_W) {
        offset_x = fond->w - SCREEN_W;
    }
    for (int dx = 0; dx < p->largeur; dx++) {
        for (int dy = 0; dy < p->hauteur; dy++) {
            int px = (int)(p->x + dx + offset_x);
            int py = p->y + dy;
            if (px >= 0 && px < fond->w && py >= 0 && py < fond->h) {
                int couleur = getpixel(fond, px, py);
                if (couleur == makecol(104, 0, 0)) { // Si détecte pixel rouge
                    return true; // Pic mortel
                }
            }
        }
    }
    return false;
}
// gère la collision entre chaque personnage et les pics (supprime s’il touche)
void gerer_collision_pics_groupe(GrpPersonnages *groupe, BITMAP *fond, float screenx) {
    float offset_x = (int)screenx >= fond->w - SCREEN_W ? fond->w - SCREEN_W : screenx;

    for (int i = 0; i < groupe->nb_personnages; i++) {
        Personnage *p = &(groupe->persos[i]);

        if (p->timer_pic > 0) continue; // immunisé

        for (int dx = 0; dx < p->largeur; dx++) {
            for (int dy = 0; dy < p->hauteur; dy++) {
                int px = (int)(p->x + dx + offset_x);
                int py = p->y + dy;

                if (px >= 0 && px < fond->w && py >= 0 && py < fond->h) {
                    if (getpixel(fond, px, py) == makecol(104, 0, 0)) {
                        // Supprimer le personnage du tableau
                        for (int j = i; j < groupe->nb_personnages - 1; j++) {
                            groupe->persos[j] = groupe->persos[j + 1];
                        }
                        groupe->nb_personnages--; // décrémenter
                        i--; // reculer l'indice car on a décalé les éléments
                        goto next_personnage; // sorti
                    }
                }
            }
        }

        next_personnage:;
    }
}