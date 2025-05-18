#include "bonus.h"
#include <math.h>
#include <stdio.h>//
#include "jeu.h"
#include "bonus.h"
#include <allegro.h>
#include <stdio.h>

BonusPosition creer_bonus(int x, int y, BITMAP *sprite, BITMAP *explosion) {
    BonusPosition bonus;
    bonus.x = x;
    bonus.y = y;
    bonus.largeur = sprite->w / 4;
    bonus.hauteur = sprite->h / 4;
    bonus.sprite = sprite;
    bonus.sprite_explosion[0] = sprite;
    bonus.sprite_explosion[1] = explosion;
    bonus.explosion_timer = 0;
    bonus.actif = 1;
    bonus.en_chute=0;
    return bonus;
}

void afficher_bonus(BonusPosition bonus, BITMAP *buffer, float screenx) {
    if (!bonus.actif && bonus.explosion_timer <= 0) return;

    int ecran_x = bonus.x - (int)screenx;
    int largeur, hauteur;
    BITMAP *img = NULL;

    if (bonus.explosion_timer > 0 && bonus.sprite_explosion[1]) {
        img = bonus.sprite_explosion[1];  // Explosion
        largeur = bonus.largeur * 2;
        hauteur = bonus.hauteur * 2;
        ecran_x -= (largeur - bonus.largeur) / 2;
    } else if (bonus.actif && bonus.sprite) {
        img = bonus.sprite;  // Bombe normale
        largeur = bonus.largeur;
        hauteur = bonus.hauteur;
    } else {
        return;
    }

    if (img) {
        stretch_sprite(buffer, img, ecran_x, bonus.y, largeur, hauteur);
    }
}

int collision_bonus(BonusPosition *bonus, int perso_x, int perso_y, int perso_w, int perso_h, float screenx) {
    if (!bonus->actif) return 0;

    int marge = bonus->largeur / 5;  // 20% de la largeur en marge
    if (marge < 5) marge = 5;        // minimum 5px pour éviter de trop réduire

    int bonus_ecran_x = bonus->x - (int)screenx;

    int zone_x = bonus_ecran_x + marge;
    int zone_y = bonus->y + marge;
    int zone_largeur = bonus->largeur - 2 * marge;
    int zone_hauteur = bonus->hauteur - 2 * marge;

    if (zone_largeur <= 0 || zone_hauteur <= 0) return 0;

    if (perso_x + perso_w > zone_x &&
        perso_x < zone_x + zone_largeur &&
        perso_y + perso_h > zone_y &&
        perso_y < zone_y + zone_hauteur) {
        bonus->actif = 0;
        bonus->explosion_timer = 60; // 1 seconde
        return 1;
        }

    return 0;
}


checkpoint creer_checkpoint(int x, int y, const char* path_rouge, const char* path_vert) {
    checkpoint cp;
    cp.x = x;
    cp.y = y;
    cp.sprite[0] = load_bitmap(path_rouge, NULL);
    cp.sprite[1] = load_bitmap(path_vert, NULL);
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

void afficher_checkpoint(BITMAP *buffer, checkpoint cp, int screenx) {
    int frame = cp.active ? 1 : 0;
    if (cp.sprite[frame]) {
        int largeur = cp.sprite[frame]->w / 12;
        int hauteur = cp.sprite[frame]->h / 12;
        stretch_sprite(buffer, cp.sprite[frame], cp.x - screenx, cp.y, largeur, hauteur);
    }
}
int collision_checkpoint(checkpoint *cp, GrpPersonnages *groupe, int *reprendre_x, int *reprendre_y, float screenx) {
    if (cp->active) return 0;

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

            cp->active = 1;
            *reprendre_x = cp->x;
            *reprendre_y = cp->y;

            return 1;
            }
    }

    return 0;
}



