#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ALLOC(size, type) ((type*) malloc((size) * sizeof(type)))

__attribute__((always_inline))
inline int absolute(int v1, int v2) {
  return (v1 < v2) ? v2 - v1 : v1 - v2;
}

__attribute__((always_inline))
inline bool check_for_jolly(int* seq, size_t l) {
  int* diff_seq = ALLOC(l - 1, int);
  for (size_t i = 0; i != l - 1; ++i) {
    int abs_v = absolute(seq[i], seq[i+1]);
    if (abs_v >= l || !abs_v || diff_seq[abs_v-1]) {
      free(diff_seq);
      return false;
    } else ++diff_seq[abs_v-1];
  }
  free(diff_seq);
  return true;
}

int main(void) {
  int seq_s;
  scanf("%d", &seq_s);
  int* seq = ALLOC(seq_s, int);
  for (size_t i = 0U; i != seq_s; ++i) {
    scanf("%d", &seq[i]);
  }
  bool jolly = check_for_jolly(seq, seq_s);
  if (jolly) printf("Jolly\n");
  else printf("Not jolly\n");
  free(seq);
  return EXIT_SUCCESS;
}
