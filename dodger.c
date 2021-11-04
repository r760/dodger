#include <ncurses.h>
#include <stdlib.h>

#define HEIGHT 10
#define WIDTH 22

#define LEFT_KEY(c) (c == 'h' || c == 'a')
#define RIGHT_KEY(c) (c == 'l' || c == 'd')

void gameOverHandler();

struct Coordinate {
  int y;
  int x;
};

char board[HEIGHT][WIDTH];

struct Coordinate *aircraft[4];
char defaultCharacter = ' ', enemyCharacter = '*';
bool stay = true;
int score = 0, bestScore = 0;

/*
 * Utility Functions
 */

/**
 * set all board pieces to c
 */
void set(char c) {
  int y, x;
  for (y = 0; y < HEIGHT; y++) {
    for (x = 0; x < WIDTH; x++) {
      board[y][x] = c;
    }
  }
}

void writeToBoard() {
  int y, x;
  for (y = 0; y < HEIGHT; y++) {
    for (x = 0; x < WIDTH; x++) {
      if (board[y][x] == '^' || board[y][x] == '+') {
        board[y][x] = defaultCharacter;
      }
    }
  }

  int i;
  for (i = 0; i < 4; i++) {
    int y = aircraft[i]->y;
    int x = aircraft[i]->x;
    if (i == 0) {
      board[y][x] = '^';
    } else {
      board[y][x] = '+';
    }
  }
}

void newGame() {
  set(defaultCharacter);
  struct Coordinate *c0 =
      (struct Coordinate *)malloc(sizeof(struct Coordinate));
  c0->y = HEIGHT - 2;
  c0->x = (WIDTH / 2);

  struct Coordinate *c1 =
      (struct Coordinate *)malloc(sizeof(struct Coordinate));
  c1->y = HEIGHT - 1;
  c1->x = (WIDTH / 2) - 1;

  struct Coordinate *c2 =
      (struct Coordinate *)malloc(sizeof(struct Coordinate));
  c2->y = HEIGHT - 1;
  c2->x = (WIDTH / 2);

  struct Coordinate *c3 =
      (struct Coordinate *)malloc(sizeof(struct Coordinate));
  c3->y = HEIGHT - 1;
  c3->x = (WIDTH / 2) + 1;

  aircraft[0] = c0;
  aircraft[1] = c1;
  aircraft[2] = c2;
  aircraft[3] = c3;

  stay = true;
  score = 0;
}

/*
 * IO Functions
 */

void print(const char *temp, int y, int x) {
  int i = 0;
  while (temp[i] != '\0') {
    mvaddch(y, x++, temp[i++]);
  }
}

void printBindings() {
  const char *bindings[4] = {"Commands:", "q - Quit\n",
                             "h (or a) - Move left\n",
                             "l (or d) - Move right\n"};

  int i, y = 2, x = 99;
  for (i = 0; i < 4; i++) {
    if (i == 1) {
      x++;
    }
    print(bindings[i], y++, x);
  }
}

void printBoard() {
  int y, x, temp;
  temp = 2;
  for (y = 0; y < HEIGHT; y++) {
    move(temp, 5);
    printw("+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+--"
           "-+---+---+---+---+---+\n");
    move(temp + 1, 5);
    for (x = 0; x < WIDTH; x++) {
      printw("| %c ", board[y][x]);
    }
    printw("|\n");
    temp += 2;
  }
  move(temp, 5);
  printw("+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+"
         "---+---+---+---+---+\n");
}

void printScore() {
  move(1, 42);
  printw("BEST SCORE (%d) - SCORE (%d)\n", bestScore, score);
}

void gameOverHandler() {
  char c;

  if (score > bestScore) {
    bestScore = score;
  }

  while (true) {
    clear();
    printBoard();
    printScore();
    print("Commands:\n", 2, 99);
    print("q - Quit\n", 3, 100);
    print("n - New game\n", 4, 100);
    print("Game Over!\n", 24, 45);
    move(2, 108);
    refresh();

    c = getch();
    if (c == 'q') {
      stay = false;
      break;
    } else if (c == 'n') {
      newGame();
      break;
    }
  }
}

void spawn() {
  int y = -1, x = -1;
  y = rand() % (HEIGHT / 2);
  x = rand() % WIDTH;

  board[y][x] = enemyCharacter;
}

/**
 * movement
 */

void left(struct Coordinate *aircraft[]) {
  int i;
  if (aircraft[1]->x - 1 >= 0) {
    for (i = 0; i < 4; i++) {
      int x = aircraft[i]->x;
      aircraft[i]->x = x - 1;
    }
  }
}

void right(struct Coordinate *aircraft[]) {
  int i;
  if (aircraft[3]->x + 1 <= WIDTH - 1) {
    for (i = 0; i < 4; i++) {
      int x = aircraft[i]->x;
      aircraft[i]->x = x + 1;
    }
  }
}

void fall() {
  int y, x;
  for (y = HEIGHT - 1; y >= 0; y--) {
    for (x = 0; x < WIDTH; x++) {
      if (board[y][x] == enemyCharacter) {
        board[y][x] = defaultCharacter;
        int yTemp = y + 1;
        if (yTemp <= HEIGHT - 1) {
          board[yTemp][x] = enemyCharacter;

          int i;
          for (i = 0; i < 4; i++) {
            if (aircraft[i]->y == yTemp && aircraft[i]->x == x) {
              gameOverHandler();
              return;
            }
          }
        } else {
          score++;
        }
      }
    }
  }
}

void writeOutBestScore() {
  FILE *out = fopen("best_score.txt", "w");
  fprintf(out, "%d", bestScore);
  fclose(out);
}

void readInBestScore() {
  FILE *in = fopen("best_score.txt", "r");
  if (in == NULL) {
    writeOutBestScore();
  } else {
    fscanf(in, "%d", &bestScore);
    fclose(in);
  }
}

int main() {
  readInBestScore();

  newGame();
  initscr();

  while (stay) {
    // timeout(70);
    timeout(250);
    clear();
    writeToBoard();
    fall();
    spawn();
    printBoard();
    printScore();
    printBindings();
    move(2, 108);
    refresh();

    if (stay) {
      char c = getch();
      if (c == 'q') {
        stay = false;
      } else if (LEFT_KEY(c)) {
        left(aircraft);
      } else if (RIGHT_KEY(c)) {
        right(aircraft);
      }
    }
  }

  endwin();
  writeOutBestScore();
  return 0;
}