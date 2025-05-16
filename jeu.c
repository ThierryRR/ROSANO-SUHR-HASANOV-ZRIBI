#include <stdio.h>
#include <allegro.h>
#include "jeu.h"
#include "menu.h"
#include "personnage.h"
#include "boolean.h"
#include "scroll.h"
#include "bonus.h"
#define NB_PICS 3
void jeu_niveau_1(BITMAP *fond_final, Joueur *j) {
    BITMAP *fond = copier_bitmap(fond_final);
    BITMAP *page = create_bitmap(SCREEN_W, SCREEN_H);

    if ( !fond || !page ) {
        allegro_message("Erreur de chargement des ressources.");
        exit(1);
    }
    GrpPersonnages groupe;
    groupe.nb_personnages = 1;

    // 🔥 Protection : ne PAS écraser les coordonnées si c'est un DEMO
    if (strncmp(j->nom, "DEMO", 4) != 0) {
        j->reprise_x = 3200;
        j->reprise_y = 500;
    }

    int reprise_x = j->reprise_x;
    int reprise_y = j->reprise_y;
    float screenx = reprise_x - SCREEN_W / 2;
    if (screenx < 0) screenx = 0;
    if (screenx > fond->w - SCREEN_W) screenx = fond->w - SCREEN_W;
    int perso_x = reprise_x - (int)screenx;


    creation_personnage(&groupe.persos[0], perso_x, reprise_y, 64, 64);

    checkpoint cp = creer_checkpoint(600, 330, "drapeau0.bmp", "drapeau1.bmp");
    cp.largeur = cp.sprite[0]->w / 12;
    cp.hauteur = cp.sprite[0]->h / 12;

    game_over = false;
    int fin_scroll = fond->w - SCREEN_W;
    float dragon_speed = 1.0;
    int dragon_acceleration_timer = 0;
    int dragon_malus_timer = 0;
    int jeu_lance = 0;


    clear_bitmap(page);
    blit(fond, page, (int)screenx, 0, 0, 0, SCREEN_W, SCREEN_H);
    dessiner_groupe(&groupe, page);
    textout_centre_ex(page, font, "Appuie sur SPACE pour commencer", SCREEN_W / 2, SCREEN_H - 30, makecol(255, 255, 255), -1);
    blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    clear_keybuf();

    while (!jeu_lance) {
        poll_keyboard();
        if (key[KEY_ESC]) goto FIN_JEU;
        if (key[KEY_SPACE]) {
            while (key[KEY_SPACE]) poll_keyboard();
            jeu_lance = 1;
        }
        rest(10);
    }

    temps = 0;
    LOCK_VARIABLE(temps);
    LOCK_FUNCTION(temps_init);
    install_int_ex(temps_init, BPS_TO_TIMER(60));

    while (!game_over) {
        poll_keyboard();

        if (key[KEY_ESC]) {
            allegro_exit();
            exit(0);
        }
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
///ddddddddddd
        while (temps > 0) {
            poll_keyboard();
            int space = key[KEY_SPACE];
            gerer_acceleration(&dragon_speed, &dragon_acceleration_timer, &dragon_malus_timer, space);

            screenx += dragon_speed;
            if (screenx > fin_scroll) screenx = fin_scroll;
            for (int i = 0; i < groupe.nb_personnages; i++) {
                int pos_abs = (int)screenx + groupe.persos[i].x;
                if (pos_abs >= fond->w) {
                    int choix = ecran_victoireniv1();
                    if (choix == 1) {
                        j->niveau = 2;
                        sauvegarder_joueur(j);
                        scrollingNiv2(j);
                    } else {
                        ecran_menu();
                    }
                    goto FIN_NIVEAU;
                }
            }   static int compteur_demo = 1;
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
            deplacer_groupe(&groupe, fond, screenx, fin_scroll ,dragon_speed );


            if (collision_checkpoint(&cp, &groupe, &reprise_x, &reprise_y, screenx)) {
                j->reprise_x = cp.x;
                j->reprise_y = cp.y;
                sauvegarder_joueur(j);
            }

            if (groupe_est_mort(&groupe)) game_over = true;

            int int_screenx = (int)screenx;
            int part1 = fond->w - int_screenx;
            int part2 = SCREEN_W - part1;
            clear_bitmap(page);
            if (part1 >= SCREEN_W) {
                blit(fond, page, int_screenx, 0, 0, 0, SCREEN_W, SCREEN_H);
            } else {
                blit(fond, page, int_screenx, 0, 0, 0, part1, SCREEN_H);
                blit(fond, page, 0, 0, part1, 0, part2, SCREEN_H);
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
        int choix = ecran_defaiteniv1();
        if (choix == 1) jeu_niveau_1(fond, j);
        else if (choix == 2) ecran_menu();
    } else if (!key[KEY_ESC]) {
        if (j->niveau < 2) {
            j->niveau = 2;
            sauvegarder_joueur(j);
        }
    }

FIN_JEU:
    destroy_bitmap(page);
    destroy_bitmap(fond);

}


void jeu_niveau_2(BITMAP *fond_final, Joueur *j) {
    BITMAP *fond = copier_bitmap(fond_final);
    BITMAP *page = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP *sprite_bonus = load_bitmap("1.bmp", NULL);
    BITMAP *sprite_bonus3 = load_bitmap("2.bmp", NULL);
    BITMAP *bombe0 = load_bitmap("bombe0.bmp", NULL);
    BITMAP *bombe1 = load_bitmap("bombe1.bmp", NULL);
    BITMAP *malusvitesse = load_bitmap("4.bmp", NULL);
    BITMAP *malustaille = load_bitmap("malus.bmp", NULL);
    BITMAP *bonustaille = load_bitmap("baloo.bmp", NULL);
    BITMAP *bonuscomportement = load_bitmap("5.bmp", NULL);
    BITMAP *bonuscomport = load_bitmap("6.bmp", NULL);
    BITMAP *sprite_pic = load_bitmap("pic.bmp", NULL);
    BITMAP *caillou=load_bitmap("caillou.bmp", NULL);
    if (!caillou||!sprite_pic||!bonuscomport || !bonuscomportement || !fond || !page || !sprite_bonus || !sprite_bonus3 || !bombe0 || !bombe1 || !malusvitesse || !malustaille || !bonustaille) {
        allegro_message("Erreur de chargement des ressources.");
        exit(1);
    }
    // Seulement si c'est une nouvelle partie sans sauvegarde
    if (strncmp(j->nom, "DEMO", 4) != 0 && j->reprise_x == 0 && j->reprise_y == 0) {
        j->reprise_x = 3200;
        j->reprise_y = 500;
    }


    BonusPosition mon_bonus1[NB_BONUS] = {//niv2
        creer_bonus(6300, 300, sprite_bonus, NULL),
        creer_bonus(300, 250, sprite_bonus, NULL),
        creer_bonus(1500, 700, sprite_bonus, NULL)
    };
    BonusPosition mon_bonus2[NB_BONUS] = {//niv2
        creer_bonus(1000, 700, bombe0, bombe1),
        creer_bonus(3300, 500, bombe0, bombe1),
        creer_bonus(4000, 280, bombe0, bombe1)
    };
    BonusPosition mon_bonus3[NB_BONUS] = {//nv2
        creer_bonus(400, 700, sprite_bonus3, NULL),
        creer_bonus(4390, 600, sprite_bonus3, NULL),
        creer_bonus(1800, 680, sprite_bonus3, NULL),
        creer_bonus(7800, 680, sprite_bonus3, NULL),
        creer_bonus(12500, 500, sprite_bonus3, NULL),

    };

    BonusPosition malust[NB_BONUS] = {//niv2
        creer_bonus(400, 800, malustaille, NULL),
        creer_bonus(3600, 250, malustaille, NULL),
        creer_bonus(3600, 780, malustaille, NULL),
        creer_bonus(6700, 490, malustaille, NULL),
    };

    BonusPosition mes_pics[NB_PICS]={
        mes_pics[0] = creer_bonus(2425, 305, sprite_pic, NULL) , // x=200, y=100
        mes_pics[1] = creer_bonus(10000, 45, sprite_pic, NULL) , // x=1000, y=50
        mes_pics[2] = creer_bonus(4300, 340, sprite_pic, NULL) // x=1800, y=200
    }; // à déclarer et initialiser dans le jeu
BonusPosition mes_caillou[NB_PICS]={
    mes_caillou[0] = creer_bonus(200, 100, caillou, NULL) , // x=200, y=100
mes_caillou[1] = creer_bonus(1900, 230, caillou, NULL) , // x=1000, y=50
mes_caillou[2] = creer_bonus(3800, 200, caillou, NULL) // x=1800, y=200
};
    GrpPersonnages groupe;
    groupe.nb_personnages = 1;


    if (j->reprise_x == 0 && j->reprise_y == 0) {
        j->reprise_x = 200;  // coordonnées de base si pas de checkpoint
        j->reprise_y = 600;
    }

    int reprise_x = j->reprise_x;
    int reprise_y = j->reprise_y;

    float screenx = reprise_x - SCREEN_W / 2;
    if (screenx < 0) screenx = 0;
    if (screenx > fond->w - SCREEN_W) screenx = fond->w - SCREEN_W;
    int perso_x = reprise_x - (int)screenx;
    float vitesses_y_pics[NB_PICS] = {0};

    creation_personnage(&groupe.persos[0], perso_x, reprise_y, 64, 64);

    checkpoint cp = creer_checkpoint(5300, 690, "drapeau0.bmp", "drapeau1.bmp");
    cp.largeur = cp.sprite[0]->w / 6;
    cp.hauteur = cp.sprite[0]->h / 6;

    float vitesses_y_cailloux[NB_PICS] = {0};
    for (int b = 0; b < NB_BONUS; b++) {
        mon_bonus2[b].largeur = bombe0->w / 12;
        mon_bonus2[b].hauteur = bombe0->h / 12;
        mon_bonus3[b].largeur = sprite_bonus3->w / 12;
        mon_bonus3[b].hauteur = sprite_bonus3->h / 12;

    }
    for (int b = 0; b < NB_PICS; b++) {
        mes_pics[b].largeur *= 2;
        mes_pics[b].hauteur *= 2;
    }

    game_over = false;
    int fin_scroll = fond->w - SCREEN_W;
    float dragon_speed = 1.0;
    int dragon_acceleration_timer = 0;
    int dragon_malus_timer = 0;
    int jeu_lance = 0;
    int timer_clones = 0;
    int timer_malus_taille = 0;
    float gravite_max = 2.0f;
    float acceleration = 0.2f;


    clear_bitmap(page);
    blit(fond, page, (int)screenx, 0, 0, 0, SCREEN_W, SCREEN_H);
    dessiner_groupe(&groupe, page);
    textout_centre_ex(page, font, "Appuie sur SPACE pour commencer", SCREEN_W / 2, SCREEN_H - 30, makecol(255, 255, 255), -1);
    blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    clear_keybuf();

    while (!jeu_lance) {
        poll_keyboard();
        if (key[KEY_ESC]) goto FIN_JEU;
        if (key[KEY_SPACE]) {
            while (key[KEY_SPACE]) poll_keyboard();
            jeu_lance = 1;
        }
        rest(10);
    }

    temps = 0;
    LOCK_VARIABLE(temps);
    LOCK_FUNCTION(temps_init);
    install_int_ex(temps_init, BPS_TO_TIMER(60));

    while (!game_over) {
        poll_keyboard();

        if (key[KEY_ESC]) {
            allegro_exit();
            exit(0);

        }
        static int compteur_demo=1;
        if (key[KEY_1]) {


            Joueur *demo = malloc(sizeof(Joueur));
            sprintf(demo->nom, "DEMO%d", compteur_demo++);
            demo->niveau = 1;
            demo->reprise_x = 4000;
            demo->reprise_y = 600;

            clear_keybuf();
            scrollingNiv2(demo);

            free(demo);
            show_mouse(screen);
            continue;
        }

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

        while (temps > 0) {
            poll_keyboard();
            int space = key[KEY_SPACE];
            gerer_acceleration(&dragon_speed, &dragon_acceleration_timer, &dragon_malus_timer, space);

            screenx += dragon_speed;
            if (screenx > fin_scroll) screenx = fin_scroll;
            for (int i = 0; i < groupe.nb_personnages; i++) {
                int pos_abs = (int)screenx + groupe.persos[i].x;
                if (pos_abs >= fond->w) {
                    int choix = ecran_victoireniv1();
                    if (choix == 1) {
                        j->niveau = 2;
                        sauvegarder_joueur(j);
                        scrollingNiv2(j);
                    } else {
                        ecran_menu();
                    }
                    goto FIN_NIVEAU;
                }
            }

            deplacer_groupe(&groupe, fond, screenx, fin_scroll,dragon_speed );
            // Appelle TOUTES les gestions de bonus classiques

            for (int b = 0; b < NB_BONUS; b++) {
                int bonus_ecran_x = mon_bonus1[b].x - (int)screenx;

                // Ne déplacer le bonus que s’il est à l’écran (avec marge de 100 pixels)
                if (bonus_ecran_x > -100 && bonus_ecran_x < SCREEN_W + 100) {
                    deplacement_position_bonus(&mon_bonus1[b], &vitesses_y_pics[b], gravite_max, acceleration, fond, screenx);
                }
            }for (int i = 0; i < NB_PICS; i++) {
                deplacement_pic(&mes_caillou[i],&vitesses_y_cailloux[i],gravite_max, acceleration, fond, screenx,&groupe); deplacement_pic(&mes_pics[i], &vitesses_y_pics[i], gravite_max, acceleration, fond, screenx,&groupe);
            }
            gerer_bonus_clones(mon_bonus1, &groupe, screenx, &timer_clones);
            gerer_malus_clones(mon_bonus2, &groupe, screenx);
            gerer_bonus_saut(mon_bonus3, &groupe, screenx, &dragon_acceleration_timer);
            gerer_taille_petit(malust, &groupe, screenx, &timer_malus_taille);
            gerer_collision_pics_groupe(&groupe, fond, screenx);
            gerer_collision_pics_dynamiques(&groupe, mes_pics, screenx);
            gerer_collision_pics_dynamiques(&groupe, mes_caillou, screenx);

            if (collision_checkpoint(&cp, &groupe, &reprise_x, &reprise_y, screenx)) {
                j->reprise_x = cp.x;
                j->reprise_y = cp.y;
                sauvegarder_joueur(j);
            }

            if (groupe_est_mort(&groupe)) game_over = true;

            int int_screenx = (int)screenx;
            clear_bitmap(page);
            blit(fond, page, int_screenx, 0, 0, 0, SCREEN_W, SCREEN_H);
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

FIN_NIVEAU:
    remove_int(temps_init);
    rest(200);
    if (game_over) {
        int choix = ecran_defaiteniv1();
        if (choix == 1) jeu_niveau_2(fond, j);
        else if (choix == 2) ecran_menu();
    } else if (!key[KEY_ESC]) {
        if (j->niveau < 2) {
            j->niveau = 2;
            sauvegarder_joueur(j);
        }
    }

FIN_JEU:
    destroy_bitmap(page);
    destroy_bitmap(fond);
    destroy_bitmap(sprite_bonus);
    destroy_bitmap(sprite_bonus3);
    destroy_bitmap(bombe0);
    destroy_bitmap(bombe1);
    destroy_bitmap(malusvitesse);
    destroy_bitmap(malustaille);
    destroy_bitmap(bonustaille);
}


void jeu_niveau_3(BITMAP *fond_final, Joueur *j) {
    BITMAP *fond = copier_bitmap(fond_final);
    BITMAP *page = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP *sprite_bonus = load_bitmap("1.bmp", NULL);
    BITMAP *sprite_bonus3 = load_bitmap("2.bmp", NULL);
    BITMAP *bombe0 = load_bitmap("bombe0.bmp", NULL);
    BITMAP *bombe1 = load_bitmap("bombe1.bmp", NULL);
    BITMAP *malusvitesse = load_bitmap("4.bmp", NULL);
    BITMAP *malustaille = load_bitmap("malus.bmp", NULL);
    BITMAP *bonustaille = load_bitmap("baloo.bmp", NULL);
    BITMAP *bonuscomportement = load_bitmap("5.bmp", NULL);
    BITMAP *bonuscomport = load_bitmap("6.bmp", NULL);
    BITMAP *sprite_pic = load_bitmap("pic.bmp", NULL);
    BITMAP *caillou=load_bitmap("caillou.bmp", NULL);
    if (!caillou||!sprite_pic||!bonuscomport || !bonuscomportement || !fond || !page || !sprite_bonus || !sprite_bonus3 || !bombe0 || !bombe1 || !malusvitesse || !malustaille || !bonustaille) {
        allegro_message("Erreur de chargement des ressources.");
        exit(1);
    }
    if (strncmp(j->nom, "DEMO", 4) != 0) {
        j->reprise_x = 800;
        j->reprise_y = 500;
    }

    BonusPosition mon_bonus1[NB_BONUS] = {//niv2
        creer_bonus(6300, 300, sprite_bonus, NULL),
        creer_bonus(300, 250, sprite_bonus, NULL),
        creer_bonus(1500, 700, sprite_bonus, NULL)
    };
    BonusPosition mon_bonus2[NB_BONUS] = {//niv2
        creer_bonus(1000, 700, bombe0, bombe1),
        creer_bonus(3300, 500, bombe0, bombe1),
        creer_bonus(4000, 280, bombe0, bombe1)
    };
    BonusPosition mon_bonus3[NB_BONUS] = {//nv2
        creer_bonus(400, 300, sprite_bonus3, NULL),
        creer_bonus(4390, 200, sprite_bonus3, NULL),
        creer_bonus(1800, 480, sprite_bonus3, NULL)
    };
    BonusPosition mon_bonus4[NB_BONUS] = {
        creer_bonus(1200, 300, malusvitesse, NULL),
        creer_bonus(4350, 200, malusvitesse, NULL),
        creer_bonus(1000, 280, malusvitesse, NULL)
    };
    BonusPosition malust[NB_BONUS] = {//niv2
        creer_bonus(400, 800, malustaille, NULL),
        creer_bonus(3500, 250, malustaille, NULL),
        creer_bonus(3100, 280, malustaille, NULL)
    };
    BonusPosition mon_bonus5[NB_BONUS] = {
        creer_bonus(5000, 500, bonuscomportement, NULL),
        creer_bonus(4500, 250, bonuscomportement, NULL),
        creer_bonus(3100, 280, bonuscomportement, NULL)
    };
    BonusPosition mon_bonus6[NB_BONUS] = {
        creer_bonus(200, 700, bonuscomport, NULL),
        creer_bonus(3520, 500, bonuscomport, NULL),
        creer_bonus(4101, 700, bonuscomport, NULL)
    };
    BonusPosition bonust[NB_BONUS] = {
        creer_bonus(700, 300, bonustaille, NULL),
        creer_bonus(3600, 250, bonustaille, NULL),
        creer_bonus(4200, 280, bonustaille, NULL)
    };
    BonusPosition mes_pics[NB_PICS]={
        mes_pics[0] = creer_bonus(1900, 400, sprite_pic, NULL) , // x=200, y=100
        mes_pics[1] = creer_bonus(1000, 50, sprite_pic, NULL) , // x=1000, y=50
        mes_pics[2] = creer_bonus(4000, 200, sprite_pic, NULL) // x=1800, y=200
    }; // à déclarer et initialiser dans le jeu
BonusPosition mes_caillou[NB_PICS]={
    mes_caillou[0] = creer_bonus(200, 100, caillou, NULL) , // x=200, y=100
mes_caillou[1] = creer_bonus(2100, 50, caillou, NULL) , // x=1000, y=50
mes_caillou[2] = creer_bonus(3800, 200, caillou, NULL) // x=1800, y=200
};

    GrpPersonnages groupe;
    groupe.nb_personnages = 1;
    if (j->reprise_x == 0 && j->reprise_y == 0) {
        j->reprise_x = 200;  // coordonnées de base si pas de checkpoint
        j->reprise_y = 600;
    }

    int reprise_x = j->reprise_x;
    int reprise_y = j->reprise_y;

    float screenx = reprise_x - SCREEN_W / 2;
    if (screenx < 0) screenx = 0;
    if (screenx > fond->w - SCREEN_W) screenx = fond->w - SCREEN_W;
    int perso_x = reprise_x - (int)screenx;
    float vitesses_y_pics[NB_PICS] = {0};

    creation_personnage(&groupe.persos[0], perso_x, reprise_y, 64, 64);

    checkpoint cp = creer_checkpoint(600, 330, "drapeau0.bmp", "drapeau1.bmp");
    cp.largeur = cp.sprite[0]->w / 12;
    cp.hauteur = cp.sprite[0]->h / 12;

    float vitesses_y_cailloux[NB_PICS] = {0};
    for (int b = 0; b < NB_BONUS; b++) {
        mon_bonus2[b].largeur = bombe0->w / 12;
        mon_bonus2[b].hauteur = bombe0->h / 12;
        mon_bonus3[b].largeur = sprite_bonus3->w / 12;
        mon_bonus3[b].hauteur = sprite_bonus3->h / 12;

    }
    for (int b = 0; b < NB_PICS; b++) {
        mes_pics[b].largeur *= 2;
        mes_pics[b].hauteur *= 2;
    }

    game_over = false;
    int fin_scroll = fond->w - SCREEN_W;
    float dragon_speed = 1.0;
    int dragon_acceleration_timer = 0;
    int dragon_malus_timer = 0;
    int jeu_lance = 0;
    int timer_clones = 0;
    int timer_malus_taille = 0;
    int timer_bonus_taille = 0;
    float vitesse_y_bonus = 0.0f;
    float gravite_max = 2.0f;
    float acceleration = 0.2f;

    int bonus_colle_timer=0;
    // Écran de démarrage
    clear_bitmap(page);
    blit(fond, page, (int)screenx, 0, 0, 0, SCREEN_W, SCREEN_H);
    dessiner_groupe(&groupe, page);
    textout_centre_ex(page, font, "Appuie sur SPACE pour commencer", SCREEN_W / 2, SCREEN_H - 30, makecol(255, 255, 255), -1);
    blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    clear_keybuf();

    while (!jeu_lance) {
        poll_keyboard();
        if (key[KEY_ESC]) goto FIN_JEU;
        if (key[KEY_SPACE]) {
            while (key[KEY_SPACE]) poll_keyboard();
            jeu_lance = 1;
        }
        rest(10);
    }

    temps = 0;
    LOCK_VARIABLE(temps);
    LOCK_FUNCTION(temps_init);
    install_int_ex(temps_init, BPS_TO_TIMER(60));

    while (!game_over) {
        poll_keyboard();

        if (key[KEY_ESC]) {
            allegro_exit();
            exit(0);

        }
        static int compteur_demo=1;
        if (key[KEY_1]) {


            Joueur *demo = malloc(sizeof(Joueur));
            sprintf(demo->nom, "DEMO%d", compteur_demo++);
            demo->niveau = 1;
            demo->reprise_x = 3300;
            demo->reprise_y = 300;

            clear_keybuf();
            scrollingNiv2(demo);

            free(demo);
            show_mouse(screen);
            continue;
        }

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

        while (temps > 0) {
            poll_keyboard();
            int space = key[KEY_SPACE];
            gerer_acceleration(&dragon_speed, &dragon_acceleration_timer, &dragon_malus_timer, space);

            screenx += dragon_speed;
            if (screenx > fin_scroll) screenx = fin_scroll;
            for (int i = 0; i < groupe.nb_personnages; i++) {
                int pos_abs = (int)screenx + groupe.persos[i].x;
                if (pos_abs >= fond->w) {
                    int choix = ecran_victoireniv1();
                    if (choix == 1) {
                        j->niveau = 2;
                        sauvegarder_joueur(j);
                        scrollingNiv2(j);
                    } else {
                        ecran_menu();
                    }
                    goto FIN_NIVEAU;
                }
            }

            deplacer_groupe(&groupe, fond, screenx, fin_scroll,dragon_speed );
            // Appelle TOUTES les gestions de bonus classiques
            gerer_bonus_clones(mon_bonus1, &groupe, screenx, &timer_clones);
            gerer_malus_clones(mon_bonus2, &groupe, screenx);
            gerer_bonus_saut(mon_bonus3, &groupe, screenx, &dragon_acceleration_timer);
           gerer_malus_vitesse(mon_bonus4, &groupe, screenx, &dragon_malus_timer);
            gerer_taille_petit(malust, &groupe, screenx, &timer_malus_taille);
            gerer_taille_grand(bonust, &groupe, screenx, &timer_bonus_taille);
            gerer_bonus_colle(mon_bonus6, &groupe, screenx);;
gerer_bonus_colle(mon_bonus6, &groupe, screenx);

            for (int b = 0; b < NB_BONUS; b++) {
                int bonus_ecran_x = mon_bonus1[b].x - (int)screenx;

                // Ne déplacer le bonus que s’il est à l’écran (avec marge de 100 pixels)
                if (bonus_ecran_x > -100 && bonus_ecran_x < SCREEN_W + 100) {
                    deplacement_position_bonus(&mon_bonus1[b], &vitesses_y_pics[b], gravite_max, acceleration, fond, screenx);
                }
            }for (int i = 0; i < NB_PICS; i++) {
                deplacement_pic(&mes_caillou[i],&vitesses_y_cailloux[i],gravite_max, acceleration, fond, screenx,&groupe); deplacement_pic(&mes_pics[i], &vitesses_y_pics[i], gravite_max, acceleration, fond, screenx,&groupe);
            }



            gerer_bonus_immunite_pic(mon_bonus5, &groupe, screenx);
            gerer_collision_pics_groupe(&groupe, fond, screenx);
            gerer_collision_pics_dynamiques(&groupe, mes_pics, screenx);
            gerer_collision_pics_dynamiques(&groupe, mes_caillou, screenx);

            if (collision_checkpoint(&cp, &groupe, &reprise_x, &reprise_y, screenx)) {
                j->reprise_x = cp.x;
                j->reprise_y = cp.y;
                sauvegarder_joueur(j);
            }

            if (groupe_est_mort(&groupe)) game_over = true;

            int int_screenx = (int)screenx;
            clear_bitmap(page);
            blit(fond, page, int_screenx, 0, 0, 0, SCREEN_W, SCREEN_H);
            clear_bitmap(page);
            blit(fond, page, int_screenx, 0, 0, 0, SCREEN_W, SCREEN_H);

            for (int b = 0; b < NB_BONUS; b++) {
                afficher_bonus(mon_bonus1[b], page, screenx);
                afficher_bonus_explosion(mon_bonus2[b], page, screenx);
                afficher_bonus(mon_bonus3[b], page, screenx);
                afficher_bonus(mon_bonus4[b], page, screenx);
                afficher_bonus(mon_bonus5[b], page, screenx);
                afficher_bonus(mon_bonus6[b], page, screenx);
                afficher_bonus(malust[b], page, screenx);
                afficher_bonus(bonust[b], page, screenx);

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

FIN_NIVEAU:
    remove_int(temps_init);
    rest(200);
    if (game_over) {
        int choix = ecran_defaiteniv1();
        if (choix == 1) jeu_niveau_2(fond, j);
        else if (choix == 2) ecran_menu();
    } else if (!key[KEY_ESC]) {
        if (j->niveau < 2) {
            j->niveau = 2;
            sauvegarder_joueur(j);
        }
    }

FIN_JEU:
    destroy_bitmap(page);
    destroy_bitmap(fond);
    destroy_bitmap(sprite_bonus);
    destroy_bitmap(sprite_bonus3);
    destroy_bitmap(bombe0);
    destroy_bitmap(bombe1);
    destroy_bitmap(malusvitesse);
    destroy_bitmap(malustaille);
    destroy_bitmap(bonustaille);
}

void gerer_acceleration(float *dragon_speed, int *dragon_acceleration_timer, int *dragon_malus_timer ,int space) {
    if (*dragon_malus_timer > 0) {
        *dragon_speed = 1;  // vitesse de scroll réduite
        (*dragon_malus_timer)--;
    } else if (*dragon_acceleration_timer > 0) {
        *dragon_speed = 7.0;  // vitesse de scroll boostée
        (*dragon_acceleration_timer)--;
    } else if (space) {
        *dragon_speed = 3;  // scroll légèrement accéléré à la main
    } else {
        *dragon_speed = 2;  // vitesse de scroll normale
    }
}
