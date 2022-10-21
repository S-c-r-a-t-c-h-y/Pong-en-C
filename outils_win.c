#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <io.h>
#include <fcntl.h>
#include <wchar.h>

#define UTF_16_ENCODING 0x00020000
#define NORMAL_ENCODING 0x4000

#define BOMBE -1
#define DRAPEAU -2
#define VIDE 0
#define PAS_DECOUVERT -3
#define DECOUVERT -4

#define CLEAR_SCREEN "cls"

const char vide = ' ';
const int bombe = 0x2699;
const int drapeau = 0x2691;
const int pas_decouvert = 0x2610;
// U+2612

/* Les spécifications des fonctions se trouvent dans le fichier outils_win.h */

void print_unicode(int unicode_code)
{
    // défini l'encodage de la console en UTF-16
    _setmode(_fileno(stdout), UTF_16_ENCODING);

    const wchar_t character = unicode_code;
    wprintf(L"%c", character);

    // remet l'encodage de la console comme normal
    _setmode(_fileno(stdout), NORMAL_ENCODING);
}

void clear_screen()
{
    system(CLEAR_SCREEN);
}

void affiche_tableau(int **tab, int hauteur, int largeur)
{
    for (int i = 0; i < hauteur; i++)
    {
        for (int j = 0; j < largeur; j++)
        {
            printf(" ");
            switch (tab[i][j])
            {
            case BOMBE:
                print_unicode(bombe);
                break;
            case DRAPEAU:
                print_unicode(drapeau);
                break;
            case VIDE:
                printf("%c", vide);
                break;
            case PAS_DECOUVERT:
                print_unicode(pas_decouvert);
                break;
            default:
                printf("%d", tab[i][j]);
                break;
            }
        }
        printf("\n");
    }
}

int **creer_tableau(int m, int n)
{
    int **matrice = (int **)malloc(sizeof(int *) * m);
    if (matrice == NULL)
    {
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < m; i++)
    {
        matrice[i] = (int *)malloc(sizeof(int) * n);
        if (matrice[i] == NULL)
        {
            exit(EXIT_FAILURE);
        }
    }

    return matrice;
}

void initialiser_tableau_courant(int **tab, int m, int n)
{
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            tab[i][j] = PAS_DECOUVERT;
        }
    }
}

void initialiser_tableau_solution(int **tab, int m, int n, int nombre_bombes)
{
    // on vérifie que le nombre de bombes est bien inférieur au nombre de cases du tableau
    assert(nombre_bombes < m * n);

    srand(time(NULL));
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            tab[i][j] = VIDE;
        }
    }

    int x;
    int y;

    // place les bombes dans la grille
    for (int k = 0; k < nombre_bombes; k++)
    {
        x = rand() % n;
        y = rand() % m;
        while (tab[y][x] == BOMBE)
        {
            x = rand() % n;
            y = rand() % m;
        }
        tab[y][x] = BOMBE;
    }

    // calcul les nombres pour toutes les autres cases
    for (y = 0; y < m; y++)
    {
        for (x = 0; x < n; x++)
        {
            if (tab[y][x] == BOMBE)
            {
                continue;
            }
            int bombes_voisines = 0;
            for (int i = y - 1; i < y + 2; i++)
            {
                if (i < 0 || i >= m)
                {
                    continue;
                }
                for (int j = x - 1; j < x + 2; j++)
                {
                    if (j < 0 || j >= n || (i == 0 && j == 0))
                    {
                        continue;
                    }
                    if (tab[i][j] == BOMBE)
                    {
                        bombes_voisines += 1;
                    }
                }
            }
            tab[y][x] = bombes_voisines;
        }
    }
}

void _decouvrir_case(int **tableau_courant, int **tableau_solution, int m, int n, int y, int x, int iteration, int *cases_decouvertes)
{
    int valeur = tableau_solution[y][x];

    // la case à découvrir est une bombe
    if (valeur == BOMBE)
    {
        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (tableau_solution[i][j] == BOMBE)
                {
                    tableau_courant[i][j] = BOMBE;
                }
            }
        }

        *cases_decouvertes = -1;
        return;
    }

    // la case à découvrir est un drapeau
    if (valeur == DRAPEAU)
    {
        *cases_decouvertes = 0;
        return;
    }

    // la case à découvrir est un chiffre
    if (valeur != VIDE)
    {
        if (tableau_courant[y][x] != valeur)
        {
            tableau_courant[y][x] = valeur;
            *cases_decouvertes = (*cases_decouvertes) + 1;
        }
        return;
    }

    // la case à découvrir est vide
    if (tableau_courant[y][x] == DECOUVERT)
    {
        return;
    }
    tableau_courant[y][x] = DECOUVERT;
    *cases_decouvertes = (*cases_decouvertes) + 1;
    for (int i = y - 1; i < y + 2; i++)
    {
        if (i < 0 || i >= m)
        {
            continue;
        }
        for (int j = x - 1; j < x + 2; j++)
        {
            if (j < 0 || j >= n)
            {
                continue;
            }
            _decouvrir_case(tableau_courant, tableau_solution, m, n, i, j, iteration + 1, cases_decouvertes);
        }
    }
    if (iteration == 0)
    {
        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (tableau_courant[i][j] == DECOUVERT)
                {
                    tableau_courant[i][j] = VIDE;
                }
            }
        }
    }
}

int decouvrir_case(int **tableau_courant, int **tableau_solution, int m, int n, int y, int x)
{
    assert(x >= 0 && x < n && y >= 0 && y < m);
    int cases_decouvertes = 0;
    _decouvrir_case(tableau_courant, tableau_solution, m, n, y, x, 0, &cases_decouvertes);
    return cases_decouvertes;
}

void liberer_tableau(int **tab, int m)
{
    for (int i = 0; i < m; i++)
    {
        free(tab[i]);
    }
    free(tab);
}
