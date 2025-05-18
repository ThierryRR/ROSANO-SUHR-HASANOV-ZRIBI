#include "personnage.h"
#include <stdio.h>
#include "boolean.h"
#include "bonus.h"
void creation_personnage(Personnage *p,int x,int y, int largeur, int hauteur) {
    p->x=x;
    p->y=y;
    p->vy=0;
    p->frame=0;
    p->largeur=largeur;
    p->hauteur=hauteur;
    p->timer_vitesse=0;
    p->timer_colle = 0;
    p->timer_pic = 0;
    p->compteur_espace_colle = 0;
    char sprites[100];
    for(int i=0;i<5;i++) {
        sprintf(sprites, "dragon%d.bmp", i);
        p->sprites[i] = load_bitmap(sprites, NULL);
        if(p->sprites[i]==NULL) {
            allegro_message("erreurvvv");
            exit(1);
        }
    }//sdvrerever
}
void dessiner_personnage(Personnage *p, BITMAP* buffer ) {
    static int compteur_anim = 0;
    static int espacetouche = 0;

    if (key[KEY_SPACE]) {
        espacetouche++;
    } else {
       espacetouche = 0;
    }
    if (!key[KEY_SPACE] && p->vy > 3) {
        stretch_sprite(buffer, p->sprites[3], p->x, p->y, p->largeur, p->hauteur);
        return;
    }

    if (espacetouche >= 5) {
        stretch_sprite(buffer, p->sprites[4], p->x, p->y, p->largeur, p->hauteur);
        return;
    }

    if (key[KEY_SPACE]) {
        compteur_anim++;
        if (compteur_anim >= 12) {
            p->frame = (p->frame + 1) % 3;
            compteur_anim = 0;
        }
    }

    stretch_sprite(buffer, p->sprites[p->frame], p->x, p->y, p->largeur, p->hauteur);
}



int collision_personnage(Personnage* p, BITMAP* fond, float screenx) {
    float offset_x = screenx;
    if ((int)screenx >= fond->w - SCREEN_W) {
        offset_x = fond->w - SCREEN_W;
    }

    for (int dx = 1; dx < p->largeur; dx++) {
        for (int dy = 2; dy < p->hauteur ; dy++) {
            int px = (int)(p->x + dx + offset_x);
            int py = p->y + dy;

            if (px >= 0 && px < fond->w && py >= 0 && py < fond->h) {
                int couleur = getpixel(fond, px, py);
                if (getr(couleur) == 0 && getg(couleur) == 0 && getb(couleur) == 0) {
                    return 1; // décor noir → collision
                }
            }
        }
    }
    return 0;
}


int saut_possible(Personnage* p, BITMAP* fond, float screenx) {
    int old_y = p->y;
    p->y = old_y - 5;  // on simule le saut
    int possible = !collision_personnage(p, fond, screenx);
    p->y = old_y;      // on revient à la position d'origine
    return possible;
}


#include <math.h>  // pour abs()

#include <math.h>  // pour abs()

