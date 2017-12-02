/* This is the Graphical Editor problem from programming challenges *
 * Miguel - Skiena. The problem can be found on page 19 of the book *
 * or on page 39 of the PDF file (Program Used: Okular)             */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define ALLOC(size, type) ((type*) malloc((size) * sizeof(type)))

#define FREE_AND_NULL(p) {free(p); p = NULL;}

#define DEC(x) (x - 48U)

#define NB(v1, v2) ((v1 == v2) ? 1U : 0U) /* NB stands for neighbour */

typedef unsigned char uchar;

short int execute_command(uchar***, size_t*, size_t*, uchar);
uchar** create(size_t, size_t);
void free_a(uchar**, size_t, size_t);
void print_array(uchar**, size_t, size_t);
void command_i(uchar***, size_t*, size_t*);
void command_c(uchar**, size_t, size_t);
void command_l(uchar**, size_t, size_t);
void command_v(uchar**, size_t, size_t);
void command_h(uchar**, size_t, size_t);
void command_k(uchar**, size_t, size_t);
void command_s(uchar**, size_t, size_t);
void command_f(uchar**, size_t, size_t);

unsigned short int flag_e;

int main(void) {
  uchar ch;
  uchar** array;
  size_t r;
  size_t c;
  while ((ch = getchar()) && execute_command(&array, &r, &c, ch));
  return EXIT_SUCCESS;
}

uchar** create(size_t r, size_t c) {
  uchar** new = ALLOC(r, uchar*);
  for (size_t i = 0; i != r; ++i) {
    new[i] = ALLOC(c, uchar);
  } return new;
}

void free_a(uchar** array, size_t r, size_t c) {
  for (size_t i = 0; i != r; ++i) {
    FREE_AND_NULL(array[i]);
  } FREE_AND_NULL(array);
}

void print_array(uchar** array, size_t r, size_t c) {
  for (size_t i = 0; i != r; ++i) {
    putchar('\n');
    for (size_t j = 0; j != c; ++j)
      putchar(array[i][j]);
  } putchar('\n');
}

/* Command execution. The available commands are all the possible cases.
   Every other command has no effect on the program.                     */

short int execute_command(uchar*** array, size_t* r,
                          size_t* c, uchar command) {
  switch(command) {
  case 'I': command_i(array, r, c);    return 1;
  case 'C': command_c(*array, *r, *c); return 1;
  case 'L': command_l(*array, *r, *c); return 1;
  case 'V': command_v(*array, *r, *c); return 1;
  case 'H': command_h(*array, *r, *c); return 1;
  case 'K': command_k(*array, *r, *c); return 1;
  case 'F': command_f(*array, *r, *c); return 1;
  case 'S': command_s(*array, *r, *c); return 1;
  case 'X': free_a(*array, *r, *c);    return 0;
  default : {
    uchar ch;
    while ((ch = getchar()) != '\n');
    return 1;
  }
  }
}

void get_command_arguments(char* args) {
  uchar ch;
  size_t i = 0;
  while ((ch = getchar()) != '\n') {
    if (isspace(ch)) continue;
    args[i++] = ch;
  }
}

/* Recursive implementation of the F command. An iterative one could be faster
   but it's not worth the time to write one                                    */

void colour(uchar** array, size_t i, size_t j,
	    size_t r, size_t c, uchar cl) {
  uchar original = array[i][j];
  array[i][j] = cl;
  if (i > 0 && NB(array[i-1][j], original)) colour(array, i-1, j, r, c, cl);
  if (i + 1 < r && NB(array[i+1][j], original)) colour(array, i+1, j, r, c, cl);
  if (j > 0 && NB(array[i][j-1], original)) colour(array, i, j-1, r, c, cl);
  if (j+1 < c && NB(array[i][j+1], original)) colour(array, i, j+1, r, c, cl);
}

void command_f(uchar** array, size_t r, size_t c) {
  uchar args[3];
  get_command_arguments(args);
  colour(array, DEC(args[1]) - 1U,
	 DEC(args[0]) - 1U,
	 r, c, args[2]);
}

void command_s(uchar** array, size_t r, size_t c) {
  uchar ch;
  while ((ch = getchar()) != '\n') {
    if (isspace(ch)) continue;
    putchar(ch);
  } print_array(array, r, c);
}

void command_k(uchar** array, size_t r, size_t c) {
  uchar args[5];
  get_command_arguments(args);
  for (size_t i = DEC(args[1]) - 1U; i <= DEC(args[3]) - 1U; ++i)
    for (size_t j = DEC(args[0]) - 1U; j <= DEC(args[2]) - 1U; ++j)
      array[i][j] = args[4];
}

void command_v(uchar** array, size_t r, size_t c) {
  uchar args[4];
  get_command_arguments(args);
  for (size_t i = DEC(args[1]) - 1U; i <= DEC(args[2]) - 1U; ++i)
    array[i][DEC(args[0]) - 1U] = args[3];
}

void command_l(uchar** array, size_t r, size_t c) {
  uchar args[3];
  get_command_arguments(args);
  array[DEC(args[1]) - 1U][DEC(args[0]) - 1U] = args[2];
}

void command_c(uchar** array, size_t r, size_t c) {
  for (size_t i = 0; i != r; ++i)
    for (size_t j = 0; j != c; ++j)
      array[i][j] = 48U;
}

void command_i(uchar*** array, size_t* r, size_t* c) {
  uchar args[2];
  get_command_arguments(args);
  *r = DEC(args[1]);
  *c = DEC(args[0]);
  *array = create(*r, *c);
  command_c(*array, *r, *c);
}

void command_h(uchar** array, size_t r, size_t c) {
  uchar args[4];
  get_command_arguments(args);
  for (size_t i = DEC(args[0]) - 1U; i <= DEC(args[1]) - 1U; ++i)
    array[DEC(args[2]) - 1U][i] = args[3];
}
