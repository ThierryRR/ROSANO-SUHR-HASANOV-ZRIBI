#include <stdio.h>
#include <allegro.h>
#include "menu.h"
#include "scroll.h"
#include "boolean.h"
#include "bonus.h"
#include "personnage.h"
// initialise allegro et configure le mode graphique en plein écran
void initialisation_allegro() {
    allegro_init(); // initialise la bibliothèque allegro
    install_keyboard(); // active la gestion du clavier
    install_mouse(); // active la gestion de la souris
    set_color_depth(desktop_color_depth()); // définit la profondeur de couleur selon le bureau

    // essaie de passer en mode plein écran 1920x1080
    if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 1920, 1080, 0, 0) != 0) {
        allegro_message("Problème mode graphique"); // affiche un message d’erreur si échec
        allegro_exit(); // quitte allegro proprement
        exit(EXIT_FAILURE); // termine le programme
    }

    // verrouille les variables pour l’utilisation avec les interruptions
    LOCK_VARIABLE(temps);
    LOCK_FUNCTION(temps_init);
}

// fonction principale du programme
int main() {
    initialisation_allegro(); // configure allegro et l’écran

    // boucle principale du jeu tant que le jeu n'est pas terminé
    while (!game_over) {

        affichage_ecran_dacceuil(); // affiche l'écran d'accueil
        if (game_over) break; // si l'utilisateur a quitté depuis l’accueil

        ecran_menu(); // affiche le menu principal
        if (game_over) break; // si l'utilisateur quitte depuis le menu
    }

    allegro_exit(); // libère les ressources allegro
    return 0; // fin normale du programme
}

END_OF_MAIN(); // macro requise par allegro pour initialiser le point d’entrée du programme




