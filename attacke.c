/*************************************************************
**         Europäisches Institut für Systemsicherheit        *
**         Praktikum "Kryptoanalyse"                         *
**                                                           *
**   Versuch 2: Permutations-Chiffre                         *
**                                                           *
**************************************************************
**
** attacke.c: Implementierung einer Permutations-Chiffre
**            Rahmenprogramm zur Lösung
**/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <ctype.h>

#include "libperm.h"

#define PERIODE 20
#define CHIFFRAT "chiffrat"
#define PERMUTATION "permutation"
#define LOESUNG "klartext"

int loesung[PERIODE];

int laenge;
char *chiffrat;

char *scratch1, *scratch2;

void attacke (void);

int main (void)
{
  FILE *f;

  f = fopen (CHIFFRAT, "r");
  if (! f) {
    perror ("fopen");
    fprintf (stderr, "Konnte Datei %s nicht oeffnen\n", CHIFFRAT);
    exit (2);
  }
  fseek (f, 0, SEEK_END);
  laenge = ftell (f);
  rewind (f);
  chiffrat = malloc (laenge);
  scratch1 = malloc (laenge);
  scratch2 = malloc (laenge);
  if (! chiffrat || ! scratch1 || ! scratch2) {
    fprintf (stderr, "Konnte Puffer nicht allozieren\n");
    exit (2);
  }
  if (fread (chiffrat, 1, laenge, f) != laenge) {
    fprintf (stderr, "Fehler beim einlesen der Datei %s\n", CHIFFRAT);
    exit (2);
  }
  fclose (f);

  {
    attacke ();
  }
  if (writeperm (PERMUTATION, PERIODE, loesung) < 0) {
    fprintf (stderr, "Fehler beim Schreiben der Loesung auf Datei %s\n",
             PERMUTATION);
    exit (2);
  }
  printf ("Nun kannst Du versuchen, die Datei mit dem Befehl:\n");
  printf ("  decrypt %s %s %s\n", PERMUTATION, CHIFFRAT, LOESUNG);
  printf ("zu entschluesseln, um zu sehen, ob die Loesung stimmt.\n");
  exit (0);
}

/**
  Checks whether char1 preceeds char2.
  output 1: char1 is whitespace and char2 is uppercase
  output 0: else
*/
int check_chars(int char1, int char2) 
{
  if (isspace(char1) && isupper(char2)) {
    return 1;
  }
  return 0;
}

void attacke (void)
{
	/* *** Hier soll die Attacke implementiert werden *** */
	/* Globale Variablen:
	*   laenge         Laenge des Chiffrats
	*   chiffrat       Puffer, in dem das Chiffrat vorliegt
	*   scratch1  und
	*   scratch2       2 Puffer der Laenge 'laenge', die beliebig verwendet
	*                  werden koennen (char *)
	*   loesung        int loesung[PERIODE], dort sollte nach dem Ende
	*                  dieser Funktion die gesuchte Permutation stehen!
	*   PERIODE        In diesem #define steht die Periodenlaenge, die
	*                  in diesem Versuch benutzt wurde.
	*/

  /* Aufgabe */

  int A[PERIODE][PERIODE] = {0};
  int max[PERIODE] = {0};
  int blocks = laenge / PERIODE;
  // compare i with all j in all k blocks
  for (int i = 0; i < PERIODE; i++) {
    for (int j = 0; j < PERIODE; j++) { 
      if (i != j) {
        for (int k = 0; k < blocks; k++) {
          A[i][j] += check_chars(chiffrat[i + k*PERIODE], chiffrat[j + k*PERIODE]);
        }
      }
    }
  }
  for (int i = 0; i < PERIODE; i++) {
    for (int j = 0; j < PERIODE; j++) {
      printf("%02i ", A[i][j]);
    }
    printf("\n");
  }
  // search the max in every column
  int tmp_max = 0;
  printf("\nSearching Max of every column...");
  for (int i = 0; i < PERIODE; i++) {
    tmp_max = 0;
    for (int j = 0; j < PERIODE; j++) {
      if (tmp_max < A[j][i]) {
        tmp_max = A[j][i];
      }
    }
    max[i] = tmp_max;
  }

  printf("\n");
  for (int i = 0; i < PERIODE; i++) {
    printf("%02i ", max[i]);
  }
  printf("\n");

  // search for the min in max[] to find the first entry of the solution
  loesung[0] = 0;
  for (int i = 0; i < PERIODE; i++) {
    if (max[loesung[0]] > max[i]) {
      loesung[0] = i;
    }
  }
  printf("Min is: %02i in max[%02i]\n", max[loesung[0]], loesung[0]);

  // find the rest inductive
  for (int i = 0; i < PERIODE-1; i++) {

    printf("\nSearching max now in row: %02i\n", loesung[i]);
    for (int k = 0; k < PERIODE; k++) {
      printf("%02i ", A[loesung[i]][k]);
    }
    printf("\n");

    tmp_max = 0;
    for (int j = 0; j < PERIODE; j++) {
      if (tmp_max < A[loesung[i]][j]) {
        tmp_max = A[loesung[i]][j];
        loesung[i+1] = j;
      }
    }
    printf("Max is: %02i in max[%02i]\n", A[loesung[i]][loesung[i+1]], loesung[i+1]);
  }

  printf("\nDIE LÖSUNG IST: \n");
  for (int i = 0; i < PERIODE; i++) {
    printf("%02i ", loesung[i]);
  }
  printf("\n\n");

}