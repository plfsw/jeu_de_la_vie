/**
 * \file io_cairo.c
 * \brief Fichier contenant les fonctions io du mode graphique.
 */

#include "io_cairo.h"

void draw_cell_cairo(grille g, grille ga, int i, int j, cairo_surface_t *surface, int vieillissement){
    int x = SIZEX/2-(g.nbc*SIZECELL)/2, y = SIZEY/2-(g.nbl*SIZECELL)/2;
    cairo_t *cr;
    cr = cairo_create(surface);
    
    float colors_r[8] = {150.0/256, 167.0/256, 187.0/256, 202.0/256, 219.0/256, 131.0/256, 109.0/256, 87.0/256};
    float colors_g[8] = {222.0/256, 195.0/256, 167.0/256, 139.0/256, 112.0/256, 67.0/256, 56.0/256, 44.0/256};
    float colors_b[8] = {150.0/256, 167.0/256, 187.0/256, 202.0/256, 219.0/256, 88.0/256, 73.0/256, 58.0/256};
    
    if(g.cellules[i][j] == -1) cairo_set_source_rgb(cr, BROWN);
    else if(vieillissement) cairo_set_source_rgb(cr, colors_r[ga.cellules[i][j]-1], colors_g[ga.cellules[i][j]-1], colors_b[ga.cellules[i][j]-1]);
    else cairo_set_source_rgb(cr, colors_r[0], colors_g[0], colors_b[0]);
    cairo_rectangle(cr, x+(j * SIZECELL)+4, y+(i * SIZECELL)+4, SIZERECT, SIZERECT);
    cairo_fill(cr);
    cairo_destroy(cr);
}

void affichage_read_string_cairo(char t[], cairo_t *cr){
    cairo_set_source_rgb(cr, BLACK);
    cairo_paint(cr);
    cairo_set_source_rgb(cr, GREY);
    cairo_move_to(cr, 5, 10);
    cairo_show_text(cr, "Nouvelle grille: ");
    cairo_move_to(cr, 85, 10);
    cairo_show_text(cr, t);
}
void read_string_cairo(char t[], Display* dpy, XEvent e, cairo_surface_t *surface){
    int lgr;
    char chaine[MAXSTRINGLENGTH];
    cairo_t *cr;
    cr = cairo_create(surface);
    for(int i = 0; i < MAXSTRINGLENGTH; i++) {
        chaine[i] = 0;
        t[i] = 0;
    }
    KeySym touche;
    int i = 0;
    
    while(chaine[0] != 13){
        t[i] = '\0';
        affichage_read_string_cairo(t, cr);
        XNextEvent(dpy, &e);
        if(e.type == KeyPress){
          lgr = XLookupString(&e.xkey, chaine, 20, &touche, 0);
          if (chaine[0] == 8) i--;
          else if(chaine[0] != 0){
              chaine[lgr] = '\0';
              t[i] = chaine[0];
              i++;
        }
      }
    }
    t[i-1] = '\0';
}

void affiche_infos_cairo(cairo_t *cr, int vieillissement, int cyclique, 
    int p,char *per, char* age){
        
    cairo_set_source_rgb(cr, GREY);
    cairo_move_to(cr, 5, 10);
    cairo_show_text(cr, "Jeu de la vie, v0.2");
    
    cairo_move_to(cr, 5, 35);
    if(vieillissement) cairo_show_text(cr, "Vieillissement activé.");
    else cairo_show_text(cr, "Vieillissement désactivé.");
    cairo_move_to(cr, 5, 50);
    if(cyclique) cairo_show_text(cr, "Mode cyclique activé.");
    else cairo_show_text(cr, "Mode cyclique désactivé.");
    
    cairo_move_to(cr, 5, 65);
    cairo_show_text(cr, "Temps d'évolution: ");
    cairo_move_to(cr, 103, 65);
    cairo_show_text(cr, age);
    
    cairo_move_to(cr, 5, 80);
    cairo_show_text(cr, "Periode: ");
    cairo_move_to(cr, 50, 80);
    
    if(p == -1) cairo_show_text(cr, "Soit il n'y a pas d'oscillations, soit la période est trop importante.");
    else if(p == 1) cairo_show_text(cr, "1, la grille est dans un état stable.");
    else if(p > 0) cairo_show_text(cr, per);
    else if(p == 0) cairo_show_text(cr, "Appuyez sur la touche \"o\" pour calculer la periode.");
}

