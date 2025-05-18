#include <allegro.h>
#include "menu.h"
#include "stdio.h"
#include "scroll.h"
#include "boolean.h"
#include "joueur.h"

Joueur* chargement_du_joueur(BITMAP* screen) {// fonction de chargement d'un joueur
    FILE *pf = fopen("joueur.txt", "r");// ouvre le fichier joueur et verification
    if (pf == NULL) {
        allegro_message("erreur");
        return NULL;
    }
// stockage temporaire des joueurs
    char noms[100][30];
    int niveaux[100], xs[100], ys[100];
    int nb = 0;
    while (fscanf(pf, "%s %d %d %d", noms[nb], &niveaux[nb], &xs[nb], &ys[nb]) == 4 && nb < 100) {// lecture
        nb++;
    }
    fclose(pf);

    BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);// creation et verification du buffer
    if (!buffer) {
        allegro_message("erreur");
        return NULL;
    }

    BITMAP *fond = load_bitmap("dragon_centre.bmp", NULL);// creation et verification de l'image
    if (!fond) {
        allegro_message("erreur");
        destroy_bitmap(buffer);
        return NULL;
    }
// parametres des carrés qui font office de boutons
    int ligne_hauteur = 50;
    int start_y = 150;
    int case_x = SCREEN_W - 100;
    int case_l = 30;
    int decalage_y = 20;

    while (1) {// boucle infini pour choisir un joueur
        poll_keyboard();
        if (key[KEY_ESC]) break;
        if (key[KEY_SPACE]) {
            show_mouse(NULL);
            destroy_bitmap(buffer);
            destroy_bitmap(fond);
            return NULL;

        }
// affichage
        draw_sprite(buffer, fond, 0, 0);
        textout_ex(buffer, font, "Clique sur une case pour sélectionner un joueur", 50, 50, makecol(255,255,255), -1);
// affichage des joueurs
        for (int i = 0; i < nb; i++) {
            int ligne_y = start_y + i * ligne_hauteur;
            int texte_y = ligne_y + decalage_y;
            int case_y = ligne_y + (ligne_hauteur - case_l) / 2;

            textout_ex(buffer, font, noms[i], 50, texte_y, makecol(255,255,255), -1);
            char niveau_str[20];
            sprintf(niveau_str, "Niv %d", niveaux[i]);
            textout_ex(buffer, font, niveau_str, 300, texte_y, makecol(255,255,255), -1);

            rect(buffer, case_x, case_y, case_x + case_l, case_y + case_l, makecol(255,255,255));
        }

        show_mouse(buffer);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);// affichage à  l'ecrzn

        if (mouse_b & 1) {// clic gauche de la souris
            for (int i = 0; i < nb; i++) {
                int ligne_y = start_y + i * ligne_hauteur;
                int case_y = ligne_y + (ligne_hauteur - case_l) / 2;

                if (mouse_x >= case_x && mouse_x <= case_x + case_l &&
                    mouse_y >= case_y && mouse_y <= case_y + case_l) {// si cesr dans la bonne case

                    Joueur* j = malloc(sizeof(Joueur));// cration du joueur selection
                    if (!j) {
                        destroy_bitmap(buffer);
                        destroy_bitmap(fond);
                        allegro_message("erreur");
                        return NULL;
                    }
// recuperztion des donnes
                    strcpy(j->nom, noms[i]);
                    j->niveau = niveaux[i];
                    j->reprise_x = xs[i];
                    j->reprise_y = ys[i];

                    destroy_bitmap(fond);
                    destroy_bitmap(buffer);
                    clear_keybuf();
                    return j;// retourne le joueur on peut reprendre
                }
            }
        }

        rest(10);
    }
// liberation de la memoire
    destroy_bitmap(fond);
    destroy_bitmap(buffer);
    clear_keybuf();
    return NULL;
}

