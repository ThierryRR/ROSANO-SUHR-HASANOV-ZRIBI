#include <allegro.h>
#include "menu.h"
#include "jeu.h"
#include "bonus.h"


BITMAP* fusion(BITMAP** images, int nb_images) {
    if (images == NULL || nb_images == 0) { // vérifie que les images soit utilisables
        allegro_message("erreur");
        exit(1);
    }
    int largeur_totale = 0;
    int hauteur = images[0]->h;
    for (int i = 0; i < nb_images; i++) {
        if (images[i] == NULL) { // vérifie pour chaque image qu'elle soit utilisable
            allegro_message("erreur");
            exit(1);
        }
        if (images[i]->h != hauteur) { // vérifie qu'elles aient la mm hauteur
            allegro_message("erreur.");
            exit(1);
        }
          largeur_totale += images[i]->w - 5;
    }
    BITMAP* resultat = create_bitmap(largeur_totale, hauteur); // crée le bitmap
    if (resultat == NULL) {
        allegro_message("erreur");
        exit(1);
    }
    int x = 0;
    for (int i = 0; i < nb_images; i++) {
        blit(images[i], resultat, 0, 0, x, 0, images[i]->w, hauteur);
        x += images[i]->w-5;
    }
    return resultat; // retourne le fond
}
void scrollingNiv1(Joueur* j) {
    const char* noms_fichiers[] = { // fichiers BMP du 1er niveau
        "niveau1_0.bmp",
        "niveau1_1.bmp",
        "niveau1_2.bmp" ,
        "niveau1_3.bmp",
        "niveau1_4.bmp",
    };
    const int nb_fonds = sizeof(noms_fichiers) / sizeof(noms_fichiers[0]);
    BITMAP* mes_fonds[nb_fonds];
    for (int i = 0; i < nb_fonds; i++) {
        mes_fonds[i] = load_bitmap(noms_fichiers[i], NULL); // chargement des BITMAPS
        if (mes_fonds[i] == NULL) {
            for (int j = 0; j < i; j++) {
                destroy_bitmap(mes_fonds[j]);
            }
            exit(1);
        }
    }
    BITMAP* fond_final = fusion(mes_fonds, nb_fonds); // Fusionne toutes les images
    jeu_niveau_1(fond_final,j); // Lance le niveau 1
    for (int i = 0; i < nb_fonds; i++) {
        destroy_bitmap(mes_fonds[i]);
    }
}
BITMAP* copier_bitmap(BITMAP* src) { // copie d'un BITMAP
    if (!src) return NULL;
    BITMAP* copie = create_bitmap(src->w, src->h);
    if (!copie) return NULL;
    blit(src, copie, 0, 0, 0, 0, src->w, src->h);
    return copie;
}

void scrollingNiv2(Joueur *j) {
    const char* noms_fichiers[] = { // toutes les images du 2ème niveau
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
        mes_fonds[i] = load_bitmap(noms_fichiers[i], NULL); // charge les BMP
        if (mes_fonds[i] == NULL) {
            for (int j = 0; j < i; j++) {
                destroy_bitmap(mes_fonds[j]); // détruit en cas d'erreur
            }
            exit(1);
        }
    }
    BITMAP* fond_final = fusion(mes_fonds, nb_fonds);
    jeu_niveau_2(fond_final,j); // lance le niveau 2
    for (int i = 0; i < nb_fonds; i++) {
        destroy_bitmap(mes_fonds[i]);
    }
}
void scrollingNiv3(Joueur *j) {
    const char* noms_fichiers[] = { // toutes les images du 3ème niveau
        "niveau3_0.bmp",
        "niveau3_1.bmp",
        "niveau3_2.bmp",
        "niveau3_3.bmp",
        "niveau3_5.bmp",
        "niveau3_6.bmp",
        "niveau3_7.bmp",
        "niveau3_8.bmp",

    };
    const int nb_fonds = sizeof(noms_fichiers) / sizeof(noms_fichiers[0]);
    BITMAP* mes_fonds[nb_fonds];
    for (int i = 0; i < nb_fonds; i++) {
        mes_fonds[i] = load_bitmap(noms_fichiers[i], NULL); // charge les BITMAPS
        if (mes_fonds[i] == NULL) {
            for (int j = 0; j < i; j++) {
                destroy_bitmap(mes_fonds[j]);
            }
            exit(1);
        }
    }
    BITMAP* fond_final = fusion(mes_fonds, nb_fonds);
    jeu_niveau_3(fond_final,j); // lance le niveau 3
    for (int i = 0; i < nb_fonds; i++) {
        destroy_bitmap(mes_fonds[i]);
    }
}