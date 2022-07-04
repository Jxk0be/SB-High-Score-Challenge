/*	Jake Shoffner
	sb-play.cpp
	03/16/22
	Credit: Dr. James Plank (UTK) for using "disjoint_set.hpp" (https://bitbucket.org/jimplank/plank-disjoint-sets/src/master/)
*/

#include "disjoint_set.hpp"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <vector>
#include <map>
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
	vector <int> indices, temp;
	map <int, vector <int> > ratings;
	map <int, vector <int> > score;
	int counter = 0;
	Disjoint_Set *d;
	
	void Analyze();
	void Play();
	void Choose(int index, int n);
};

void Superball::Play() {
	/* Indices that can be SWAP'd */
	for (unsigned int i = 0; i < board.size(); ++i) {
		if (board[i] != '.' && board[i] != '*') indices.push_back(i);
	}
	
	/* SCORE when the amount of spaces left are less than the minimum needed to score */
	if (board.size() - indices.size() < (unsigned int)mss) {
		Analyze();
	}
	else {
		/* We want to enumerate all indices that are valid colors, 2 indices to eventually SWAP */
		Choose(0, 2);
		
		/* SWAP output */
		if (!ratings.empty()) cout << "SWAP " << ratings.rbegin()->second[0] / c << ' ' << ratings.rbegin()->second[0] % c << ' ' << ratings.rbegin()->second[1] / c << ' ' << ratings.rbegin()->second[1] % c << '\n';
	}

	/* Hard-coded last SWAP in order to end the game */
	cout << "SWAP 0 0 1 0\n";
}

/* Stolen N Choose K enumeration from lecture notes */
void Superball::Choose(int index, int n) {
	if (n == 0) {
		int holder = 0;
		
		/* Swapping the indices */
		holder = board[temp[0]];
		board[temp[0]] = board[temp[1]];
		board[temp[1]] = holder;
		
		/* Calling Analyze() to show us possible scoring cells for this enumeration's SWAP */
		Analyze();
	
		/* Pushing back the positions that we SWAP'd to get the max set of this enumeration */
		ratings[counter].push_back(temp[0]);
		ratings[counter].push_back(temp[1]);
		
		/* Swapping back to the original board for next enumeration's SWAP */
		holder = board[temp[0]];
		board[temp[0]] = board[temp[1]];
		board[temp[1]] = holder;
		
		return;
	}

	if (n > (int)indices.size() - index) return;

	temp.push_back(indices[index]);
	Choose(index + 1, n - 1);
	temp.pop_back();

	Choose(index + 1, n);
}

/* My sweet little function, but with some augmentations from the last part of the lab */
void Superball::Analyze() {
	d = new Disjoint_Set;
	d->Initialize(r * c);
	vector <int> setTrack;
	setTrack.resize(r * c, 0);

	for (int i = 0; i < r; ++i) {
		for (int j = 0; j < c; ++j) {
			if (((i * c + j) % c) - (c - 1) != 0 && (i * c + j) < (r * c - 1) && board[i * c + j] != '.' && board[i * c + j] != '*' && board[i * c + j + 1] != '.' && board[i * c + j + 1] != '*') {
				if (d->Find(i * c + j) != d->Find(i * c + j + 1) && board[i * c + j] == board[i * c + j + 1]) d->Union(d->Find(i * c + j), d->Find(i * c + j + 1));
			}
			if (((i + 1) * c + j) < (r * c) && board[(i + 1) * c + j] != '.' && board[(i + 1) * c + j] != '*' && board[i * c + j] != '*' && board[i * c + j] != '.') {
				if (d->Find(i * c + j) != d->Find((i + 1) * c + j) && board[i * c + j] == board[(i + 1) * c + j]) d->Union(d->Find(i * c + j), d->Find((i + 1) * c + j));
			}
		}
	}

	for (unsigned int i = 0; i < board.size(); ++i) {
		if (board[i] != '.' && board[i] != '*') setTrack[d->Find(i)]++;
	}

	counter = 0;	
	for (int i = 0; i < (int)board.size(); ++i) {
		if (setTrack[i] >= mss) {
			for (unsigned int j = 0; j < goals.size(); ++j) {
				if (d->Find(j) == i && goals[j] == 1) {
					/* If the current set size is bigger, we update our counter to be the maximum set size, and insert the positions of the counter inside a map */
					if (setTrack[i] > counter) counter = setTrack[i];
					score[counter].push_back(j / c);
					score[counter].push_back(j % c);
					break;
				}
			}
		}
	}

	/* If there are less spots than there are minimum spaces to score, then we can try and max our SCORE move */
	if (board.size() - indices.size() < (unsigned int)mss && !score.empty()) cout << "SCORE " << score.rbegin()->second[0] << ' ' << score.rbegin()->second[1] << '\n';
	
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
	s = new Superball(argc, argv);
	
	s->Play();
	delete s;

	return 0;
}