int sauvegarder_joueur(Joueur *j) {
    // les joueurs de test ne sont pas sauvegarder
    if (strncmp(j->nom, "DEMO", 4) == 0) return 1;

    FILE *pf = fopen("joueur.txt", "r");// ouverture
    char lignes[100][50];
    int niveaux[100], xs[100], ys[100];
    int nb_joueurs = 0, modifie = 0;

    if (pf != NULL) {
        while (fscanf(pf, "%s %d %d %d", lignes[nb_joueurs], &niveaux[nb_joueurs], &xs[nb_joueurs], &ys[nb_joueurs]) == 4) {// lecture du ficher
            if (strcmp(j->nom, lignes[nb_joueurs]) == 0) {// mise à jour du joueur si joueur deja existant
                niveaux[nb_joueurs] = j->niveau;
                xs[nb_joueurs] = j->reprise_x;
                ys[nb_joueurs] = j->reprise_y;
                modifie = 1;
            }
            nb_joueurs++;
        }
        fclose(pf);
    }
// nouveau joueur à ajouter
    if (!modifie) {
        if (nb_joueurs >= 16) {// interface pour supprimer un joueur si la limite est atteinte
            BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);
            BITMAP *fond = load_bitmap("dragon_centre.bmp", NULL);
            if (!buffer || !fond) {
                if (buffer) destroy_bitmap(buffer);
                if (fond) destroy_bitmap(fond);
                return 0;
            }

            int ligne_hauteur = 50;
            int start_y = 150;
            int case_x = SCREEN_W - 100;
            int case_l = 30;
            int decalage_y = 20;

            while (1) {
                poll_keyboard();
                if (key[KEY_TAB]) {
                    clear_keybuf();
                    show_mouse(NULL);
                    destroy_bitmap(buffer);
                    destroy_bitmap(fond);
                    clear_keybuf();
                    game_over = false;
                    return 0;
                }
// affichage à l'ecran si trop de sauvegarde
                draw_sprite(buffer, fond, 0, 0);
                textout_ex(buffer, font, "Limite de 16 sauvegardes atteinte", 50, 50, makecol(255,0,0), -1);
                textout_ex(buffer, font, "Clique sur une case pour SUPPRIMER un joueur", 50, 80, makecol(255,255,255), -1);
                textout_ex(buffer, font, "Appuie sur TAB pour quitter", 50, 110, makecol(200,200,200), -1);
// affichage des joueurs et de leurs parametres
                for (int i = 0; i < nb_joueurs; i++) {
                    // zone des boutoons
                    int ligne_y = start_y + i * ligne_hauteur;
                    int texte_y = ligne_y + decalage_y;
                    int case_y = ligne_y + (ligne_hauteur - case_l) / 2;

                    if (strlen(lignes[i]) > 0) {// information des joueurs
                        textout_ex(buffer, font, lignes[i], 50, texte_y, makecol(255,255,255), -1);
                        char niveau_str[20];
                        sprintf(niveau_str, "Niv %d", niveaux[i]);
                        textout_ex(buffer, font, niveau_str, 300, texte_y, makecol(255,255,255), -1);
                        rect(buffer, case_x, case_y, case_x + case_l, case_y + case_l, makecol(255,255,255));
                    }
                }

                show_mouse(buffer);
                blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);// affichage
// si on clique dans une des cases / boutons
                if (mouse_b & 1) {
                    for (int i = 0; i < nb_joueurs; i++) {
                        int ligne_y = start_y + i * ligne_hauteur;
                        int case_y = ligne_y + (ligne_hauteur - case_l) / 2;

                        if (mouse_x >= case_x && mouse_x <= case_x + case_l &&
                            mouse_y >= case_y && mouse_y <= case_y + case_l &&
                            strlen(lignes[i]) > 0) {

                            lignes[i][0] = '\0';
                            niveaux[i] = -1;
// reecriture du fichier
                            FILE *pfw = fopen("joueur.txt", "w");
                            if (!pfw) {
                                destroy_bitmap(buffer);
                                destroy_bitmap(fond);
                                return 0;
                            }
                            for (int j = 0; j < nb_joueurs; j++) {// modification des info
                                if (strlen(lignes[j]) > 0 && niveaux[j] != -1) {
                                    fprintf(pfw, "%s %d %d %d\n", lignes[j], niveaux[j], xs[j], ys[j]);
                                }
                            }
                            fclose(pfw);
                            show_mouse(screen);
                            destroy_bitmap(buffer);
                            destroy_bitmap(fond);
                            clear_keybuf();
                            game_over = false;
                            return 2;// retour du nouveau joueur
                        }
                    }
                }

                rest(10);
            }
        }
//  ajoute le joueur si il ya de lespace
        strcpy(lignes[nb_joueurs], j->nom);
        niveaux[nb_joueurs] = j->niveau;
        xs[nb_joueurs] = j->reprise_x;
        ys[nb_joueurs] = j->reprise_y;
        nb_joueurs++;
    }
