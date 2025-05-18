
#include <allegro.h>
#include "menu.h"
#include "stdio.h"
#include "scroll.h"
#include "boolean.h"
#include "joueur.h"

//variable globale de fin de jeu
bool game_over = false;

void affichage_ecran_dacceuil() {// fonction qui affiche affichage ecran d'acceuil
    show_mouse(NULL);//masque la souris
    BITMAP *image = load_bitmap("badlandecran1.bmp", NULL);//chargement de l'image + verification
    if (image == NULL) {
        allegro_message("erreur");
        exit(1);
    }
//affichage de l'image
    blit(image, screen, 0, 0, 0, 0, image->w, image->h);
    clear_keybuf();//vide les touches en mémoire
//boucle d'attente de la touche espace ou echa
    while (!key[KEY_SPACE] && !key[KEY_ESC]) {
        poll_keyboard();// mise a jour des touches du clavier
        rest(10);// legere pause pour pas surcharger l'ordinateur
    }
//liberation de l'image ,efface l'ecran,vide le buffer du clavier
    destroy_bitmap(image);
    clear_to_color(screen, makecol(0, 0, 0));
    clear_keybuf();

    if (key[KEY_ESC]) {
        game_over = true;  // on arrête le jeu
    }
    // sinon le jeu continue
}
void ecran_menu() {//fonction du lenu principal du jeu
    Joueur *j = NULL;//pointeur sur le joueur courant
    BITMAP *image = load_bitmap("badlandmenu.bmp", NULL);//chargement du fond de l'image
    BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);//double buffer

    if (!image || !buffer) {//verification
        allegro_message("erreur");
        if (image) destroy_bitmap(image);
        if (buffer) destroy_bitmap(buffer);
        return;
    }

    game_over = false;//rénitialisation du game_over
    static int compteur_demo = 1;//compteur pour creer des joueurs de test pour notre jeu

    while (!key[KEY_ESC] && !game_over) {
        poll_keyboard();
//affichage de la souris de limage sur le buffer
        clear_bitmap(buffer);
        blit(image, buffer, 0, 0, 0, 0, image->w, image->h);
        show_mouse(buffer);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        show_mouse(NULL);//cache la souris apres laffichage

     //touche 1 lance un joueur demo niv 1
        if (key[KEY_1]) {
            Joueur *demo = malloc(sizeof(Joueur));
            if (!demo) continue;
            sprintf(demo->nom, "DEMO%d", compteur_demo++);
            demo->niveau = 1;
            demo->reprise_x = 75;
            demo->reprise_y = 300;

            destroy_bitmap(buffer);
            destroy_bitmap(image);
            clear_keybuf();
            scrollingNiv1(demo);//lance le niveau 1
            free(demo);// libere la memoire
            return;
        }     if (key[KEY_2]) {//touche 2 lance niv 2
            Joueur *demo = malloc(sizeof(Joueur));
            if (!demo) continue;
            sprintf(demo->nom, "DEMO%d", compteur_demo++);
            demo->niveau = 2;
            demo->reprise_x = 200;
            demo->reprise_y = 600;
            destroy_bitmap(buffer);
            destroy_bitmap(image);
            clear_keybuf();
            scrollingNiv2(demo);//lance le niveau 2
            free(demo);// libere la memoire
            return;
        }     if (key[KEY_3]) {
            Joueur *demo = malloc(sizeof(Joueur));
            if (!demo) continue;
            sprintf(demo->nom, "DEMO%d", compteur_demo++);
            demo->niveau = 3;
            demo->reprise_x = 200;
            demo->reprise_y = 700;

            destroy_bitmap(buffer);
            destroy_bitmap(image);
            clear_keybuf();
            scrollingNiv3(demo);//lance le niveau 3
            free(demo);// libere la memoire
            return;
        }
//appuie sur tab retourn à l'ecran d'acceuil
        if (key[KEY_TAB]) {
            clear_keybuf();
            show_mouse(NULL);

            destroy_bitmap(buffer);
            destroy_bitmap(image);

            // retour l'écran d'accueil
            affichage_ecran_dacceuil();
            clear_keybuf();
            return;
        }



        // clic gauche de la souris permet de gerer les touches
        if (mouse_b & 1) {;
           if ((mouse_x >= 305) && (mouse_x <= 1611) && (mouse_y >= 646) && (mouse_y <= 778)) {//zone nouveau joueur
                Joueur *temp = nouveau_joueur(screen);
                if (temp != NULL) {
                    destroy_bitmap(buffer);
                    destroy_bitmap(image);
                    scrollingNiv1(temp);//lance le niv 1
                    free(temp);
                    return;
                } else {
                    clear_keybuf();

                    continue;//reaffiche le menu
                }
            }

// zone chargement joueur
             if ((mouse_x >= 236) && (mouse_x <= 1676) && (mouse_y >= 817) && (mouse_y <= 942 )) {

                j = chargement_du_joueur(screen);//chargement du fichier texte
                if (j != NULL) {
                    //mance le niveau correspondant
                    if (j->niveau == 1) scrollingNiv1(j);
                    else if (j->niveau == 2) scrollingNiv2(j);
                    else scrollingNiv3(j);
                    // retour menu
                } else {

                    clear_keybuf();
                    continue;  //si erreur retour menu
                }
            }
        }

        rest(10);
    }