void gerer_malus_clones(BonusPosition mon_bonus2[], GrpPersonnages *groupe, int screenx) {
    for (int b = 0; b < NB_BONUS; b++) {
        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&mon_bonus2[b], groupe->persos[i].x, groupe->persos[i].y, groupe->persos[i].largeur, groupe->persos[i].hauteur, screenx)) {
                int n = (rand() % 4) + 1;
                if (n > groupe->nb_personnages) n = groupe->nb_personnages;
                for (int j = 0; j < n; j++) {
                    int idx = rand() % groupe->nb_personnages;
                    groupe->persos[idx] = groupe->persos[groupe->nb_personnages - 1];
                    groupe->nb_personnages--;
                }
                mon_bonus2[b].explosion_timer = 180;
                break;
            }
        }
    }

    for (int b = 0; b < NB_BONUS; b++) {
        if (mon_bonus2[b].explosion_timer > 0) mon_bonus2[b].explosion_timer--;
    }
}void gerer_bonus_clones(BonusPosition mon_bonus1[], GrpPersonnages *groupe, int screenx, int *timer_clones) {
    for (int b = 0; b < NB_BONUS; b++) {
        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&mon_bonus1[b], groupe->persos[i].x, groupe->persos[i].y,
                                groupe->persos[i].largeur, groupe->persos[i].hauteur, screenx)) {
                int clones = (rand() % 5) + 1;
                Personnage *original = &groupe->persos[i];
                for (int j = 0; j < clones && groupe->nb_personnages < MAX_PERSONNAGES; j++) {
                    int dx = 20 * j;
                    creation_personnage(&groupe->persos[groupe->nb_personnages++],
                                        original->x + dx, original->y, original->largeur, original->hauteur);
                }
                *timer_clones = 60;
                break; // un seul déclenchement par bonus
                                }
        }
    }
}





void gerer_bonus_saut(BonusPosition mon_bonus3[], GrpPersonnages *groupe, int screenx, int *dragon_acceleration_timer) {
    for (int b = 0; b < NB_BONUS; b++) {
        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&mon_bonus3[b], groupe->persos[i].x, groupe->persos[i].y, groupe->persos[i].largeur, groupe->persos[i].hauteur, screenx)) {
                for (int j = 0; j < groupe->nb_personnages; j++) {
                    *dragon_acceleration_timer = 180;
                }

                break;
            }
        }
    }
}
void gerer_malus_vitesse(BonusPosition mon_bonus4[], GrpPersonnages *groupe, int screenx, int *dragon_malus_timer) {
    for (int b = 0; b < NB_BONUS; b++) {
        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&mon_bonus4[b], groupe->persos[i].x, groupe->persos[i].y, groupe->persos[i].largeur, groupe->persos[i].hauteur, screenx)) {
                for (int j = 0; j < groupe->nb_personnages; j++) {
                    *dragon_malus_timer = 180;
                }

                break;
            }
        }
    }
}

void gerer_taille_petit(BonusPosition malust[], GrpPersonnages*groupe, int screenx, int *timer_malus_taille) {
    for (int b = 0; b < NB_BONUS; b++) {
        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&malust[b], groupe->persos[i].x, groupe->persos[i].y, groupe->persos[i].largeur, groupe->persos[i].hauteur, screenx)) {
                for (int j = 0; j < groupe->nb_personnages; j++) {
                    groupe->persos[j].largeur /= 2;
                    groupe->persos[j].hauteur /= 2;
                }
                *timer_malus_taille = 300;
                break;
            }
        }
    }

    if (*timer_malus_taille > 0) {
        (*timer_malus_taille)--;
        if (*timer_malus_taille == 0) {
            for (int j = 0; j < groupe->nb_personnages; j++) {
                groupe->persos[j].largeur *= 2;
                groupe->persos[j].hauteur *= 2;
            }
        }
    }
}

void gerer_taille_grand (BonusPosition bonust[], GrpPersonnages *groupe, int screenx, int *timer_bonus_taille) {
    for (int b = 0; b < NB_BONUS; b++) {
        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&bonust[b], groupe->persos[i].x, groupe->persos[i].y,
                                groupe->persos[i].largeur, groupe->persos[i].hauteur, screenx)) {
                for (int j = 0; j < groupe->nb_personnages; j++) {
                    groupe->persos[j].largeur *= 2;
                    groupe->persos[j].hauteur *= 2;
                }
                *timer_bonus_taille = 300;
                break;
                                }
        }
    }

    // Réduction après le délai
    if (*timer_bonus_taille > 0) {
        (*timer_bonus_taille)--;
        if (*timer_bonus_taille == 0) {
            for (int j = 0; j < groupe->nb_personnages; j++) {
                groupe->persos[j].largeur /= 2;
                groupe->persos[j].hauteur /= 2;
            }
        }
    }
}




