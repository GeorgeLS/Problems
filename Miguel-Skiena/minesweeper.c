#include <stdio.h>
#include <stdlib.h>

#define ALLOC(size, type) ((type*) malloc((size) * sizeof(type)))
#define CALLOC(size, type) ((type*) calloc((size), sizeof(type)))

/* These are for geting the position of the mine */

#define INSIDE 0
#define CORNER 1
#define ROW 2
#define COL 3

/* These are for geting the direction of the mine */

#define LEFT 4
#define RIGHT 7
#define UP 5
#define DOWN 6

/* Combining "position" and direction to get the location of the mine */

#define C1 (UP + LEFT)
#define C2 (UP + RIGHT)
#define C3 (DOWN + LEFT)
#define C4 (DOWN + RIGHT)

/* We have 9 different cases:                                                       *
 * 1st case: Inside                                                                 *
 * 2nd case: C1                                                                     *
 * 3rd case: C2                                                                     *
 * 4th case: C3                                                                     *
 * 5th case: C4                                                                     *
 * 6th case: UP                                                                     *
 * 7th case: DOWN                                                                   *
 * 8th case: LEFT                                                                   *
 * 9th case: RIGHT                                                                  *
 *                                                                                  *
 * Below there is a 3D array that helps compute the indices of the array            *
 * that needs to be accessed in each case to get the number of total adjacent mines */

static ssize_t indices[9][2][8] = { { {-1, -1, -1, 0, 0, 1, 1, 1}, {-1, 0, 1, -1 , 1, -1, 0, 1} },   /* 1st case */
                                    { {0, 1, 1, 0, 0, 0, 0, 0},    {1, 0, 1, 0, 0, 0, 0, 0}     },   /* 2nd case */
                                    { {0, 1, 1, 0, 0, 0, 0, 0},    {-1, -1, 0, 0, 0, 0, 0, 0}   },   /* 3rd case */
                                    { {1, 1, 0, 0, 0, 0, 0, 0},    {0, 1, 1, 0, 0, 0, 0, 0}     },   /* 4th case */
                                    { {1, 1, 0, 0, 0, 0, 0, 0},    {-1, 0, -1, 0, 0, 0, 0, 0}   },   /* 5th case */
                                    { {0, 0, 1, 1, 1, 0, 0, 0},    {-1, 1, -1, 0, 1, 0, 0, 0}   },   /* 6th case */
                                    { {-1, -1, -1, 0, 0, 0, 0, 0}, {-1, 0, 1, -1, 1, 0, 0, 0}   },   /* 7th case */
                                    { {-1, -1, 0, 1, 1, 0, 0, 0},  {0, 1, 1, 0, 1, 0, 0, 0}     },   /* 8th case */
                                    { {-1, -1, 0, 1, 1, 0, 0, 0},  {-1, 0, -1, -1, 0, 0, 0, 0}  } }; /* 9th case */

typedef struct {
  size_t a_i;
  size_t a_j;
} index_t;

static unsigned int** mines;
static index_t** i_a;
static char** field;

static void get_mines(char**, unsigned int**, size_t, size_t, size_t, size_t);
static unsigned int get_position(size_t, size_t, size_t, size_t);
static unsigned int get_direction(size_t, size_t, unsigned int);
static char** create_field(size_t, size_t);
static void free_field(char**, size_t);
static unsigned int** create_mines(size_t, size_t);
static void free_mines(unsigned int**, size_t);
static void get_field(char**, size_t, size_t, index_t**, size_t*);

