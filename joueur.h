//
// Created by thier on 18/05/2025.
//

#ifndef JOUEUR_H
#define JOUEUR_H
#include <allegro.h>  // ajout de la biblotheque allegro
// structure joueur
typedef struct {
    char nom[50];      // nom
    int niveau;        // son niveau
    int reprise_x;     // position sur x
    int reprise_y;     // position sur y
} Joueur;
// charge les données d’un joueur à partir d’un fichier
int charger_Joueur(Joueur *j);
// sauvegarde les données du joueur (niveau, position..)  dans le fichier
int sauvegarder_joueur(Joueur *j);
// permet à l'utilisateur de saisir un  nom de joueur
Joueur *saisir_joueur(BITMAP *screen);
// recherche si un joueur existe déjà dans le fichier
int recherche_dans_fichier(Joueur *j);
// affiche un interface pour sélectionner un joueur existant dans la liste
Joueur *chargement_du_joueur(BITMAP *screen);
// crée un nouveau joueur
Joueur* nouveau_joueur(BITMAP *screen);

#endif //JOUEUR_H