void afficher_bonus_explosion(BonusPosition bonus, BITMAP *buffer, float screenx) {
    int largeur = bonus.largeur;
    int hauteur = bonus.hauteur;
    int x_affiche = bonus.x - (int)screenx;
    int y_affiche = bonus.y;

    if (bonus.explosion_timer > 0 && bonus.sprite_explosion[1]) {
        // Afficher l'explosion pendant 1 seconde (60 frames)
        stretch_sprite(buffer, bonus.sprite_explosion[1], x_affiche, y_affiche, largeur, hauteur);
    } else if (bonus.sprite) {
        // Affichage par défaut
        stretch_sprite(buffer, bonus.sprite, x_affiche, y_affiche, largeur, hauteur);
    }
}



void gerer_bonus_immunite_pic(BonusPosition mon_bonus5[], GrpPersonnages *groupe, int screenx) {
    for (int b = 0; b < NB_BONUS; b++) {
        if (!mon_bonus5[b].actif) continue;

        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&mon_bonus5[b],
                                groupe->persos[i].x, groupe->persos[i].y,
                                groupe->persos[i].largeur, groupe->persos[i].hauteur,
                                screenx)) {

                for (int j = 0; j < groupe->nb_personnages; j++) {
                    groupe->persos[j].timer_pic = 300;
                }
                break;
                                }
        }
    }

    // Décrément timer
    for (int i = 0; i < groupe->nb_personnages; i++) {
        if (groupe->persos[i].timer_pic > 0)
            groupe->persos[i].timer_pic--;
    }
}

void deplacement_position_bonus(BonusPosition *bonus, float *vitesse_y, float gravite_max, float acceleration, BITMAP *fond, float screenx) {
    if (!bonus->actif) return;

    // Appliquer une accélération (gravité douce)
    *vitesse_y += acceleration;
    if (*vitesse_y > gravite_max) *vitesse_y = gravite_max;

    int tentative_y = bonus->y + (int)(*vitesse_y);
    int bonus_x_ecran = bonus->x - (int)screenx;

    // Vérifie les pixels sous le bonus (ligne horizontale)
    int collision = 0;
    for (int dx = 0; dx < bonus->largeur; dx++) {
        int px = bonus->x + dx;
        int py = tentative_y + bonus->hauteur;
        if (px >= 0 && px < fond->w && py >= 0 && py < fond->h) {
            int couleur = getpixel(fond, px, py);
            if (getr(couleur) == 0 && getg(couleur) == 0 && getb(couleur) == 0) {
                collision = 1;
                break;
            }
        }
    }

    // Si pas de collision, on déplace ; sinon on stoppe
    if (!collision) {
        bonus->y = tentative_y;
    } else {
        *vitesse_y = 0; // Arrêt vertical
    }
}
void deplacement_pic(BonusPosition *pic, float *vitesse_y, float gravite_max, float acceleration, BITMAP *fond, float screenx, GrpPersonnages *groupe) {
    if (!pic->actif) return;

    // Déclenchement de la chute si perso est proche en X et à droite du pic
    if (!pic->en_chute) {
        for (int i = 0; i < groupe->nb_personnages; i++) {
            int perso_x_monde = groupe->persos[i].x + (int)screenx;
            int distance_x = abs(pic->x - perso_x_monde);

            if (distance_x <= 300 && perso_x_monde > pic->x) {
                pic->en_chute = 1;  // Le pic tombe
                break;
            }
        }

        if (!pic->en_chute) return;  // Si toujours pas déclenché, on quitte
    }

    // Gravité classique
    *vitesse_y += acceleration;
    if (*vitesse_y > gravite_max) *vitesse_y = gravite_max;

    int tentative_y = pic->y + (int)(*vitesse_y);

    // Collision sol
    int collision = 0;
    for (int dx = 0; dx < pic->largeur; dx++) {
        for (int dy = 0; dy <= 1; dy++) {
            int px = pic->x + dx;
            int py = tentative_y + pic->hauteur + dy;
            if (px >= 0 && px < fond->w && py >= 0 && py < fond->h) {
                int couleur = getpixel(fond, px, py);
                if (getr(couleur) == 0 && getg(couleur) == 0 && getb(couleur) == 0) {
                    collision = 1;
                    break;
                }
            }
        }
        if (collision) break;
    }

    // Appliquer déplacement si pas de collision
    if (!collision) {
        pic->y = tentative_y;
    } else {
        *vitesse_y = 0;  // Stop au sol
    }
}