int main(void) {
  size_t n;
  size_t m;
  unsigned int c = 0;
  scanf("%lu %lu", &n, &m);
  getchar();
  while (n && m) {
    i_a = ALLOC(n * m, index_t*);
    ++c;
    size_t i_c = 0;
    field = create_field(n, m);
    mines = create_mines(n, m);
    get_field(field, n, m, i_a, &i_c);
    for (size_t i = 0; i != i_c; ++i)
      get_mines(field, mines, n, m, i_a[i]->a_i, i_a[i]->a_j);
    for (size_t i = 0; i != i_c; ++i)
        mines[i_a[i]->a_i][i_a[i]->a_j] = '*';
    printf("Field #%u:\n", c);
    for (size_t i = 0; i != n; ++i) {
      putchar('\n');
      for (size_t j = 0; j != m; ++j) {
        if (mines[i][j] != '*') printf("%u", mines[i][j]);
        else printf("%c", mines[i][j]);
      }
    }
    putchar('\n');
    free_field(field, n);
    free_mines(mines, n);
    for (size_t i = 0; i != i_c; ++i) {
      free(i_a[i]);
      i_a[i] = NULL;
    }
    free(i_a);
    i_a = NULL;
    scanf("%lu %lu", &n, &m);
    getchar();
    putchar('\n');
  }
  return EXIT_SUCCESS;
}

void get_mines(char** field, unsigned int** mines, size_t field_r,
                 size_t field_c, size_t r, size_t c) {
  unsigned int p = get_position(field_r, field_c, r, c);
  unsigned int d = get_direction(r, c, p);
  size_t index;
  size_t n;
  switch(d) {
    case C1:     index = 1; n = 3; break;
    case C2:     index = 2; n = 3; break;
    case C3:     index = 3; n = 3; break;
    case C4:     index = 4; n = 3; break;
    case LEFT:   index = 7; n = 5; break;
    case RIGHT:  index = 8; n = 5; break;
    case UP:     index = 5; n = 5; break;
    case DOWN:   index = 6; n = 5; break;
    default:     index = 0; n = 8; break;
  }
  for (size_t i = 0; i != n; ++i) {
    ++mines[indices[index][0][i] + r][indices[index][1][i] + c];
  }
}

char** create_field(size_t r, size_t c) {
  char** new = ALLOC(r, char*);
  for (size_t i = 0; i != r; ++i) {
    new[i] = ALLOC(c, char);
  } return new;
}

void get_field(char** field, size_t r, size_t c, index_t** i_a, size_t* index) {
  for (size_t i = 0; i != r; ++i) {
    char ch;
    for (size_t j = 0; j != c; ++j) {
      field[i][j] = getchar();
      if (field[i][j] == '*') {
        i_a[*index] = ALLOC(1, index_t);
        i_a[*index]->a_i = i;
        i_a[(*index)++]->a_j = j;
      }
    }
    ch = getchar();
  }
}

void free_field(char** field, size_t r) {
  for (size_t i = 0; i != r; ++i) {
    free(field[i]);
    field[i] = NULL;
  }
  free(field);
  field = NULL;
}

unsigned int** create_mines(size_t r, size_t c) {
  unsigned int** new = CALLOC(r, unsigned int*);
  for (size_t i = 0; i != r; ++i) {
    new[i] = CALLOC(c, unsigned int);
  } return new;
}

void free_mines(unsigned int** mines, size_t r) {
  for (size_t i = 0; i != r; ++i) {
    free(mines[i]);
    mines[i] = NULL;
  }
  free(mines);
  mines = NULL;
}

unsigned int get_position(size_t field_r, size_t field_c,
                          size_t r, size_t c) {
  if ((r == 0 || r == field_r - 1) && 
      (c == 0 || c == field_c - 1)) return CORNER;
  else if ((r == 0 || r == field_r - 1) &&
           (c != 0 || c != field_c - 1)) return ROW;
  else if ((r != 0 || r != field_r - 1) &&
           (c == 0 || c == field_c - 1)) return COL;
  else return INSIDE;
}

unsigned int get_direction(size_t r, size_t c, unsigned int p) {
  switch(p) {
    case CORNER: return (((r == 0) ? UP : DOWN) + ((c == 0) ? LEFT : RIGHT));
    case ROW:    return ((r == 0) ? UP : DOWN);
    case COL:    return ((c == 0) ? LEFT : RIGHT);
  }
}
