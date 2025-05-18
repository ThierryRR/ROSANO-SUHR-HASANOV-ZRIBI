#include "bonus.h"
#include <math.h>
#include <stdio.h>//
#include "jeuniv1.h"
#include "bonus.h"
#include <allegro.h>
#include <stdio.h>


BonusPosition creer_bonus(int x, int y, BITMAP *sprite, BITMAP *explosion) {
    BonusPosition bonus;
    bonus.x = x; // position x
    bonus.y = y; // position y
    bonus.largeur = sprite->w / 4; // largeur du bonus
    bonus.hauteur = sprite->h / 4; // hauteur du bonus
    bonus.sprite = sprite; // image
    bonus.sprite_explosion[0] = sprite; // image normale
    bonus.sprite_explosion[1] = explosion; // image d'explosion
    bonus.explosion_timer = 0; // timer d'explosion
    bonus.actif = 1; // actif au départ
    bonus.en_chute = 0; // chute désactivée
    return bonus;
}

void afficher_bonus(BonusPosition bonus, BITMAP *buffer, float screenx) {
    if (!bonus.actif && bonus.explosion_timer <= 0) return; // on n'affiche rien

    int ecran_x = bonus.x - (int)screenx; // position à l'écran
    int largeur, hauteur;
    BITMAP *img = NULL;

    if (bonus.explosion_timer > 0 && bonus.sprite_explosion[1]) {
        img = bonus.sprite_explosion[1]; // image explosion
        largeur = bonus.largeur * 2; // deux fois plus grand
        hauteur = bonus.hauteur * 2;
        ecran_x -= (largeur - bonus.largeur) / 2;
    } else if (bonus.actif && bonus.sprite) {
        img = bonus.sprite; // image normale
        largeur = bonus.largeur;
        hauteur = bonus.hauteur;
    } else {
        return;
    }

    if (img) {
        stretch_sprite(buffer, img, ecran_x, bonus.y, largeur, hauteur); // affichage
    }
}


// detecte la collision entre le personnage et un bonus actif
int collision_bonus(BonusPosition *bonus, int perso_x, int perso_y, int perso_w, int perso_h, float screenx) {
    if (!bonus->actif) return 0; // si inactif on sort directement

    int marge = bonus->largeur / 5; // on réduit la zone de collision
    if (marge < 5) marge = 5;

    int bonus_ecran_x = bonus->x - (int)screenx;

    int zone_x = bonus_ecran_x + marge;
    int zone_y = bonus->y + marge;
    int zone_largeur = bonus->largeur - 2 * marge;
    int zone_hauteur = bonus->hauteur - 2 * marge;

    if (zone_largeur <= 0 || zone_hauteur <= 0) return 0; // zone trop petite

    if (perso_x + perso_w > zone_x &&
        perso_x < zone_x + zone_largeur &&
        perso_y + perso_h > zone_y &&
        perso_y < zone_y + zone_hauteur) {
        bonus->actif = 0; // desactive le bonus
        bonus->explosion_timer = 60; // timer explosion
        return 1;
    }
    return 0;
}


// supprime un nombre aléatoire de clones lorsqu'on touche un malus
void gerer_malus_clones(BonusPosition mon_bonus2[], GrpPersonnages *groupe, int screenx) {
    for (int b = 0; b < NB_BONUS; b++) {
        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&mon_bonus2[b], groupe->persos[i].x, groupe->persos[i].y, groupe->persos[i].largeur, groupe->persos[i].hauteur, screenx)) {
                int n = (rand() % 4) + 1; // nombre de clones à supprimer (entre 1 et 4)
                if (n > groupe->nb_personnages) n = groupe->nb_personnages;
                for (int j = 0; j < n; j++) {
                    int idx = rand() % groupe->nb_personnages; // sélectionne un perso aléatoire
                    groupe->persos[idx] = groupe->persos[groupe->nb_personnages - 1];
                    groupe->nb_personnages--; // décrémente le nombre de personnages
                }
                mon_bonus2[b].explosion_timer = 180; // explosion pendant 3s
                break; // on sort de la boucle une fois le malus déclenché
            }
        }
    }
    // décrémente le timer d'explosion des bonus
    for (int b = 0; b < NB_BONUS; b++) {
        if (mon_bonus2[b].explosion_timer > 0) mon_bonus2[b].explosion_timer--;
    }
}

// crée plusieurs clones du personnage qui touche le bonus
void gerer_bonus_clones(BonusPosition mon_bonus1[], GrpPersonnages *groupe, int screenx, int *timer_clones) {
    for (int b = 0; b < NB_BONUS; b++) {
        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&mon_bonus1[b], groupe->persos[i].x, groupe->persos[i].y,
                                groupe->persos[i].largeur, groupe->persos[i].hauteur, screenx)) {
                int clones = (rand() % 5) + 1; // nombre de clones à générer (1 à 5)
                Personnage *original = &groupe->persos[i];
                for (int j = 0; j < clones && groupe->nb_personnages < MAX_PERSONNAGES; j++) {
                    int dx = 20 * j; // décale horizontalement les clones
                    creation_personnage(&groupe->persos[groupe->nb_personnages++],
                                        original->x + dx, original->y, original->largeur, original->hauteur);
                }
                *timer_clones = 60; // durée visuelle ou logique éventuelle
                break; // un seul bonus par boucle
            }
        }
    }
}

