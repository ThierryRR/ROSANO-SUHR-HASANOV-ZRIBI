#include "chekpoint .h"

checkpoint creer_checkpoint(int x, int y, const char* path_rouge, const char* path_vert) {
    checkpoint cp;
    cp.x = x;
    cp.y = y;
    cp.sprite[0] = load_bitmap(path_rouge, NULL); // sprite rouge
    cp.sprite[1] = load_bitmap(path_vert, NULL);  // sprite vert
    cp.active = 0;

    if (cp.sprite[0]) {
        cp.largeur = cp.sprite[0]->w / 12;
        cp.hauteur = cp.sprite[0]->h / 12;
    } else {
        cp.largeur = 20;
        cp.hauteur = 40;
    }
    return cp;
}

// affiche le checkpoint selon son etat (rouge ou vert)
void afficher_checkpoint(BITMAP *buffer, checkpoint cp, int screenx) {
    int frame = cp.active ? 1 : 0; // 0 = inactif, 1 = actif
    if (cp.sprite[frame]) {
        int largeur = cp.sprite[frame]->w / 12;
        int hauteur = cp.sprite[frame]->h / 12;
        stretch_sprite(buffer, cp.sprite[frame], cp.x - screenx, cp.y, largeur, hauteur);
    }
}

// verifie si un personnage touche le checkpoint
int collision_checkpoint(checkpoint *cp, GrpPersonnages *groupe, int *reprendre_x, int *reprendre_y, float screenx) {
    if (cp->active) return 0; // deja actif, pas besoin de verifier

    int marge = 20;
    int zone_x = cp->x - marge / 2;
    int zone_y = cp->y - marge / 2;
    int zone_largeur = cp->largeur + marge;
    int zone_hauteur = cp->hauteur + marge;

    for (int i = 0; i < groupe->nb_personnages; i++) {
        Personnage *p = &groupe->persos[i];

        int perso_x_monde = p->x + (int)screenx;
        int perso_y_monde = p->y;

        if (perso_x_monde + p->largeur > zone_x &&
            perso_x_monde < zone_x + zone_largeur &&
            perso_y_monde + p->hauteur > zone_y &&
            perso_y_monde < zone_y + zone_hauteur) {

            cp->active = 1; // on active le checkpoint
            *reprendre_x = cp->x;
            *reprendre_y = cp->y;

            return 1; // collision valide
        }
    }

    return 0; // aucun perso n'a touche le checkpoint
}
//
// Created by thier on 18/05/2025.
//