// liberation du buffer et de l'image
    destroy_bitmap(buffer);
    destroy_bitmap(image);
    clear_to_color(screen, makecol(0, 0, 0));//nettoyage de l'ecran
}

int ecran_victoire() {// fonction qui affiche si le joueur sort de lecran a droite
    BITMAP *image = load_bitmap("ecran_victoire.bmp", NULL);// initialisation puis verification
    if (!image) {
        allegro_message("erreur");
        exit(1);
    }

    BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);//double buffer
    if (!buffer) {
        destroy_bitmap(image);
        allegro_message("erreur");
        exit(1);
    }

    show_mouse(screen);//affichage de la souris sur lecran

    while (!key[KEY_ESC]) {//boucle tant quon appuie pas sur espace
        blit(image, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);//affichage d el'image

        show_mouse(buffer);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        show_mouse(NULL);

        if (mouse_b & 1) {
        //zone qui amene au niv suivant

            if ((mouse_x >= 173) && (mouse_x <= 1746) && (mouse_y >= 510) && (mouse_y <= 677)) {
                destroy_bitmap(image);
                destroy_bitmap(buffer);
                rest(300);
                return 1;//zone qui retoune au menu
            } else if ((mouse_x >= 175) && (mouse_x <= 1739) && (mouse_y >= 737) && (mouse_y <= 910)) {
                destroy_bitmap(image);
                destroy_bitmap(buffer);
                rest(300);
                return 2;
            }
        }
        rest(10);
    }

//liberation mémoire
    destroy_bitmap(image);
    destroy_bitmap(buffer);
    return 2;
}
int ecran_defaite() {//fonction qui s'active quand on quitte l'ecran par la gauche
    BITMAP *image = load_bitmap("ecran_defaite.bmp", NULL);//initialisation puis verification
    if (!image) {
        allegro_message("erreur.bmp");
        exit(1);
    }

    BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);// creation et verifiaction du buffer
    if (!buffer) {
        destroy_bitmap(image);
        allegro_message("erreurr");
        exit(1);
    }

    show_mouse(screen);//affichage de la souris

    while (!key[KEY_ESC]) {
        blit(image, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);// affichage de l'image sur le buffer

        show_mouse(buffer);// affichage de la souris sur le buffer
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);// double buffer
        show_mouse(NULL);// remise a null

        if (mouse_b & 1) {// zone pour relancer le niveau

            if ((mouse_x >= 356) && (mouse_x <= 1444) && (mouse_y >= 324) && (mouse_y <= 537 )) {
                destroy_bitmap(image);
                destroy_bitmap(buffer);
                rest(300);
                return 1;
            } else if ((mouse_x >= 318) && (mouse_x <=1473) && (mouse_y >= 602) && (mouse_y <= 808)) {// zone retour au menu
                destroy_bitmap(image);
                destroy_bitmap(buffer);
                rest(300);
                return 2;
            }
        }
        rest(10);
    }
// liberation mémoire

    destroy_bitmap(image);
    destroy_bitmap(buffer);
    return 2;
}