// active un boost de vitesse temporaire lorsque le bonus est ramassé
void gerer_bonus_saut(BonusPosition mon_bonus3[], GrpPersonnages *groupe, int screenx, int *dragon_acceleration_timer) {
    for (int b = 0; b < NB_BONUS; b++) {
        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&mon_bonus3[b], groupe->persos[i].x, groupe->persos[i].y, groupe->persos[i].largeur, groupe->persos[i].hauteur, screenx)) {
                for (int j = 0; j < groupe->nb_personnages; j++) {
                    *dragon_acceleration_timer = 180; // accélération pendant 3 secondes
                }
                break;
            }
        }
    }
}

// applique un malus de scroll ralenti
void gerer_malus_vitesse(BonusPosition mon_bonus4[], GrpPersonnages *groupe, int screenx, int *dragon_malus_timer) {
    for (int b = 0; b < NB_BONUS; b++) {
        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&mon_bonus4[b], groupe->persos[i].x, groupe->persos[i].y, groupe->persos[i].largeur, groupe->persos[i].hauteur, screenx)) {
                for (int j = 0; j < groupe->nb_personnages; j++) {
                    *dragon_malus_timer = 180; // scroll ralenti pendant 3 secondes
                }
                break;
            }
        }
    }
}

// réduit la taille des personnages
void gerer_taille_petit(BonusPosition malust[], GrpPersonnages*groupe, int screenx, int *timer_malus_taille) {
    for (int b = 0; b < NB_BONUS; b++) {
        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&malust[b], groupe->persos[i].x, groupe->persos[i].y, groupe->persos[i].largeur, groupe->persos[i].hauteur, screenx)) {
                for (int j = 0; j < groupe->nb_personnages; j++) {
                    groupe->persos[j].largeur /= 2; // divise largeur par 2
                    groupe->persos[j].hauteur /= 2; // divise hauteur par 2
                }
                *timer_malus_taille = 300; // dure 5 secondes
                break;
            }
        }
    }
    // une fois le temps écoulé on remet la taille d'origine
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

// augmente temporairement la taille des personnages
void gerer_taille_grand (BonusPosition bonust[], GrpPersonnages *groupe, int screenx, int *timer_bonus_taille) {
    for (int b = 0; b < NB_BONUS; b++) {
        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&bonust[b], groupe->persos[i].x, groupe->persos[i].y,
                                groupe->persos[i].largeur, groupe->persos[i].hauteur, screenx)) {
                for (int j = 0; j < groupe->nb_personnages; j++) {
                    groupe->persos[j].largeur *= 2; // double la largeur
                    groupe->persos[j].hauteur *= 2; // double la hauteur
                }
                *timer_bonus_taille = 300; // dure 5 secondes
                break;
            }
        }
    }
    // retour à la taille normale après le délai
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
    //initialisation de la taille
    int largeur = bonus.largeur;
    int hauteur = bonus.hauteur;
    int x_affiche = bonus.x - (int)screenx;
    int y_affiche = bonus.y;

    if (bonus.explosion_timer > 0 && bonus.sprite_explosion[1]) {
        stretch_sprite(buffer, bonus.sprite_explosion[1], x_affiche, y_affiche, largeur, hauteur); // explosion
    } else if (bonus.sprite) {
        stretch_sprite(buffer, bonus.sprite, x_affiche, y_affiche, largeur, hauteur); // image normale
    }
}


void gerer_bonus_immunite_pic(BonusPosition mon_bonus5[], GrpPersonnages *groupe, int screenx) {
    for (int b = 0; b < NB_BONUS; b++) {
        if (!mon_bonus5[b].actif) continue; // ignore si le bonus est déjà utilisé

        for (int i = 0; i < groupe->nb_personnages; i++) {
            // vérifie la collision entre un personnage et le bonus
            if (collision_bonus(&mon_bonus5[b],
                                groupe->persos[i].x, groupe->persos[i].y,
                                groupe->persos[i].largeur, groupe->persos[i].hauteur,
                                screenx)) {

                for (int j = 0; j < groupe->nb_personnages; j++) {
                    groupe->persos[j].timer_pic = 300; // donne 5 secondes d'immunité aux pics
                }
                break; // on sort de la boucle
                                }
        }
    }

    // décrémente le timer d'immunité pour chaque personnage
    for (int i = 0; i < groupe->nb_personnages; i++) {
        if (groupe->persos[i].timer_pic > 0)
            groupe->persos[i].timer_pic--;
    }
}

