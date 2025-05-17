
#include <allegro.h>

#include "menu.h"
#include "boolean.h"
#include "jeu.h"
#include "bonus.h"


BITMAP* fusion(BITMAP** images, int nb_images) {
    if (images == NULL || nb_images == 0) {
        allegro_message("erreurdddd");
        exit(1);
    }
    int largeur_totale = 0;
    int hauteur = images[0]->h;
    for (int i = 0; i < nb_images; i++) {
        if (images[i] == NULL) {
            allegro_message("erreuraaaa", i);
            exit(1);
        }
        if (images[i]->h != hauteur) {
            allegro_message("erreurcccc.");
            exit(1);
        }
          largeur_totale += images[i]->w - 5;
    }
    BITMAP* resultat = create_bitmap(largeur_totale, hauteur);
    if (resultat == NULL) {
        allegro_message("erreurbbb");
        exit(1);
    }
    int x = 0;
    for (int i = 0; i < nb_images; i++) {
        blit(images[i], resultat, 0, 0, x, 0, images[i]->w, hauteur);
        x += images[i]->w-5;
    }
    return resultat;
}void scrollingNiv1(Joueur* j) {
    const char* noms_fichiers[] = {

        "niveau1_0.bmp",
        "niveau1_1.bmp",
        "niveau1_2.bmp" ,
        "niveau1_3.bmp",
        "niveau1_4.bmp",
    };
    const int nb_fonds = sizeof(noms_fichiers) / sizeof(noms_fichiers[0]);
    BITMAP* mes_fonds[nb_fonds];
    for (int i = 0; i < nb_fonds; i++) {
        mes_fonds[i] = load_bitmap(noms_fichiers[i], NULL);
        if (mes_fonds[i] == NULL) {
            for (int j = 0; j < i; j++) {
                destroy_bitmap(mes_fonds[j]);
            }
            exit(1);
        }
    }
    BITMAP* fond_final = fusion(mes_fonds, nb_fonds);
    jeu_niveau_1(fond_final,j);
    for (int i = 0; i < nb_fonds; i++) {
        destroy_bitmap(mes_fonds[i]);
    }
}
BITMAP* copier_bitmap(BITMAP* src) {
    if (!src) return NULL;
    BITMAP* copie = create_bitmap(src->w, src->h);
    if (!copie) return NULL;
    blit(src, copie, 0, 0, 0, 0, src->w, src->h);
    return copie;
}

//feeefe
void scrollingNiv2(Joueur *j) {
    const char* noms_fichiers[] = {
        "niveau2_0.bmp",
        "niveau2_1.bmp",
        "niveau2_2.bmp",
        "niveau2_3.bmp",
        "niveau2_4.bmp",
        "niveau2_5.bmp",
        "niveau2_6.bmp",
        "niveau2_7.bmp",

    };
    const int nb_fonds = sizeof(noms_fichiers) / sizeof(noms_fichiers[0]);
    BITMAP* mes_fonds[nb_fonds];
    for (int i = 0; i < nb_fonds; i++) {
        mes_fonds[i] = load_bitmap(noms_fichiers[i], NULL);
        if (mes_fonds[i] == NULL) {
            for (int j = 0; j < i; j++) {
                destroy_bitmap(mes_fonds[j]);
            }
            exit(1);
        }
    }
    BITMAP* fond_final = fusion(mes_fonds, nb_fonds);
    jeu_niveau_2(fond_final,j);
    for (int i = 0; i < nb_fonds; i++) {
        destroy_bitmap(mes_fonds[i]);
    }
}