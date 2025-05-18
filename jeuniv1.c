 #include <stdio.h>
#include <allegro.h>
#include "jeuniv1.h"
#include "menu.h"
#include "personnage.h"
#include "boolean.h"
#include "scroll.h"
#include "bonus.h"
#include "joueur.h"
#include "chekpoint .h"
#include "pic.h"

void jeu_niveau_1(BITMAP *fond_final, Joueur *j) {//fonction qui gère la logique du niveau1
    //initialisation des bitmaps
    BITMAP *fond = copier_bitmap(fond_final);
    BITMAP *page = create_bitmap(SCREEN_W, SCREEN_H);

    if ( !fond || !page ) {//verification de l'initialisation
        allegro_message("Erreur de chargement des ressources.");
        exit(1);
    }// initialisation des personnages (ici le nombre restera à 1)
    GrpPersonnages groupe;
    groupe.nb_personnages = 1;

    if (strncmp(j->nom, "DEMO", 4) != 0) {// si cest pas un joueur demo
        // position initiale sur x et y
        j->reprise_x = 500;
        j->reprise_y = 800;
    }


    int reprise_x = j->reprise_x;
    int reprise_y = j->reprise_y;
    float screenx = reprise_x - SCREEN_W / 2;//calcul du scroll
    if (screenx < 0) screenx = 0;//limite a gauche
    if (screenx > fond->w - SCREEN_W) screenx = fond->w - SCREEN_W;//limite à droite
    int perso_x = reprise_x - (int)screenx;//position du dragon sur l'ecran


    creation_personnage(&groupe.persos[0], perso_x, reprise_y, 64, 64);// creation du personnage sur l'écran



    game_over = false;
    int fin_scroll = fond->w - SCREEN_W;// fin du noveau
    float dragon_speed = 1.0;// vitesse initiale du scroll
    //initialisation des timers (ils ne sont pas présents dans ce niveau)
    int dragon_acceleration_timer = 0;
    int dragon_malus_timer = 0;
    int jeu_lance = 0;

//affichage de l'ecran avant le lancer du jeu
    clear_bitmap(page);
    blit(fond, page, (int)screenx, 0, 0, 0, SCREEN_W, SCREEN_H);
    dessiner_groupe(&groupe, page);
    textout_centre_ex(page, font, "Appuie sur SPACE pour commencer", SCREEN_W / 2, SCREEN_H - 30, makecol(255, 255, 255), -1);
    blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    clear_keybuf();
// boucle avant le demarrage
    while (!jeu_lance) {
        poll_keyboard();
        if (key[KEY_ESC]) goto FIN_JEU;//quitter le jeu avec echap
        if (key[KEY_SPACE]) {// permet de lancer le jeu
            while (key[KEY_SPACE]) poll_keyboard();//evite les anti-rebonds
            jeu_lance = 1;
        }
        rest(10);
    }
//initialistation du timer
    temps = 0;
    LOCK_VARIABLE(temps);
    LOCK_FUNCTION(temps_init);
    install_int_ex(temps_init, BPS_TO_TIMER(60));
//boucle du jeu
    while (!game_over) {
        poll_keyboard();


        if (key[KEY_B]) {//retour à l'écran dacceuil
            clear_keybuf();
            show_mouse(NULL);
            affichage_ecran_dacceuil();
            clear_keybuf();
            destroy_bitmap(page);
            destroy_bitmap(fond);
            remove_int(temps_init);
            ecran_menu();

            return;
        }
//gestion des pixels selon le timer
        while (temps > 0) {
            poll_keyboard();
            int space = key[KEY_SPACE];
            gerer_acceleration(&dragon_speed, &dragon_acceleration_timer, &dragon_malus_timer, space);//gere lacceleration du scrolling

            screenx += dragon_speed;// avance le fond
            if (screenx > fin_scroll) screenx = fin_scroll;//limite la fin du niveau
            //verifie que le joueur quitte lecran
            for (int i = 0; i < groupe.nb_personnages; i++) {
                int pos_abs = (int)screenx + groupe.persos[i].x;
                if (pos_abs >= fond->w) {
                    int choix = ecran_victoire();//affichage de l'ecran de victoire
                    if (choix == 1) {//passse au niveau suivant
                        j->niveau = 2;
                        sauvegarder_joueur(j);
                        scrollingNiv2(j);
                    } else {//retour au menu


                            if (j->niveau < 2) {
                                j->niveau = 2;
                                sauvegarder_joueur(j);
                            }
                            ecran_menu();

                    }
                    goto FIN_NIVEAU;
                }
            }
            //mode test pour accéder rapidement a une zone avancée
            //créeation dynamique d'un joueur demo quand 1 est appuyé
            static int compteur_demo = 1;
            if (key[KEY_1]) {
                Joueur *demo = malloc(sizeof(Joueur));
                sprintf(demo->nom, "DEMO%d", compteur_demo++);
                demo->niveau = 1;
                demo->reprise_x = 3300;
                demo->reprise_y = 600;

                clear_keybuf();
                scrollingNiv1(demo);

                free(demo);
                show_mouse(screen);
                continue;
            }
            //mise a jour de la position du groupe
            deplacer_groupe(&groupe, fond, screenx, fin_scroll ,dragon_speed );
            if (groupe_est_mort(&groupe)) game_over = true; //tous les personnages sont morts -> fin de jeu
            //double buffering et gestion de l'affichage avec le scrolling
            int int_screenx = (int)screenx;
            int part1 = fond->w - int_screenx; // part1 représente la portion du fond restant à partir de la position screenx jusqu'à la fin du fond
            int part2 = SCREEN_W - part1; // calcul de la portion restante de l'écran à compléter si la fin du fond est atteinte
                                          // part2 est utilisée uniquement si part1 est insuffisant pour remplir tout l’écran
            clear_bitmap(page);
            // ici si la portion restante à partir de screenx suffit à remplir l’écran, on affiche la portion visible
            if (part1 >= SCREEN_W) {
                blit(fond, page, int_screenx, 0, 0, 0, SCREEN_W, SCREEN_H);
            } else {// sinon, on est à la fin du fond et on doit faire un "wrap" circulaire
                    // affiche d’abord la fin du fond (part1 pixels), puis complete le reste de l'écran avant le début du fond
                blit(fond, page, int_screenx, 0, 0, 0, part1, SCREEN_H);
                blit(fond, page, 0, 0, part1, 0, part2, SCREEN_H);
            }
            dessiner_groupe(&groupe, page);
            temps--;
        }

        blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H); // affichage le contenu de l’image tampon (page) à l’écran (double buffering)

    }

FIN_NIVEAU:
    remove_int(temps_init);
    rest(200);

    // gestion de l'écran de défaite
    if (game_over) {
        int choix = ecran_defaite();// appel de la fonction ecran defaite
        if (choix == 1) {
            j->niveau = 1;
            sauvegarder_joueur(j);
            jeu_niveau_1(fond, j);// on relance
        } else {//sinon retour ecran menu
            ecran_menu();
        }
    } else if (!key[KEY_ESC]) {
        if (j->niveau < 2) {
            j->niveau = 2;
            sauvegarder_joueur(j);
        }
    }

FIN_JEU:
    destroy_bitmap(page);//libération mémoire
    destroy_bitmap(fond);

}