// on reecris avec toutes les nouvelles donnes
    pf = fopen("joueur.txt", "w");
    if (!pf) return 0;

    for (int i = 0; i < nb_joueurs; i++) {
        fprintf(pf, "%s %d %d %d\n", lignes[i], niveaux[i], xs[i], ys[i]);
    }
    fclose(pf);
    return 1;// retoune le fichier fini
}
Joueur *saisir_joueur(BITMAP *screen) {
    BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);
    if (!buffer) {
        allegro_message("Erreur : crÃ©ation buffer.");
        return NULL;
    }

    BITMAP *fond = load_bitmap("ecran_nom_final.bmp", NULL);
    if (!fond) {
        destroy_bitmap(buffer);
        allegro_message("Erreur : fond introuvable.");
        return NULL;
    }

    BITMAP *texte_temp = create_bitmap(850, 60); // Buffer pour les dimensions du bandeau
    if (!texte_temp) {
        destroy_bitmap(buffer);
        destroy_bitmap(fond);
        allegro_message("Erreur : texte temp.");
        return NULL;
    }

    Joueur *j = malloc(sizeof(Joueur));
    if (!j) {
        destroy_bitmap(buffer);
        destroy_bitmap(fond);
        destroy_bitmap(texte_temp);
        allegro_message("Erreur : joueur malloc.");
        return NULL;
    }

    int i = 0;
    int touche = 1;
    j->niveau = 1;
    j->nom[0] = '\0';
    j->reprise_x = 75;
    j->reprise_y = 300;

    while (!key[KEY_ENTER]) {
        poll_keyboard();
        // Si l'utilisateur appuie sur ESPACE, on quitte et on revient au menu
        if (key[KEY_SPACE]) {
            show_mouse(NULL);
            destroy_bitmap(buffer);
            destroy_bitmap(fond);
            destroy_bitmap(texte_temp);
            free(j);
            clear_keybuf();
            return NULL;
        }

        if (keypressed() && touche) {
            int k = readkey();
            char c = k & 0xFF;
            k = k >> 8;
            if (k == KEY_BACKSPACE && i > 0) {
                i--;
                j->nom[i] = '\0';
            } else if (i < 49 && ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))) {
                j->nom[i++] = c;
                j->nom[i] = '\0';
            }
            touche = 0;
        }

        if (!keypressed()) touche = 1;

        draw_sprite(buffer, fond, 0, 0);
        clear_to_color(texte_temp, makecol(255, 0, 255)); // couleur transparente
        textout_centre_ex(texte_temp, font, j->nom, texte_temp->w / 2, 20, makecol(0, 0, 0), -1);
        stretch_sprite(buffer, texte_temp, 450, 500, 850, 200);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        show_mouse(buffer);
        rest(10); // Petite pause
    }

    clear_keybuf();
    show_mouse(screen);
    destroy_bitmap(texte_temp);
    destroy_bitmap(buffer);
    destroy_bitmap(fond);
    return j;
}


Joueur* nouveau_joueur(BITMAP *screen) {// fonction qui initialise un nouveau joueur
    FILE *pf = fopen("joueur.txt", "r");
    if (!pf) {
        allegro_message("erreur");
        return NULL;
    }

    int count = 0;
    char ligne[100];
    while (fgets(ligne, sizeof(ligne), pf)) {// compte les joueurs
        if (ligne[0] != '\0' && ligne[0] != '\n') {
            count++;
        }
    }
    fclose(pf);

    if (count >= 16) { // si 16 joueurs, on crée un joueur test
        Joueur *temp = malloc(sizeof(Joueur));
        if (!temp) return NULL;
        strcpy(temp->nom, "DEMO");
        temp->niveau = 1;
        temp->reprise_x = 0;
        temp->reprise_y = 0;
        int res = sauvegarder_joueur(temp);
        free(temp);
        if (res == 0) return NULL;
    }

    while (1) { // boucle jusqu'à obtenir un nom valide
        clear_keybuf();
        Joueur *j = saisir_joueur(screen); // entrée du nom
        if (j == NULL) {
            clear_keybuf();
            return NULL;  // retour menu
        }

        if (recherche_dans_fichier(j)) {// si pseudo déjà utilisé
            BITMAP *fond = load_bitmap("ecran_nom_final.bmp", NULL);
            BITMAP *texte_temp = create_bitmap(850, 60);
            if (fond && texte_temp) {
                draw_sprite(screen, fond, 0, 0);
                clear_to_color(texte_temp, makecol(255, 0, 255));
                char msg[100];
                sprintf(msg, "Le pseudo %s existe déjà", j->nom);
                textout_centre_ex(texte_temp, font, msg, texte_temp->w / 2, 20, makecol(0, 0, 0), -1);
                stretch_sprite(screen, texte_temp, 450, 500, 850, 200);

                show_mouse(screen);
                rest(1500);

                destroy_bitmap(texte_temp);
                destroy_bitmap(fond);
            }
            free(j);
            continue;
        }

        int statut = sauvegarder_joueur(j); // sauvegarde
        if (statut == 0) {
            free(j);
            return NULL;
        }

        // message de bienvenue dans le bandeau
        BITMAP *fond = load_bitmap("ecran_nom_final.bmp", NULL);
        BITMAP *texte_temp = create_bitmap(850, 60);
        if (fond && texte_temp) {
            draw_sprite(screen, fond, 0, 0);
            clear_to_color(texte_temp, makecol(255, 0, 255));
            char msg[100];
            sprintf(msg, "Bienvenue %s !!", j->nom);
            textout_centre_ex(texte_temp, font, msg, texte_temp->w / 2, 20, makecol(0, 0, 0), -1);
            stretch_sprite(screen, texte_temp, 450, 500, 850, 200);

            show_mouse(screen);
            rest(1500);

            destroy_bitmap(texte_temp);
            destroy_bitmap(fond);
        }

        return j; // joueur prêt à jouer
    }
}


int recherche_dans_fichier(Joueur *j) {// fonction qui cherche si nom est deja exitant
    FILE *pf = fopen("joueur.txt", "r");// lecture du fichier
    if (pf == NULL) {
        allegro_message("erreur");
        exit(1);
    }

    char nom_fichier[50];
    int niveau, x, y;

    while (fscanf(pf, "%s %d %d %d", nom_fichier, &niveau, &x, &y) == 4) {//lecture des donnes
        if (strcmp(j->nom, nom_fichier) == 0) {
            fclose(pf);
            return 1;// pseudo existant
        }
    }

    fclose(pf);
    return 0;// pseudo nexite pas
}

