#include "personnage.h"
#include <stdio.h>
#include "boolean.h"
#include "bonus.h"

void creation_personnage(Personnage *p,int x,int y, int largeur, int hauteur) {
    // prends la valeur mis en parametres dans jeu
    p->x=x;
    p->y=y;
    // initialisation de la vitrsse a zero
    p->vy=0;
    // affichage du premier sprite (sprite 0)
    p->frame=0;
    //definition de la largeur mit en parametre
    p->largeur=largeur;
    p->hauteur=hauteur;
    // initialisation des timers et parametres des bonus
    p->timer_vitesse=0;
    p->timer_colle = 0;
    p->timer_pic = 0;
    p->compteur_espace_colle = 0;

    char sprites[100];
    for(int i=0;i<5;i++) {
        sprintf(sprites, "dragon%d.bmp", i); // chargement des sprites
        p->sprites[i] = load_bitmap(sprites, NULL);
        if(p->sprites[i]==NULL) {
            allegro_message("erreur");
            exit(1);
        }
    }
}
void dessiner_personnage(Personnage *p, BITMAP* buffer ) {
    static int compteur_anim = 0;// compteur animation
    static int espacetouche = 0;// compteur de touche espace appuye

    if (key[KEY_SPACE]) {// incrementation du compteur
        espacetouche++;
    } else {
       espacetouche = 0;// remise a zero
    }
    if (!key[KEY_SPACE] && p->vy > 3) {
        stretch_sprite(buffer, p->sprites[3], p->x, p->y, p->largeur, p->hauteur); // affichage en chute
        return;
    }

    if (espacetouche >= 5) {
        stretch_sprite(buffer, p->sprites[4], p->x, p->y, p->largeur, p->hauteur); // affichage en vol
        return;
    }

    if (key[KEY_SPACE]) {// delay entrre chaque animation
        compteur_anim++;
        if (compteur_anim >= 12) {
            p->frame = (p->frame + 1) % 3;
            compteur_anim = 0;
        }
    }

    stretch_sprite(buffer, p->sprites[p->frame], p->x, p->y, p->largeur, p->hauteur); // animation de base
}



int collision_personnage(Personnage* p, BITMAP* fond, float screenx) {
    //corrige le scroll si on atteint le bord droit
    float offset_x = screenx;
    if ((int)screenx >= fond->w - SCREEN_W) {
        offset_x = fond->w - SCREEN_W;
    }
// verfication de chaque pixel
    for (int dx = 1; dx < p->largeur; dx++) {
        for (int dy = 2; dy < p->hauteur ; dy++) {
            int px = (int)(p->x + dx + offset_x);
            int py = p->y + dy;

            if (px >= 0 && px < fond->w && py >= 0 && py < fond->h) {
                int couleur = getpixel(fond, px, py);
                if (getr(couleur) == 0 && getg(couleur) == 0 && getb(couleur) == 0) { // colisions avec les obstacles noir
                    return 1;
                }
                if (getr(couleur) == 104 && getg(couleur) == 0 && getb(couleur) == 0 && p->timer_pic > 0){
                    return 1; // collisions avec les obstacles noir quand le bonus est actif
                }
            }
        }
    }
    return 0;
}


int saut_possible(Personnage* p, BITMAP* fond, float screenx) {
    int old_y = p->y;
    p->y = old_y - 5;  // on vérifie si la position où on veut aller est dispo
    int possible = !collision_personnage(p, fond, screenx);
    p->y = old_y;
    return possible;
}

