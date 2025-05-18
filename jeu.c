#include <stdio.h>
#include <allegro.h>
#include "jeu.h"
#include "menu.h"
#include "personnage.h"
#include "boolean.h"
#include "scroll.h"
#include "bonus.h"
#include "joueur.h"
#include "chekpoint .h"
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

        if (key[KEY_ESC]) {//permet de quitter le jeu
            allegro_exit();
            exit(0);
        }
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
                demo->reprise_y = 300;

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
    if (game_over) {//affichage écran victoire.défaite et recuperation du choix du joueur
        int choix = ecran_victoire();
        if (choix == 1) { //passage au niveau 2
            j->niveau = 2;
            j->reprise_x = 300;
            j->reprise_y = 500;
            sauvegarder_joueur(j);
            jeu_niveau_2(fond, j);
        } else {//retourne au menu principal
            ecran_menu();
        }


    } else if (!key[KEY_ESC]) {
        if (j->niveau < 2) { //mise a jour de la progression du joueur
            j->niveau = 2;
            sauvegarder_joueur(j);
        }
    }

FIN_JEU:
    destroy_bitmap(page);//libération mémoire
    destroy_bitmap(fond);

}
void jeu_niveau_2(BITMAP *fond_final, Joueur *j) {
    // copie du fond passé en paramètre pour éviter les modifications directes
    BITMAP *fond = copier_bitmap(fond_final);
    BITMAP *page = create_bitmap(SCREEN_W, SCREEN_H); // bitmap tampon pour l'affichage

    // chargement des sprites utilisés dans le niveau
    BITMAP *sprite_bonus = load_bitmap("1.bmp", NULL);
    BITMAP *sprite_bonus3 = load_bitmap("2.bmp", NULL);
    BITMAP *bombe0 = load_bitmap("bombe0.bmp", NULL);
    BITMAP *bombe1 = load_bitmap("bombe1.bmp", NULL);
    BITMAP *malustaille = load_bitmap("malus.bmp", NULL);
    BITMAP *bonustaille = load_bitmap("baloo.bmp", NULL);
    BITMAP *sprite_pic = load_bitmap("pic.bmp", NULL);
    BITMAP *caillou = load_bitmap("caillou.bmp", NULL);

    // vérifie que tous les bitmaps ont bien été chargés
    if (!caillou||!sprite_pic|| !fond || !page || !sprite_bonus || !sprite_bonus3 || !bombe0 || !bombe1  || !malustaille || !bonustaille) {
        allegro_message("Erreur de chargement des ressources.");
        exit(1);
    }

    // initialisation des différents groupes de bonus/malus
    BonusPosition mon_bonus1[NB_BONUS] = {// bonus de clonage
        creer_bonus(6300, 300, sprite_bonus, NULL),
        creer_bonus(300, 250, sprite_bonus, NULL),
        creer_bonus(1500, 700, sprite_bonus, NULL)
    };

    BonusPosition mon_bonus2[NB_BONUS] = {//malus qui tue les clones
        creer_bonus(1000, 700, bombe0, bombe1),
        creer_bonus(3300, 500, bombe0, bombe1),
        creer_bonus(4000, 280, bombe0, bombe1)
    };

    BonusPosition mon_bonus3[NB_BONUS] = {// bonus de vitesse
        creer_bonus(4390, 600, sprite_bonus3, NULL),
        creer_bonus(1800, 680, sprite_bonus3, NULL),
        creer_bonus(7800, 680, sprite_bonus3, NULL),
        creer_bonus(12500, 500, sprite_bonus3, NULL),
    };

    BonusPosition malust[NB_BONUS] = {// malus de taille
        creer_bonus(400, 800, malustaille, NULL),
        creer_bonus(3600, 250, malustaille, NULL),
        creer_bonus(3600, 780, malustaille, NULL),
        creer_bonus(6700, 490, malustaille, NULL),
    };

    // création des pics dynamiques
    BonusPosition mes_pics[NB_PICS]={
        creer_bonus(2425, 305, sprite_pic, NULL),
        creer_bonus(10000, 45, sprite_pic, NULL),
        creer_bonus(4300, 340, sprite_pic, NULL)
    };

    // création des rochers dynamiques
    BonusPosition mes_caillou[NB_PICS]={
        creer_bonus(200, 100, caillou, NULL),
        creer_bonus(1900, 230, caillou, NULL),
        creer_bonus(3800, 200, caillou, NULL)
    };

    // initialisation du groupe avec un seul personnage
    GrpPersonnages groupe;
    groupe.nb_personnages = 1;

    // position initiale du joueur si pas encore définie
    if  (j->niveau == 2 && j->reprise_x == 500) {
        j->reprise_x = 200;
        j->reprise_y = 600;
    }

    // calcul du scroll horizontal pour centrer le joueur
    float screenx = j->reprise_x - SCREEN_W / 2;
    if (screenx < 0) screenx = 0;
    if (screenx > fond->w - SCREEN_W) screenx = fond->w - SCREEN_W;
    int perso_x = j->reprise_x - (int)screenx;

    // sauvegarde des positions de reprise (pour les checkpoints)
    int reprise_x = j->reprise_x;
    int reprise_y = j->reprise_y;

    // initialisation des vitesses verticales pour les éléments dynamiques
    float vitesses_y_pics[NB_PICS] = {0};
    float vitesses_y_cailloux[NB_PICS] = {0};

    // création du personnage
    creation_personnage(&groupe.persos[0], perso_x, reprise_y, 64, 64);

    // création du checkpoint
    checkpoint cp = creer_checkpoint(5300, 690, "drapeau0.bmp", "drapeau1.bmp");
    cp.largeur = cp.sprite[0]->w / 6;
    cp.hauteur = cp.sprite[0]->h / 6;

    // réduction de la taille des bombes et bonus
    for (int b = 0; b < NB_BONUS; b++) {
        mon_bonus2[b].largeur = bombe0->w / 12;
        mon_bonus2[b].hauteur = bombe0->h / 12;
        mon_bonus3[b].largeur = sprite_bonus3->w / 12;
        mon_bonus3[b].hauteur = sprite_bonus3->h / 12;
    }

    // double la taille des pics pour améliorer la visibilité
    for (int b = 0; b < NB_PICS; b++) {
        mes_pics[b].largeur *= 2;
        mes_pics[b].hauteur *= 2;
    }

    // initialisation des variables de jeu
    game_over = false;
    int fin_scroll = fond->w - SCREEN_W;
    float dragon_speed = 1.0;
    int dragon_acceleration_timer = 0;
    int dragon_malus_timer = 0;
    int jeu_lance = 0;
    int timer_clones = 0;
    int timer_malus_taille = 0;
    float gravite_max = 3.0f;
    float acceleration = 0.6f;

    // affichage initial avant le début du jeu
    clear_bitmap(page);
    blit(fond, page, (int)screenx, 0, 0, 0, SCREEN_W, SCREEN_H);
    dessiner_groupe(&groupe, page);
    textout_centre_ex(page, font, "Appuie sur SPACE pour commencer", SCREEN_W / 2, SCREEN_H - 30, makecol(255, 255, 255), -1);
    blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    clear_keybuf();

    // attente que le joueur appuie sur SPACE
    while (!jeu_lance) {
        poll_keyboard();
        if (key[KEY_ESC]) goto FIN_JEU;
        if (key[KEY_SPACE]) {
            while (key[KEY_SPACE]) poll_keyboard();
            jeu_lance = 1;
        }
        rest(10);
    }

    // configuration du timer à 60 fps
    temps = 0;
    LOCK_VARIABLE(temps);
    LOCK_FUNCTION(temps_init);
    install_int_ex(temps_init, BPS_TO_TIMER(60));

    // boucle principale du niveau
    while (!game_over) {
        poll_keyboard();
        if (key[KEY_ESC]) {
            allegro_exit();
            exit(0);
        }

        // accès au mode démonstration si 1 est pressé
        static int compteur_demo = 1;
        if (key[KEY_1]) {
            Joueur *demo = malloc(sizeof(Joueur));
            sprintf(demo->nom, "DEMO%d", compteur_demo++);
            demo->niveau = 2;
            demo->reprise_x = 4000;
            demo->reprise_y = 600;
            clear_keybuf();
            scrollingNiv2(demo);
            free(demo);
            show_mouse(screen);
            continue;
        }

        // retour au menu avec la touche B
        if (key[KEY_B]) {
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

        // mise à jour du jeu à chaque frame
        while (temps > 0) {
            poll_keyboard();
            int space = key[KEY_SPACE];
            gerer_acceleration(&dragon_speed, &dragon_acceleration_timer, &dragon_malus_timer, space);

            // mise à jour du scrolling
            screenx += dragon_speed;
            if (screenx > fin_scroll) screenx = fin_scroll;

            // vérifie si tous les personnages ont atteint la fin
            int tous_passes = 1;
            for (int i = 0; i < groupe.nb_personnages; i++) {
                int pos_abs = (int)screenx + groupe.persos[i].x;
                if (pos_abs < fond->w) {
                    tous_passes = 0;
                    break;
                }
            }

            // si tous passés, écran de victoire
            if (tous_passes) {
                int choix = ecran_victoire();
                remove_int(temps_init);
                if (choix == 1) {
                    j->niveau = 3;
                    sauvegarder_joueur(j);
                    scrollingNiv3(j);
                } else {
                    ecran_menu();
                }
                return;
            }

            // déplacement des personnages
            deplacer_groupe(&groupe, fond, screenx, fin_scroll, dragon_speed);

            // mise à jour des bonus et pièges
            for (int b = 0; b < NB_BONUS; b++) {
                int bonus_ecran_x = mon_bonus1[b].x - (int)screenx;
                if (bonus_ecran_x > -100 && bonus_ecran_x < SCREEN_W + 100) {
                    deplacement_position_bonus(&mon_bonus1[b], &vitesses_y_pics[b], gravite_max, acceleration, fond, screenx);
                }
            }

            for (int i = 0; i < NB_PICS; i++) {
                deplacement_pic(&mes_caillou[i], &vitesses_y_cailloux[i], gravite_max, acceleration, fond, screenx, &groupe);
                deplacement_pic(&mes_pics[i], &vitesses_y_pics[i], gravite_max, acceleration, fond, screenx, &groupe);
            }

            // gestion des effets de bonus/malus
            gerer_bonus_clones(mon_bonus1, &groupe, screenx, &timer_clones);
            gerer_malus_clones(mon_bonus2, &groupe, screenx);
            gerer_bonus_saut(mon_bonus3, &groupe, screenx, &dragon_acceleration_timer);
            gerer_taille_petit(malust, &groupe, screenx, &timer_malus_taille);
            gerer_collision_pics_groupe(&groupe, fond, screenx);
            gerer_collision_pics_dynamiques(&groupe, mes_pics, screenx);
            gerer_collision_pics_dynamiques(&groupe, mes_caillou, screenx);

            // gestion des checkpoints
            if (collision_checkpoint(&cp, &groupe, &reprise_x, &reprise_y, screenx)) {
                j->reprise_x = cp.x;
                j->reprise_y = cp.y;
                sauvegarder_joueur(j);
            }

            // si tous les personnages sont morts, fin du niveau
            if (groupe_est_mort(&groupe)) {
                game_over = true;
                break;
            }

            // affichage des éléments à l'écran groupe de clones,bonus checkpoint
            int int_screenx = (int)screenx;
            clear_bitmap(page);
            blit(fond, page, int_screenx, 0, 0, 0, SCREEN_W, SCREEN_H);

            for (int b = 0; b < NB_BONUS; b++) {
                afficher_bonus(mon_bonus1[b], page, screenx);
                afficher_bonus_explosion(mon_bonus2[b], page, screenx);
                afficher_bonus(mon_bonus3[b], page, screenx);
                afficher_bonus(malust[b], page, screenx);
            }

            for (int i = 0; i < NB_PICS; i++) {
                afficher_bonus(mes_pics[i], page, screenx);
                afficher_bonus(mes_caillou[i], page, screenx);
            }

            afficher_checkpoint(page, cp, int_screenx);
            dessiner_groupe(&groupe, page);

            temps--;
        }

        blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }

FIN_NIVEAU://gestion de fin de partie
    remove_int(temps_init);
    rest(200);

    // gestion de l'écran de défaite
    if (game_over) {
        int choix = ecran_defaite();// appel de la fonction ecran defaite
        if (choix == 1) {
            j->niveau = 2;
            sauvegarder_joueur(j);
            jeu_niveau_2(fond, j);// on relance
        } else {//sinon retour ecran menu
            ecran_menu();
        }
    } else if (!key[KEY_ESC]) {
        if (j->niveau < 2) {
            j->niveau = 2;
            sauvegarder_joueur(j);
        }
    }

FIN_JEU:// liberation de la memoure
    destroy_bitmap(page);
    destroy_bitmap(fond);
    destroy_bitmap(sprite_bonus);
    destroy_bitmap(sprite_bonus3);
    destroy_bitmap(bombe0);
    destroy_bitmap(bombe1);
    destroy_bitmap(malustaille);
    destroy_bitmap(bonustaille);
}

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
    if (j->niveau == 3 && j->reprise_x == 5300 ) {
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

        if (key[KEY_ESC]) {// quitte le jeu
            allegro_exit();
            exit(0);
        }

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

void gerer_acceleration(float *dragon_speed, int *dragon_acceleration_timer, int *dragon_malus_timer ,int space) {
    if (*dragon_malus_timer > 0) {// si malus vitesse actif
        *dragon_speed = 1;  // vitesse du scroll diminue
        (*dragon_malus_timer)--;//decrementation
    } else if (*dragon_acceleration_timer > 0) {//si bonus vitesse du scroll actif
        *dragon_speed = 7.0;  // vitesse du scroll augmente
        (*dragon_acceleration_timer)--;//decrementation
    } else if (space) {
        *dragon_speed = 3;  // scroll acceleré quand on appuie sur espace
    } else {
        *dragon_speed = 2;  // vitesse normal du jeu
    }
}

