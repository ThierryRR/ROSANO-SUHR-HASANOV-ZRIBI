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
        sprintf(sprites, "dragon%d.bmp", i); // Chargement des sprites
        p->sprites[i] = load_bitmap(sprites, NULL);
        if(p->sprites[i]==NULL) {
            allegro_message("erreurvvv");
            exit(1);
        }
    }
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
        stretch_sprite(buffer, p->sprites[3], p->x, p->y, p->largeur, p->hauteur); // Chute
        return;
    }

    if (espacetouche >= 5) {
        stretch_sprite(buffer, p->sprites[4], p->x, p->y, p->largeur, p->hauteur); // Vol
        return;
    }

    if (key[KEY_SPACE]) {
        compteur_anim++;
        if (compteur_anim >= 12) {
            p->frame = (p->frame + 1) % 3;
            compteur_anim = 0;
        }
    }

    stretch_sprite(buffer, p->sprites[p->frame], p->x, p->y, p->largeur, p->hauteur); // Animation de base
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
                if (getr(couleur) == 0 && getg(couleur) == 0 && getb(couleur) == 0) { // Colisions avec les obstacles noir
                    return 1;
                }
                if (getr(couleur) == 104 && getg(couleur) == 0 && getb(couleur) == 0 && p->timer_pic > 0){
                    return 1; // Collisions avec les obstacles noir quand le bonus est actif
                }
            }
        }
    }
    return 0;
}


int saut_possible(Personnage* p, BITMAP* fond, float screenx) {
    int old_y = p->y;
    p->y = old_y - 5;  // On vérifie si la position où on veut aller est dispo
    int possible = !collision_personnage(p, fond, screenx);
    p->y = old_y;
    return possible;
}

void deplacer_personnage(Personnage *p, BITMAP *fond, float screenx, int fin_scroll) {
    int old_x = p->x;  // Sauvergarde l'ancienne position
    int old_y = p->y;
    if (p->timer_colle > 0) { // Si bonus acitf, gravité changé
        if (p->vy < 6) p->vy += 1;
        p->y += p->vy;
        p->x = p->x_colle;
        if (collision_personnage(p, fond, screenx)) { // Collisions ?
            p->y = old_y; // On revient à la position
            p->vy = 0; // On met à 0 la vitesse
        }
        return;
    }
    if (key[KEY_SPACE]) { // Si espace appuyé
        if (p->vy > -6) p->vy -= 1; // Perso monte
    } else {
        if (p->vy < 6) p->vy += 1; // Perso tombe
    }
    if (p->timer_vitesse > 0) {
        p->vy *= 1.5; // Vitesse rapide
        p->timer_vitesse--;
    } else if (p->timer_vitesse < 0) {
        p->vy *= 0.5; // Vitesse lente
        p->timer_vitesse++;
    }
    if (p->vy > 8)  p->vy = 8;
    if (p->vy < -8) p->vy = -8;

    if (!key[KEY_SPACE] && p->vy >= 0) { // Recul si perso au sol sans rien
        p->y += 2;
        bool au_sol = collision_personnage(p, fond, screenx);
        p->y -= 2;

        if (au_sol) {
            p->x -= 2; //Perso recule à gauche
        }
    }
    int tentative_x = p->x;
    int tentative_y = p->y + p->vy;
    if ((int)screenx >= fin_scroll) tentative_x += 2; // Perso avance si en fin de scroll

    p->x = tentative_x;
    p->y = tentative_y;

    if (collision_personnage(p, fond, screenx)) {
        p->x = old_x; // Revient à l'ancienne position
        p->y = old_y;
        p->vy = 0;
        int sorti = 0;
        for (int r = 0; r < 10; r++) {
            p->x--; p->y--;
            if (!collision_personnage(p, fond, screenx)) {
                sorti = 1; break;
            }
            p->y += 2;
            if (!collision_personnage(p, fond, screenx)) {
                sorti = 1; break;
            }
            p->y--;
            if (!collision_personnage(p, fond, screenx)) {
                sorti = 1; break;
            }
        }
        if (!sorti) {
            p->x = old_x;
            p->y = old_y;
        }
    }

    if (p->y < 0) { // Trop haut
        p->y = 0;
        p->vy = 0;
    }
    if (p->y + p->hauteur > SCREEN_H) { // Trop bas
        p->y = SCREEN_H - p->hauteur;
        p->vy = 0;
    }
    if ((int)screenx < fin_scroll && p->x > SCREEN_W - p->largeur) {
        p->x = SCREEN_W - p->largeur;
    }

    int cs = 0;
    while (collision_personnage(p, fond, screenx) && cs < 20) {
        p->y--; cs++; // Perso remonte si colisions avec le bas
    }

    if (!key[KEY_SPACE]) {
        int cp = 0;
        while (collision_personnage(p, fond, screenx) && cp < 20) {
            p->y++; p->x--; cp++; // Perso redescend si coincé au plafond
        }
    }
}


void dessiner_groupe(GrpPersonnages *g, BITMAP *buffer) {
    for (int i = 0; i < g->nb_personnages; i++) {
        dessiner_personnage(&(g->persos[i]), buffer); // Afichage tous les perso du bonus
    }
}
void deplacer_groupe(GrpPersonnages *g, BITMAP *fond, float screenx, int fin_scroll, float vitesse_scroll) {
    for (int i = 0; i < g->nb_personnages; i++) {
        deplacer_personnage(&(g->persos[i]), fond, screenx, fin_scroll); // Déplace tous les perso indépendamment


    }
}

bool groupe_est_mort(GrpPersonnages *groupe) { // Vérifie si tous les perso du groupe sont morts
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
    if (p->timer_pic > 0) // Si bonus actif, on ignore les pics rouge
        return false;
    if ((int)screenx >= fond->w - SCREEN_W) {
        offset_x = fond->w - SCREEN_W;
    }
    for (int dx = 0; dx < p->largeur; dx++) {
        for (int dy = 0; dy < p->hauteur; dy++) {
            int px = (int)(p->x + dx + offset_x);
            int py = p->y + dy;
            if (px >= 0 && px < fond->w && py >= 0 && py < fond->h) {
                int couleur = getpixel(fond, px, py);
                if (couleur == makecol(104, 0, 0)) { // Si détecte pixel rouge
                    return true; // Pic mortel
                }
            }
        }
    }
    return false;
}
void gerer_collision_pics_groupe(GrpPersonnages *groupe, BITMAP *fond, float screenx) {
    for (int i = 0; i < groupe->nb_personnages; i++) {
        Personnage *p = &(groupe->persos[i]);
        if (p->timer_pic > 0)
            continue;
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
