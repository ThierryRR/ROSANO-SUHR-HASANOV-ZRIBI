//
// Created by timur on 18/05/2025.
//

#include "jeuniv3.h"


// fonction principale du niveau 3 du jeu
void jeu_niveau_3(BITMAP *fond_final, Joueur *j) {
    // copie du fond d'écran pour ce niveau
    BITMAP *fond = copier_bitmap(fond_final);
    BITMAP *page = create_bitmap(SCREEN_W, SCREEN_H); // bitmap temporaire pour l'affichage

    // chargement des bitmaps nécessaires au niveau
    BITMAP *malusvitesse = load_bitmap("4.bmp", NULL);
    BITMAP *bonustaille = load_bitmap("baloo.bmp", NULL);
    BITMAP *bonuscomportement = load_bitmap("5.bmp", NULL);
    BITMAP *bonuscomport = load_bitmap("6.bmp", NULL);
    BITMAP *sprite_pic = load_bitmap("pic.bmp", NULL);
    BITMAP *caillou = load_bitmap("caillou.bmp", NULL);

    // vérifie que tous les bitmaps ont bien été chargés
    if (!caillou || !sprite_pic || !bonuscomport || !bonuscomportement || !fond || !page || !malusvitesse || !bonustaille) {
        allegro_message("Erreur de chargement des ressources.");
        exit(1);
    }

    // création des bonus et malus du niveau 3
    BonusPosition mon_bonus4[NB_BONUS] = {// reductuon du scrolling
        creer_bonus(6700, 680, malusvitesse, NULL),
        creer_bonus(4800, 500, malusvitesse, NULL)
    };

    BonusPosition mon_bonus5[NB_BONUS] = {// permet de pas se faire tuer par les pics rouges
        creer_bonus(1980, 300, bonuscomportement, NULL),
        creer_bonus(8800, 600, bonuscomportement, NULL),
        creer_bonus(4120, 450, bonuscomportement, NULL)
    };

    BonusPosition mon_bonus6[NB_BONUS] = {//bonus collant
        creer_bonus(8500, 400, bonuscomport, NULL),
        creer_bonus(3401, 300, bonuscomport, NULL)
    };

    BonusPosition bonust[NB_BONUS] = {// grandit la taille
        creer_bonus(4040, 270, bonustaille, NULL),
        creer_bonus(12300, 580, bonustaille, NULL)
    };

    // création des rochers dynamiques
    BonusPosition mes_caillou[NB_PICS] = {
        creer_bonus(550, 200, caillou, NULL),
        creer_bonus(4770, 30, caillou, NULL),
        creer_bonus(9000, 200, caillou, NULL)
    };

    // initialisation du groupe de personnages
    GrpPersonnages groupe;
    groupe.nb_personnages = 1;

    // position initiale si héritée du niveau précédent
    if (j->niveau == 3 && j->reprise_x == 200 || j->reprise_x==5300 || j->reprise_x==4000|| j->reprise_x==7000 ) {
        j->reprise_x = 200;
        j->reprise_y = 700;
        sauvegarder_joueur(j);
    }

    // on ajuste la taille d'un caillou
    mes_caillou[1].largeur = (int)(mes_caillou[1].largeur * 0.6);
    mes_caillou[1].hauteur = (int)(mes_caillou[1].hauteur * 0.6);

    // calcul du scrolling horizontal
    float screenx = j->reprise_x - SCREEN_W / 2;
    if (screenx < 0) screenx = 0;
    if (screenx > fond->w - SCREEN_W) screenx = fond->w - SCREEN_W;
    int perso_x = j->reprise_x - (int)screenx;

    // vitesses verticales pour les éléments dynamiques
    float vitesses_y_cailloux[NB_PICS] = {0};

    // création du personnage
    creation_personnage(&groupe.persos[0], perso_x, j->reprise_y, 64, 64);

    // création du checkpoint
    checkpoint cp = creer_checkpoint(7500, 560, "drapeau0.bmp", "drapeau1.bmp");
    cp.largeur = cp.sprite[0]->w / 12;
    cp.hauteur = cp.sprite[0]->h / 12;

    // initialisation des variables de jeu
    game_over = false;
    int fin_scroll = fond->w - SCREEN_W;
    float dragon_speed = 1.0;
    int dragon_acceleration_timer = 0;
    int dragon_malus_timer = 0;
    int jeu_lance = 0;
    float gravite_max = 3.0f;
    float acceleration = 0.6f;
    int timer_bonus_taille = 0;

    // affichage d'intro
    clear_bitmap(page);
    blit(fond, page, (int)screenx, 0, 0, 0, SCREEN_W, SCREEN_H);
    dessiner_groupe(&groupe, page);
    textout_centre_ex(page, font, "Appuie sur SPACE pour commencer", SCREEN_W / 2, SCREEN_H - 30, makecol(255, 255, 255), -1);
    blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    clear_keybuf();

    // boucle d'attente de lancement du jeu
    while (!jeu_lance) {
        poll_keyboard();
        if (key[KEY_ESC]) goto FIN_JEU;
        if (key[KEY_SPACE]) {
            while (key[KEY_SPACE]) poll_keyboard();
            jeu_lance = 1;
        }
        rest(10);
    }

    // initialisation du timer allegro
    temps = 0;
    LOCK_VARIABLE(temps);
    LOCK_FUNCTION(temps_init);
    install_int_ex(temps_init, BPS_TO_TIMER(60));

    // boucle principale du niveau 3
    while (!game_over) {
        poll_keyboard();



        if (key[KEY_B]) {// retour au menu
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

        static int compteur_demo = 1;
        if (key[KEY_1]) {// lance un joueur test
            Joueur *demo = malloc(sizeof(Joueur));
            sprintf(demo->nom, "DEMO%d", compteur_demo++);
            demo->niveau = 3;
            demo->reprise_x = 10000;
            demo->reprise_y = 800;
            clear_keybuf();
            scrollingNiv3(demo);
            free(demo);
            show_mouse(screen);
            continue;
        }
        // mise à jour du jeu à chaque frame
        while (temps > 0) {
            poll_keyboard();
            int space = key[KEY_SPACE];
            gerer_acceleration(&dragon_speed, &dragon_acceleration_timer, &dragon_malus_timer, space);
            screenx += dragon_speed;// avance le scrolling
            if (screenx > fin_scroll) screenx = fin_scroll;
            // verifie que le gropue de personnage est sortie (dans ce niveau il ya quun seul perso)
            for (int i = 0; i < groupe.nb_personnages; i++) {
                int pos_abs = (int)screenx + groupe.persos[i].x;
                if (pos_abs >= fond->w) {
                    int choix = ecran_victoire();// retour niv 1
                    if (choix == 1) {
                        j->niveau = 1;
                        j->reprise_x = 500;
                        j->reprise_y = 800;
                        sauvegarder_joueur(j);
                        scrollingNiv1(j);
                    } else {
                        ecran_menu();//retour menu
                    }
                    goto FIN_NIVEAU;
                }
            }
            //fonction qui gere le deplacement , les collisions contre les pics et les bonus
            deplacer_groupe(&groupe, fond, screenx, fin_scroll, dragon_speed);
            gerer_collision_pics_groupe(&groupe, fond, screenx);
            gerer_malus_vitesse(mon_bonus4, &groupe, screenx, &dragon_malus_timer);
            gerer_taille_grand(bonust, &groupe, screenx, &timer_bonus_taille);
            gerer_bonus_colle(mon_bonus6, &groupe, screenx);
            gerer_bonus_immunite_pic(mon_bonus5, &groupe, screenx);
            // deplacement des pics
            for (int i = 0; i < NB_PICS; i++) {
                deplacement_pic(&mes_caillou[i], &vitesses_y_cailloux[i], gravite_max, acceleration, fond, screenx, &groupe);
            }

            gerer_collision_pics_dynamiques(&groupe, mes_caillou, screenx);
            // sauvegarde si checkpoint
            if (collision_checkpoint(&cp, &groupe, &j->reprise_x, &j->reprise_y, screenx)) {
                sauvegarder_joueur(j);
            }
           //defaite du personnage
            if (groupe_est_mort(&groupe)) {
                game_over = true;
            }
// affichage des donnes
            int int_screenx = (int)screenx;
            clear_bitmap(page);
            blit(fond, page, int_screenx, 0, 0, 0, SCREEN_W, SCREEN_H);

            for (int i = 0; i < NB_BONUS; i++) {
                afficher_bonus(mon_bonus4[i], page, screenx);
                afficher_bonus(mon_bonus5[i], page, screenx);
                afficher_bonus(mon_bonus6[i], page, screenx);
                afficher_bonus(bonust[i], page, screenx);
            }

            for (int i = 0; i < NB_PICS; i++) {
                afficher_bonus(mes_caillou[i], page, screenx);
            }

            afficher_checkpoint(page, cp, int_screenx);
            dessiner_groupe(&groupe, page);
            temps--;
        }

        blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }

FIN_NIVEAU:
    remove_int(temps_init);
    rest(200);
    if (game_over) {
        int choix = ecran_defaite();
        if (choix == 1) jeu_niveau_3(fond, j);// si defaite on relance
        else if (choix == 2) ecran_menu();//sinon retour menu
    } else if (!key[KEY_ESC]) {
        if (j->niveau < 3) {
            j->niveau = 3;
            sauvegarder_joueur(j);
        }
    }
// liberation memoure
FIN_JEU:
    destroy_bitmap(page);
    destroy_bitmap(fond);
    destroy_bitmap(malusvitesse);
    destroy_bitmap(bonustaille);
}