void gerer_bonus_colle(BonusPosition bonus[], GrpPersonnages *groupe, int screenx) {
    static int ancienne_touche_space = 0;

    for (int b = 0; b < NB_BONUS; b++) {
        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&bonus[b],
                                groupe->persos[i].x, groupe->persos[i].y,
                                groupe->persos[i].largeur, groupe->persos[i].hauteur,
                                screenx)) {
                groupe->persos[i].timer_colle = 300;  // effet colle 5s
                groupe->persos[i].x_colle = groupe->persos[i].x;  // position figée
                groupe->persos[i].compteur_espace_colle = 0;      // reset compteur
                bonus[b].actif = 0;
                break;
                                }
        }
    }

    int touche_space = key[KEY_SPACE];

    for (int i = 0; i < groupe->nb_personnages; i++) {
        Personnage *p = &groupe->persos[i];

        if (p->timer_colle > 0) {
            // Détection front montant (appui unique sur Espace)
            if (touche_space && !ancienne_touche_space) {
                p->compteur_espace_colle++;
                if (p->compteur_espace_colle >= 6) {
                    p->timer_colle = 0;  // effet colle désactivé
                    p->compteur_espace_colle = 0;
                }
            }

            // Timer de colle qui descend naturellement
            p->timer_colle--;
        }
    }

    ancienne_touche_space = touche_space;
}
int collision_bonus_pic(BonusPosition *bonus, int perso_x, int perso_y, int perso_w, int perso_h, float screenx) {
    if (!bonus->actif) return 0;

    int marge = bonus->largeur / 5;  // 20% de la largeur en marge
    if (marge < 5) marge = 5;        // minimum 5px pour éviter de trop réduire

    int bonus_ecran_x = bonus->x - (int)screenx;

    int zone_x = bonus_ecran_x + marge;
    int zone_y = bonus->y + marge;
    int zone_largeur = bonus->largeur - 2 * marge;
    int zone_hauteur = bonus->hauteur - 2 * marge;

    if (zone_largeur <= 0 || zone_hauteur <= 0) return 0;

    if (perso_x + perso_w > zone_x &&
        perso_x < zone_x + zone_largeur &&
        perso_y + perso_h > zone_y &&
        perso_y < zone_y + zone_hauteur) {
        bonus->actif = 0;
        bonus->explosion_timer = 60; // 1 seconde
        return 1;
        }

    return 0;
}

void gerer_collision_pics_dynamiques(GrpPersonnages *groupe, BonusPosition pics[], float decallage_scroll) {
    for (int j = 0; j < NB_PICS; j++) {
        if (!pics[j].actif) continue;
        for (int i = 0; i < groupe->nb_personnages; i++) {
            Personnage *p = &groupe->persos[i];
            // Ignorer les immunisés
            if (p->timer_pic > 0) continue;
            // Test de collision par le haut avec un pic
            if (collision_bonus_pic(&pics[j], p->x, p->y, p->largeur, p->hauteur, decallage_scroll)) {
                // Supprimer le personnage du groupe
                for (int k = i; k < groupe->nb_personnages - 1; k++) {
                    groupe->persos[k] = groupe->persos[k + 1];
                }
                groupe->nb_personnages--;
                i--; // Ajuster car on a décalé les indices
                break;
            }
        }
    }
}