void deplacer_personnage(Personnage *p, BITMAP *fond, float screenx, int fin_scroll) {
    int old_x = p->x;  // sauvergarde l'ancienne position
    int old_y = p->y;
    if (p->timer_colle > 0) { // si bonus acitf, gravité changé
        if (p->vy < 6) p->vy += 1;
        p->y += p->vy;
        p->x = p->x_colle;
        if (collision_personnage(p, fond, screenx)) { // collisions ?
            p->y = old_y; // on revient à la position
            p->vy = 0; // on met à 0 la vitesse
        }
        return;
    }
    if (key[KEY_SPACE]) { // Si espace appuyé
        if (p->vy > -6) p->vy -= 1; // perso monte
    } else {
        if (p->vy < 6) p->vy += 1; // perso tombe
    }
    if (p->timer_vitesse > 0) {
        p->vy *= 1.5; // vitesse rapide
        p->timer_vitesse--;
    } else if (p->timer_vitesse < 0) {
        p->vy *= 0.5; // vitesse lente
        p->timer_vitesse++;
    }
    if (p->vy > 8)  p->vy = 8;
    if (p->vy < -8) p->vy = -8;

    if (!key[KEY_SPACE] && p->vy >= 0) { // recul si perso au sol sans rien
        p->y += 2;
        bool au_sol = collision_personnage(p, fond, screenx);
        p->y -= 2;

        if (au_sol) {
            p->x -= 2; //perso recule à gauche
        }
    }
    int tentative_x = p->x;
    int tentative_y = p->y + p->vy;
    if ((int)screenx >= fin_scroll) tentative_x += 2; // perso avance si en fin de scroll

    p->x = tentative_x;
    p->y = tentative_y;

    if (collision_personnage(p, fond, screenx)) {
        p->x = old_x; // revient à l'ancienne position
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

    if (p->y < 0) { // trop haut
        p->y = 0;
        p->vy = 0;
    }
    if (p->y + p->hauteur > SCREEN_H) { // trop bas
        p->y = SCREEN_H - p->hauteur;
        p->vy = 0;
    }
    if ((int)screenx < fin_scroll && p->x > SCREEN_W - p->largeur) {
        p->x = SCREEN_W - p->largeur;
    }

    int cs = 0;
    while (collision_personnage(p, fond, screenx) && cs < 20) {
        p->y--; cs++; // perso remonte si colisions avec le bas
    }

    if (!key[KEY_SPACE]) {
        int cp = 0;
        while (collision_personnage(p, fond, screenx) && cp < 20) {
            p->y++; p->x--; cp++; // perso redescend si coincé au plafond
        }
    }
}


void dessiner_groupe(GrpPersonnages *g, BITMAP *buffer) {
    for (int i = 0; i < g->nb_personnages; i++) {
        dessiner_personnage(&(g->persos[i]), buffer); // affichage tous les perso du bonus
    }
}
void deplacer_groupe(GrpPersonnages *g, BITMAP *fond, float screenx, int fin_scroll, float vitesse_scroll) {
    for (int i = 0; i < g->nb_personnages; i++) {
        deplacer_personnage(&(g->persos[i]), fond, screenx, fin_scroll); // déplace tous les perso indépendamment


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
// gère la collision entre chaque personnage et les pics (supprime s’il touche)
void gerer_collision_pics_groupe(GrpPersonnages *groupe, BITMAP *fond, float screenx) {
    float offset_x = (int)screenx >= fond->w - SCREEN_W ? fond->w - SCREEN_W : screenx;

    for (int i = 0; i < groupe->nb_personnages; i++) {
        Personnage *p = &(groupe->persos[i]);

        if (p->timer_pic > 0) continue; // immunisé

        for (int dx = 0; dx < p->largeur; dx++) {
            for (int dy = 0; dy < p->hauteur; dy++) {
                int px = (int)(p->x + dx + offset_x);
                int py = p->y + dy;

                if (px >= 0 && px < fond->w && py >= 0 && py < fond->h) {
                    if (getpixel(fond, px, py) == makecol(104, 0, 0)) {
                        // Supprimer le personnage du tableau
                        for (int j = i; j < groupe->nb_personnages - 1; j++) {
                            groupe->persos[j] = groupe->persos[j + 1];
                        }
                        groupe->nb_personnages--; // décrémenter
                        i--; // reculer l'indice car on a décalé les éléments
                        goto next_personnage; // sortir proprement des boucles imbriquées
                    }
                }
            }
        }

        next_personnage:;
    }
}
