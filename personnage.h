//
// Created by thier on 15/04/2025.
//

#ifndef PERSONNAGE_H
#define PERSONNAGE_H
#include <allegro.h>
#define  MAX_PERSONNAGES 10
#include <stdbool.h>
typedef struct {//vezezfzvevf
    int x,y;
    int vy;
    int frame;
    int largeur, hauteur;
    int mort;
    int timer_vitesse ,timer_pic,timer_colle;
    int compteur_espace_colle;
    int x_colle;
    BITMAP *sprites[5];
} Personnage;

typedef struct {
    Personnage persos[MAX_PERSONNAGES];
    int nb_personnages;
} GrpPersonnages;bool collision_pic(Personnage* p, BITMAP* fond, float screenx);
void deplacer_groupe(GrpPersonnages *g, BITMAP *fond, float screenx, int fin_scroll,float vitesse  );
void dessiner_groupe(GrpPersonnages *g, BITMAP *buffer );

void deplacer_personnage(Personnage *p, BITMAP *fond, float screenx, int fin_scroll );
void creation_personnage(Personnage *p,int x,int y, int largeur, int hauteur);
void dessiner_personnage(Personnage *p, BITMAP* buffer);
int collision_personnage(Personnage *p, BITMAP* fond, float screenx);
int collision_personnage2(Personnage *p, BITMAP* fond, float screenx);void gerer_collision_pics_groupe(GrpPersonnages *groupe, BITMAP *fond, float screenx) ;
bool groupe_est_mort(GrpPersonnages *groupe);
#endif //PERSONNAGE_H