void deplacer_personnage(Personnage *p, BITMAP *fond, float screenx, int fin_scroll) {
    static int block_timer = 0;
    int old_x = p->x;
    int old_y = p->y;

    if (p->timer_colle > 0) {
        if (p->vy < 6) p->vy += 1;
        p->y += p->vy;

        // 🔒 Reste figé horizontalement à sa position lors du ramassage du bonus colle
        p->x = p->x_colle;

        // Collision sol
        if (collision_personnage(p, fond, screenx)) {
            p->y = old_y;
            p->vy = 0;
        }
        return;
    }

    // 1) Gravité / vol
    if (key[KEY_SPACE]) {
        if (p->vy > -6) p->vy -= 1;
    } else {
        if (p->vy < 6) p->vy += 1;
    }

    // 2) Effets vitesse temporaires
    if (p->timer_vitesse > 0) {
        p->vy *= 1.5;  p->timer_vitesse--;
    } else if (p->timer_vitesse < 0) {
        p->vy *= 0.5;  p->timer_vitesse++;
    }

    // 3) Clamp de la vitesse
    if (p->vy > 8)  p->vy = 8;
    if (p->vy < -8) p->vy = -8;

    // Recul automatique si le dragon est au sol et que la touche espace n'est pas appuyée
    if (!key[KEY_SPACE] && p->vy >= 0) {
        p->y += 2;
        bool au_sol = collision_personnage(p, fond, screenx);
        p->y -= 2;

        if (au_sol) {
            p->x -= 2; // recul vers la gauche
        }
    }


    // 4) Position prévisionnelle
    int tentative_x = p->x;
    int tentative_y = p->y + p->vy;
    if ((int)screenx >= fin_scroll) tentative_x += 2;

    // 5) Test avant déplacement
    p->x = tentative_x;
    p->y = tentative_y;

    // 6) Collision classique
    if (collision_personnage(p, fond, screenx)) {
        p->x = old_x;
        p->y = old_y;
        p->vy = 0;
        int sorti = 0;
        for (int r = 0; r < 10; r++) {
            p->x--; p->y--;
            if (!collision_personnage(p, fond, screenx)) { sorti = 1; break; }
            p->y += 2;
            if (!collision_personnage(p, fond, screenx)) { sorti = 1; break; }
            p->y--;
            if (!collision_personnage(p, fond, screenx)) { sorti = 1; break; }
        }
        if (!sorti) {
            p->x = old_x;
            p->y = old_y;
        }
    }
    // 9) Limites écran
    if (p->y < 0)               { p->y = 0;                  p->vy = 0; }
    if (p->y + p->hauteur > SCREEN_H) {
        p->y = SCREEN_H - p->hauteur;  p->vy = 0;
    }
    if ((int)screenx < fin_scroll && p->x > SCREEN_W - p->largeur) {
        p->x = SCREEN_W - p->largeur;
    }

    // 10) Ajustement sol
    int cs = 0;
    while (collision_personnage(p, fond, screenx) && cs < 20) {
        p->y--; cs++;
    }

    // 11) Ajustement plafond (si espace relâché)
    if (!key[KEY_SPACE]) {
        int cp = 0;
        while (collision_personnage(p, fond, screenx) && cp < 20) {
            p->y++; p->x--; cp++;
        }
    }
}


void dessiner_groupe(GrpPersonnages *g, BITMAP *buffer) {
    for (int i = 0; i < g->nb_personnages; i++) {
        dessiner_personnage(&(g->persos[i]), buffer);
    }
}
void deplacer_groupe(GrpPersonnages *g, BITMAP *fond, float screenx, int fin_scroll, float vitesse_scroll) {
    for (int i = 0; i < g->nb_personnages; i++) {
        deplacer_personnage(&(g->persos[i]), fond, screenx, fin_scroll);


    }
}

bool groupe_est_mort(GrpPersonnages *groupe) {

    int morts = 0;
    for (int i = 0; i < groupe->nb_personnages; i++) {
        if (groupe->persos[i].x + groupe->persos[i].largeur <= 0) {
            morts++;
        }
    }
    return (morts == groupe->nb_personnages);
}
bool collision_pic(Personnage* p, BITMAP* fond, float screenx) {
    float offset_x = screenx;
    if (p->timer_pic > 0) return false;
    if ((int)screenx >= fond->w - SCREEN_W) {
        offset_x = fond->w - SCREEN_W;
    }

    for (int dx = 0; dx < p->largeur; dx++) {
        for (int dy = 0; dy < p->hauteur; dy++) {
            int px = (int)(p->x + dx + offset_x);
            int py = p->y + dy;

            if (px >= 0 && px < fond->w && py >= 0 && py < fond->h) {
                int couleur = getpixel(fond, px, py);
                if (couleur == makecol(104, 0, 0)) {
                    return true;
                }
            }
        }
    }
    return false;
}
void gerer_collision_pics_groupe(GrpPersonnages *groupe, BITMAP *fond, float screenx) {
    for (int i = 0; i < groupe->nb_personnages; i++) {
        Personnage *p = &(groupe->persos[i]);
        if (p->timer_pic > 0) continue;
        float offset_x = screenx;
        if ((int)screenx >= fond->w - SCREEN_W) {
            offset_x = fond->w - SCREEN_W;
        }

        for (int dx = 0; dx < p->largeur; dx++) {
            for (int dy = 0; dy < p->hauteur; dy++) {
                int px = (int)(p->x + dx + offset_x);
                int py = p->y + dy;

                if (px >= 0 && px < fond->w && py >= 0 && py < fond->h) {
                    int couleur = getpixel(fond, px, py);
                    if (couleur == makecol(104, 0, 0)) {

                        p->x = -p->largeur;
                        return;
                    }
                }
            }
        }
    }
}
