#ifndef PERSONNAGE_H
#define PERSONNAGE_H

#define MAX_PERSONNAGES 10

#include <allegro.h>
#include <stdbool.h>

// structure du personnage
typedef struct {
    int x, y;  // position
    int vy; // vitesse verticale
    int frame;// frame actuelle pour l'animation
    int largeur, hauteur;  // dimensions
    int mort;  // statut mort ou vivant
    int timer_vitesse, timer_pic;// timers de bonus/malus
    int timer_colle;     // timer pour effet "colle"
    int compteur_espace_colle;   // compteur pour désactivation de la colle
    int x_colle;   // position collée
    BITMAP *sprites[5];// tableau de sprites
} Personnage;
// structure du groupe de personnages
typedef struct {
    Personnage persos[MAX_PERSONNAGES]; // tableau de personnages
    int nb_personnages;// nombre actuel dans le groupe
} GrpPersonnages;

bool collision_pic(Personnage* p, BITMAP* fond, float screenx);// fonction qui teste si un personnage touche un pic

void deplacer_groupe(GrpPersonnages *g, BITMAP *fond, float screenx, int fin_scroll, float vitesse);// déplace tout le groupe

void dessiner_groupe(GrpPersonnages *g, BITMAP *buffer);// dessine tous les personnages du groupe
void deplacer_personnage(Personnage *p, BITMAP *fond, float screenx, int fin_scroll);// déplace un personnage seul
void creation_personnage(Personnage *p, int x, int y, int largeur, int hauteur);// initialise un personnage
void dessiner_personnage(Personnage *p, BITMAP* buffer);// dessine un personnage sur l'écran
int collision_personnage(Personnage *p, BITMAP* fond, float screenx);// détecte les collisions avec le décor
void gerer_collision_pics_groupe(GrpPersonnages *groupe, BITMAP *fond, float screenx);// applique les collisions entre les personnages et les pics du décor
bool groupe_est_mort(GrpPersonnages *groupe);// teste si le groupe est vide (tous morts)

#endif //PERSONNAGE_H
