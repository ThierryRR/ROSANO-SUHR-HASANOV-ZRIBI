#ifndef SCROLL_H
#define SCROLL_H
// fusionne plusieurs bitmaps horizontalement pour créer un fond de niveau complet
BITMAP* fusion(BITMAP** images, int nb_images);
// copie un bitmap source et retourne un nouveau bitmap identique
BITMAP* copier_bitmap(BITMAP* src);
// fonctions de lancement de chaque niveau
void scrollingNiv1(Joueur* j);// lance le niveau 1
void scrollingNiv2(Joueur* j);// Lance le niveau 2
void scrollingNiv3(Joueur* j); // lance le niveau 3

#endif //SCROLL_H