void deplacement_position_bonus(BonusPosition *bonus, float *vitesse_y, float gravite_max, float acceleration, BITMAP *fond, float screenx) {
    if (!bonus->actif) return;

    // application dune accélération
    *vitesse_y += acceleration;
    if (*vitesse_y > gravite_max) *vitesse_y = gravite_max;

    int tentative_y = bonus->y + (int)(*vitesse_y);
    int bonus_x_ecran = bonus->x - (int)screenx;

    // vérification  des pixels sous le bonus
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
    // si pas de collision, on déplace, sinon on stoppe
    if (!collision) {
        bonus->y = tentative_y;
    } else {
        *vitesse_y = 0; // arrêt vertical
    }
}
void deplacement_pic(BonusPosition *pic, float *vitesse_y, float gravite_max, float acceleration, BITMAP *fond, float screenx, GrpPersonnages *groupe) {
    if (!pic->actif) return;

    // déclenchement de la chute si perso est proche en X et à droite du pic
    if (!pic->en_chute) {
        for (int i = 0; i < groupe->nb_personnages; i++) {
            int perso_x_monde = groupe->persos[i].x + (int)screenx;
            int distance_x = abs(pic->x - perso_x_monde);

            if (distance_x <= 300 && perso_x_monde > pic->x) {
                pic->en_chute = 1;  // Le pic tombe
                break;
            }
        }

        if (!pic->en_chute) return;  // si toujours pas déclenché, on quitte
    }

    // gravité classique
    *vitesse_y += acceleration;
    if (*vitesse_y > gravite_max) *vitesse_y = gravite_max;

    int tentative_y = pic->y + (int)(*vitesse_y);

    // collision sol
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

    // appliquer déplacement si pas de collision
    if (!collision) {
        pic->y = tentative_y;
    } else {
        *vitesse_y = 0;  // Stop au sol
    }
}




void gerer_bonus_colle(BonusPosition bonus[], GrpPersonnages *groupe, int screenx) {
    static int ancienne_touche_space = 0;
    static int temps_dernier_espace = -100; // éviter un double clic au début
    static int frame_actuelle = 0;
    frame_actuelle++;
    for (int b = 0; b < NB_BONUS; b++) {
        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&bonus[b],
                                groupe->persos[i].x, groupe->persos[i].y,
                                groupe->persos[i].largeur, groupe->persos[i].hauteur,
                                screenx)) {
                groupe->persos[i].timer_colle = 300;  // colle pendant 5 secondes
                groupe->persos[i].x_colle = groupe->persos[i].x;
                groupe->persos[i].compteur_espace_colle = 0;
                bonus[b].actif = 0;
                break;
                                }
        }
    }
    int touche_space = key[KEY_SPACE];
    for (int i = 0; i < groupe->nb_personnages; i++) {
        Personnage *p = &groupe->persos[i];

        if (p->timer_colle > 0) {
            // Détection de double appui rapide sur ESPACE
            if (touche_space && !ancienne_touche_space) {
                int intervalle = frame_actuelle - temps_dernier_espace;

                if (intervalle > 2 && intervalle < 20) { // entre 2 et 20 frames
                    p->timer_colle = 0; // décolle immédiatement
                    p->compteur_espace_colle = 0;
                }
                temps_dernier_espace = frame_actuelle;
            }
            p->timer_colle--;// décrémente le timer
        }
    }
    ancienne_touche_space = touche_space;
}


int collision_bonus_pic(BonusPosition *bonus, int perso_x, int perso_y, int perso_w, int perso_h, float screenx) {
    if (!bonus->actif) return 0; // si le bonus est déjà pris on sort

    int marge = bonus->largeur / 5; // marge de 20%
    if (marge < 5) marge = 5; // minimum de 5 pixels

    int bonus_ecran_x = bonus->x - (int)screenx; // position du bonus à l'écran

    int zone_x = bonus_ecran_x + marge; // zone x réduite
    int zone_y = bonus->y + marge; // zone y réduite
    int zone_largeur = bonus->largeur - 2 * marge; // largeur utile
    int zone_hauteur = bonus->hauteur - 2 * marge; // hauteur utile

    if (zone_largeur <= 0 || zone_hauteur <= 0) return 0; // si zone trop petite on sort

    if (perso_x + perso_w > zone_x && // collision en x
        perso_x < zone_x + zone_largeur &&
        perso_y + perso_h > zone_y && // collision en y
        perso_y < zone_y + zone_hauteur) {
        bonus->actif = 0; // désactive le bonus
        bonus->explosion_timer = 60; // déclenche explosion
        return 1; // collision détectée
        }

    return 0; // pas de collision
}

void gerer_collision_pics_dynamiques(GrpPersonnages *groupe, BonusPosition pics[], float decallage_scroll) {
    for (int j = 0; j < NB_PICS; j++) {
        if (!pics[j].actif) continue; // si le pic est inactif on passe
        for (int i = 0; i < groupe->nb_personnages; i++) {
            Personnage *p = &groupe->persos[i];
            if (p->timer_pic > 0) continue; // si immunisé on passe
            if (collision_bonus_pic(&pics[j], p->x, p->y, p->largeur, p->hauteur, decallage_scroll)) {
                // collision détectée par le haut
                // supprime le personnage touché
                for (int k = i; k < groupe->nb_personnages - 1; k++) {
                    groupe->persos[k] = groupe->persos[k + 1];
                }
                groupe->nb_personnages--; // on réduit le nombre total
                i--; // on ajuste l'indice
                break; // on arrête la boucle pour ce pic
            }
        }
    }
}