void affiche_grille_cairo(grille g, grille ga, cairo_surface_t *surface, int cyclique, int vieillissement, int p){
    int l=g.nbl, c=g.nbc;
    int x = SIZEX/2-(c*SIZECELL)/2, y = SIZEY/2-(l*SIZECELL)/2;
    char age[10];
    sprintf(age, "%d", g.age);
    
    char per[10];
    sprintf(per, "%d", p);
    
    cairo_t *cr;
    cr = cairo_create(surface);

    // background
    cairo_set_source_rgb(cr, BLACK);
    cairo_paint(cr);
    affiche_infos_cairo(cr, vieillissement, cyclique, p, per, age);
    
    cairo_set_source_rgb(cr, GREY);
    
    for(int i = 0; i <= c; i++){
        cairo_move_to(cr, x, y);
        cairo_line_to(cr, x, y+l*SIZECELL);
        cairo_stroke (cr);
        x += SIZECELL;
    }

    x = SIZEX/2-(c*SIZECELL)/2, y = SIZEY/2-(l*SIZECELL)/2;
    for(int i = 0; i <= l; i++){
        cairo_move_to(cr, x, y);
        cairo_line_to(cr, x+c*SIZECELL, y);
        cairo_stroke (cr);
        y += SIZECELL;
    }
    for(int j = 0; j < c; j++){
      for(int i = 0; i < l; i++){
            if(g.cellules[i][j] != 0) draw_cell_cairo(g, ga, i, j, surface, vieillissement);
        }
    }

    cairo_destroy(cr);

}

void debut_jeu_cairo (grille *g, grille *gc, grille *ga){
// X11 display
    Display *dpy;
    Window rootwin;
    Window win;
    XEvent e;
    int scr;
    void (*pt_evolue)(grille*, grille*, grille*, int (*)(int, int, grille)) = evolue_vi;
    int (*pt_voisins)(int, int, grille) = compte_voisins_vivants_cyclique;
    int lgr;
    KeySym touche;
    int cyclique = 1, vieillissement = 1, p = 0;
    char chaine[MAXSTRINGLENGTH];
    
// init the display
    if(!(dpy=XOpenDisplay(NULL))){
        fprintf(stderr, "ERROR: Could not open display\n");
        exit(1);
    }

    scr = DefaultScreen(dpy);
    rootwin = RootWindow(dpy, scr);

    win = XCreateSimpleWindow(dpy, rootwin, 1, 1, SIZEX, SIZEY, 0,
                              BlackPixel(dpy, scr), BlackPixel(dpy, scr));

    XStoreName(dpy, win, "Game of life");
    XSelectInput(dpy, win, ExposureMask | ButtonPressMask | KeyPressMask );
    XMapWindow(dpy, win);

    // Create cairo surface
    cairo_surface_t *cs;
    cs=cairo_xlib_surface_create(dpy, win, DefaultVisual(dpy, 0), SIZEX, SIZEY);
    
    // boucle principale
	while(1) {
		XNextEvent(dpy, &e);
		if(e.type==Expose && e.xexpose.count<1) {
			affiche_grille_cairo(*g, *ga, cs, cyclique, vieillissement, p) ;
		} else if(e.type==ButtonPress){
			if(e.xbutton.button == 1) pt_evolue(g, gc, ga, pt_voisins);
			else {
                break;
            }
		} 
        else if(e.type==KeyPress){
            lgr = XLookupString(&e.xkey, chaine, 20, &touche, 0);
            chaine[lgr] = 0;
            switch(chaine[0]){
                case 'c':
                { // touche "c" pour basculer d'un mode de comptage à l'autre
                    cyclique = !cyclique;
                    if(cyclique) pt_voisins = &compte_voisins_vivants_cyclique;
                    else pt_voisins = &compte_voisins_vivants_non_cyclique;
                    break;
                }
                case 'v':
                { // touche "v" pour activer/désactiver le vieillissement
                    vieillissement = !vieillissement;
                    if(vieillissement) {
                        pt_evolue = evolue_vi;
                        copie_grille(*g, *ga);
                        ga->age = g->age;
                    }
                    else {
                        pt_evolue = evolue;
                    }
                    break;

                }

                case 'n':
                { // touche "n" pour charger une nouvelle grille.
                    read_string_cairo(chaine, dpy, e, cs);
    
                    libere_grille(g);
                    libere_grille(gc);
                    libere_grille (ga);
                    
                    p = 0;

                    init_grille_from_file(chaine, g);
                    alloue_grille(g->nbl , g->nbc, gc);
                    alloue_grille(g->nbl , g->nbc, ga);
                    copie_grille(*g, *ga);
                    break;
                }
              case 'o':
              { // touche "o" pour calculer la periode de la grille à partir de l'étape en cours.
                p = periode(*g, pt_voisins);
                break;
              }
                default :
                {
                    break;
                }
        }
            
    }
    affiche_grille_cairo(*g, *ga, cs, cyclique, vieillissement, p);
  }

	cairo_surface_destroy(cs); // destroy cairo surface
	XCloseDisplay(dpy); // close the display
	return ;
}
