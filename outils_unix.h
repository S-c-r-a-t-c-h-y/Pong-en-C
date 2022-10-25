void clear_screen();
void affiche_tableau(int **tab, int hauteur, int largeur);
int **creer_tableau(int m, int n);
void initialiser_tableau_courant(int **tab, int m, int n);
void initialiser_tableau_solution(int **tab, int m, int n, int nombre_bombes);
void liberer_tableau(int **tab, int m);
int decouvrir_case(int **tableau_courant, int **tableau_solution, int m, int n, int y, int x);
int jeu_fini(int **tab_sol, **tab_courant, int m, int n);
void action_clavier(int **tab_sol, int **tab, int m, int n, int *position, int *ancienne_var, char key_pressed, int *death_wave);