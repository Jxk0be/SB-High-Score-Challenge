/*	Jake Shoffner
	sb-analyze.cpp
	03/16/22
	Credit: Dr. James Plank (UTK) for using "disjoint_set.hpp" (https://bitbucket.org/jimplank/plank-disjoint-sets/src/master/)
*/
#include "disjoint_set.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <iostream>
#include <vector>
#include <set>
using namespace std;
using plank::Disjoint_Set;

#define talloc(type, num) (type *) malloc(sizeof(type)*(num))

class Superball {
  public:
    Superball(int argc, char **argv);
    int r;
    int c;
    int mss;
    int empty;
    vector <int> board;
    vector <int> goals;
    vector <int> colors;
	void Analyze();
	Disjoint_Set *d;
};

/* My sweet little function */
void Superball::Analyze() {
	d = new Disjoint_Set;
	d->Initialize(r * c);
	vector <int> setTrack;
	setTrack.resize(r * c, 0);

	for (int i = 0; i < r; ++i) {
		for (int j = 0; j < c; ++j) {
			/* Checks every position except last since we don't need to check to the right or below it, makes sure current and next element are not '.' or '*' */
			if (((i * c + j) % c) - (c - 1) != 0 && (i * c + j) < (r * c - 1) && board[i * c + j] != '.' && board[i * c + j] != '*' && board[i * c + j + 1] != '.' && board[i * c + j + 1] != '*') {
				/* Checks if they don't have the same root and are of the same color. If these conditions pass, we know we must then Union() them together */
				if (d->Find(i * c + j) != d->Find(i * c + j + 1) && board[i * c + j] == board[i * c + j + 1]) d->Union(d->Find(i * c + j), d->Find(i * c + j + 1));
			}
			/* Checks every existing row that's bottom exists, also making sure nothing is a '.' or '*' */
			if (((i + 1) * c + j) < (r * c) && board[(i + 1) * c + j] != '.' && board[(i + 1) * c + j] != '*' && board[i * c + j] != '*' && board[i * c + j] != '.') {
				/* Checks if they don't have the same root and are of the same color. If these conditions pass, we know we must then Union() them together */
				if (d->Find(i * c + j) != d->Find((i + 1) * c + j) && board[i * c + j] == board[(i + 1) * c + j]) d->Union(d->Find(i * c + j), d->Find((i + 1) * c + j));
			}
		}
	}

	/* Creating a vector, each index represents an index on the game board, incrementing the index if it is a root set */
	for (unsigned int i = 0; i < board.size(); ++i) {
		if (board[i] != '.' && board[i] != '*') setTrack[d->Find(i)]++;
	}

	/* Printing everything to the screen */
	cout << "Scoring sets:\n";
	
	/* This goes through the entire vector of element counts, if the element counts are above the minimum number to score, we can proceed to run through them and find the index to score on */
	for (int i = 0; i < (int)board.size(); ++i) {
		if (setTrack[i] >= mss) {
			for (unsigned int j = 0; j < goals.size(); ++j) {
				if (d->Find(j) == i && goals[j] == 1) {
					printf("  Size: %-2d  Char: %-1c  Scoring Cell: %d,%d\n", setTrack[i], (char)board[i], j / c, j % c);
					break;
				}
			}
		}
	}
	
	/* Since we made disjoint set a pointer, we have to free up the memory */
	delete d;
}

/* Copied from sb-read */
void usage(const char *s) {
  fprintf(stderr, "usage: sb-analyze rows cols min-score-size colors\n");
  if (s != NULL) fprintf(stderr, "%s\n", s);
  exit(1);
}

/* Copied for sb-read */
Superball::Superball(int argc, char **argv) {
  int i, j;
  string s;

  if (argc != 5) usage(NULL);

  if (sscanf(argv[1], "%d", &r) == 0 || r <= 0) usage("Bad rows");
  if (sscanf(argv[2], "%d", &c) == 0 || c <= 0) usage("Bad cols");
  if (sscanf(argv[3], "%d", &mss) == 0 || mss <= 0) usage("Bad min-score-size");

  colors.resize(256, 0);

  for (i = 0; i < (int)strlen(argv[4]); i++) {
    if (!isalpha(argv[4][i])) usage("Colors must be distinct letters");
    if (!islower(argv[4][i])) usage("Colors must be lowercase letters");
    if (colors[argv[4][i]] != 0) usage("Duplicate color");
    colors[argv[4][i]] = 2+i;
    colors[toupper(argv[4][i])] = 2+i;
  }

  board.resize(r*c);
  goals.resize(r*c, 0);

  empty = 0;

  for (i = 0; i < r; i++) {
    if (!(cin >> s)) {
      fprintf(stderr, "Bad board: not enough rows on standard input\n");
      exit(1);
    }
    if ((int)s.size() != c) {
      fprintf(stderr, "Bad board on row %d - wrong number of characters.\n", i);
      exit(1);
    }
    for (j = 0; j < c; j++) {
      if (s[j] != '*' && s[j] != '.' && colors[s[j]] == 0) {
        fprintf(stderr, "Bad board row %d - bad character %c.\n", i, s[j]);
        exit(1);
      }
      board[i*c+j] = s[j];
      if (board[i*c+j] == '.') empty++;
      if (board[i*c+j] == '*') empty++;
      if (isupper(board[i*c+j]) || board[i*c+j] == '*') {
        goals[i*c+j] = 1;
        board[i*c+j] = tolower(board[i*c+j]);
      }
    }
  }
}

int main(int argc, char **argv) {
	Superball *s;
	int ngoal, tgoal;
	s = new Superball(argc, argv);
	tgoal = 0;
	ngoal = 0;

	for (int i = 0; i < s->r*s->c; i++) {
		if (s->goals[i] && s->board[i] != '*') {
			tgoal += s->colors[s->board[i]];
			ngoal++;
		}
	}
	
	/* Calling the function and freeing the memory of the Superball object created */
	s->Analyze();
	delete s;

	return 0;
}