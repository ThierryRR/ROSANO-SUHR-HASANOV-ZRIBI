#include <stdio.h>
#include <allegro.h>
#include "menu.h"
#include "scroll.h"
#include "boolean.h"
#include "bonus.h"
#include "personnage.h"
void initialisation_allegro() {
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(desktop_color_depth());
    if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 1920, 1080, 0, 0) != 0) {
        allegro_message("Problème mode graphique");
        allegro_exit();
        exit(EXIT_FAILURE);
    }
    LOCK_VARIABLE(temps);
    LOCK_FUNCTION(temps_init);
}



int main() {
    initialisation_allegro();

    while (!game_over) {
        affichage_ecran_dacceuil();
        if (game_over) break;  // Si appui ESC dans accueil

        ecran_menu();
        if (game_over) break;  // Si appui ESC dans menu
    }

    allegro_exit();
    return 0;
}
END_OF_MAIN();







