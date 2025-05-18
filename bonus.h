#ifndef BONUS_H
#define BONUS_H

#include <allegro.h>
#include "menu.h"
#include "personnage.h"
#include "scroll.h"

typedef struct {
    int x, y; // position du bonus
    int largeur, hauteur; // dimensions
    BITMAP *sprite; // image principale
    BITMAP *sprite_explosion[2]; // bitmap pour l'explosion
    int actif; // indique si le bonus est actif
    int explosion_timer; // temps d'affichage de l'explosion
    int en_chute; // si le bonus tombe ou non
} BonusPosition;

BonusPosition creer_bonus(int x, int y, BITMAP *sprite, BITMAP *explosion);

void gerer_bonus_colle(BonusPosition bonus[], GrpPersonnages *groupe, int screenx); // colle le perso
int collision_bonus_pic(BonusPosition *bonus, int perso_x, int perso_y, int perso_w, int perso_h, float screenx); // collision spécifique aux pics
void gerer_bonus_saut(BonusPosition mon_bonus3[], GrpPersonnages *groupe, int screenx, int *dragon_acceleration_timer); // bonus de saut
void gerer_malus_vitesse(BonusPosition mon_bonus4[], GrpPersonnages *groupe, int screenx, int *dragon_malus_timer); // malus de vitesse
void gerer_malus_clones(BonusPosition mon_bonus2[], GrpPersonnages *groupe, int screenx); // supprime des clones
void afficher_bonus(BonusPosition bonus, BITMAP *buffer, float screenx); // affiche bonus
void gerer_collision_pics_dynamiques(GrpPersonnages *groupe, BonusPosition pics[], float screenx); // collisions avec pics dynamiques
int collision_bonus(BonusPosition *bonus, int perso_x, int perso_y, int perso_w, int perso_h, float screenx); // collision avec un bonus
void gerer_bonus_clones(BonusPosition mon_bonus1[], GrpPersonnages *groupe, int screenx, int *timer_clones); // crée des clones
void gerer_taille_petit(BonusPosition malust[], GrpPersonnages*groupe, int screenx, int *timer_malus_taille); // réduit la taille
void gerer_taille_grand(BonusPosition bonust[], GrpPersonnages *groupe, int screenx, int *timer_bonus_taille); // agrandit la taille
void afficher_bonus_explosion(BonusPosition bonus, BITMAP *buffer, float screenx); // explosion visuelle
void deplacement_pic(BonusPosition *pic, float *vitesse_y, float gravite_max, float acceleration, BITMAP *fond, float screenx, GrpPersonnages *groupe); // mouvement des pics
void gerer_bonus_immunite_pic(BonusPosition mon_bonus5[], GrpPersonnages *groupe, int screenx); // immunité contre les pics
void deplacement_position_bonus(BonusPosition *bonus, float *vitesse_y, float gravite_max, float acceleration, BITMAP *fond, float screenx); // chute douce du bonus

#endif //BONUS_H
