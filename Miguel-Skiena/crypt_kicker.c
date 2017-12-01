#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define ALLOC(size, type) ((type*) malloc((size) * sizeof(type)))

#define CALLOC(size, type) ((type*) calloc((size), sizeof(type)))

#define WL 17	 /* That's the maximum word length we can have + 1 for the null terminating character */

#define LL 81	 /* Same as above except it's for the line length */

typedef unsigned char uchar;
typedef unsigned int uint;

/* Representing each word by the word (obviously), by an array which holds 
   the occurrence of every character, the length of the word and it's matched encrypted word. */

typedef struct {
  char* word;
  uchar* occurrences;
  uchar* match;
  size_t map[26];
  size_t wlen;
} word_t;

char buffer[LL];

/* Alright GCC, If you don't want to  my functions then I'll force you to... :) */

inline word_t* new_word_array(size_t) __attribute__((always_inline));
inline word_t* get_text(size_t*) __attribute__((always_inline));
inline uint matches(word_t, word_t) __attribute__((always_inline));
inline uint get_words(void) __attribute__((always_inline));
inline void construct_word(word_t*) __attribute__((always_inline));
inline void free_word_array(word_t*, size_t) __attribute__((always_inline));
inline void compute_occurrences(word_t*, size_t) __attribute__((always_inline));
inline void map(word_t*, size_t) __attribute__((always_inline));
inline void decrypt(word_t*, word_t*, size_t, size_t) __attribute__((always_inline));
inline void get_dictionary(word_t*, size_t) __attribute__((always_inline));
inline void print_decryption(word_t*, size_t) __attribute__((always_inline));
inline void clear_matches(word_t*, size_t) __attribute__((always_inline));

int main(void) {
  size_t n;
  scanf("%llu", &n);
  getchar();
  word_t* dict = new_word_array(n);
  get_dictionary(dict, n);
  map(dict, n);
  compute_occurrences(dict, n);
  while (1) {
    size_t w;
    word_t* text = get_text(&w);
    if (feof(stdin)) break;
    map(text, w);
    compute_occurrences(text, w);
    decrypt(dict, text, n, w);
    print_decryption(text, w);
    clear_matches(dict, n);
    free_word_array(text, w);
  }
  free_word_array(dict, n);
  return EXIT_SUCCESS;
}

inline word_t* new_word_array(size_t r) {
  word_t* array = ALLOC(r, word_t);
  for (size_t i = 0U; i != r; ++i) {
    construct_word(&array[i]);
  } return array;
}

inline void free_word_array(word_t* array, size_t r) {
  for (size_t i = 0U; i != r; ++i) {
    free(array[i].word);
    free(array[i].occurrences);
    free(array[i].match);
  } free(array);
}

inline void get_dictionary(word_t* dict, size_t r) {
  uchar ch;
  for (size_t i = 0; i != r; ++i) {
    size_t j = 0U;
    while((ch = getchar()) != '\n') {
      dict[i].word[j++] = ch;
    }
    dict[i].wlen = j;
    dict[i].word[j] = '\0';		     /* null terminating the fucking string */
    dict[i].occurrences = CALLOC(j, uchar);
    dict[i].match = CALLOC(j + 1, uchar);
  }
}

inline word_t* get_text(size_t* w) {
  fgets(buffer, LL, stdin);
  buffer[strlen(buffer) - 1] = '\0';
  uint count = get_words() + 1;
  word_t* text = new_word_array(count);
  *w = count;
  size_t i = 0U;
  size_t j = 0U;
  char* aux = buffer;
  while (*aux != '\0') {
    if (*aux == ' ') {
      text[i].word[j] = '\0';
      text[i].occurrences = CALLOC(j, uchar);
      text[i].match = CALLOC(j + 1, uchar);
      text[i++].wlen = j;
      j = 0U; ++aux; continue;
    }
    text[i].word[j++] = *aux++;
  }
  text[i].occurrences = CALLOC(j, uchar);
  text[i].match = CALLOC(j + 1, uchar);
  text[i].wlen = j;
  return text;
}

inline uint get_words(void) {
  assert(buffer[0] != '\0');	/* NOTE: I don't even remember why I assert this... */
  char* aux = buffer;
  uint count = 0U;
  while (*aux != '\0') if (*aux++ == ' ') ++count;
  return count;
}

inline void construct_word(word_t* word) {
  word->word = ALLOC(WL, char); 	/* We allocate WL chars just to make sure the word will fit */
  word->occurrences = NULL;		/* But we allocate space for occurrences and match */
  word->match = NULL;			/* later for space efficiency, where we will know the exact space needed */
  memset(word->map, 0, sizeof(word->map));
  word->wlen = 0U;
}

/* Mapping the characters of the alphabet to their position in the string */
inline void map(word_t* array, size_t n) {
  for (size_t i = 0U; i != n; ++i) {
    for (size_t j = 0U; j != array[i].wlen; ++j) {
      array[i].map[array[i].word[j] - 'a'] = j;
    }
  }
}

/* Computing the occurrence for every character int the string */
inline void compute_occurrences(word_t* array, size_t n) {
  for (size_t i = 0U; i != n; ++i) {
    for (size_t j = 0U; j != array[i].wlen; ++j) {
      ++array[i].occurrences[array[i].map[array[i].word[j] - 'a']];
    }
  }
}

/* Checking if an encrypted word matches a word in our dictionary */
inline uint matches(word_t target, word_t source) {
  if (target.wlen != source.wlen) return 0U;
  size_t len = target.wlen;
  for (size_t i = 0U; i != len; ++i) {
    if (target.occurrences[target.map[target.word[i] - 'a']] !=
	source.occurrences[source.map[source.word[i] - 'a']]) return 0U;
  } return 1U;
}

/* Decrypting the encrypted text */
inline void decrypt(word_t* dict, word_t* enc_text, size_t dl, size_t el) {
  for (size_t i = 0U; i != el; ++i) {
    uchar flag = 0U;		/* Via this flag we check if a match was found */
    for (size_t j = 0U; j != dl; ++j) {
      if (matches(enc_text[i], dict[j])) { /* If the encrypted word matches one of our dictionary then: */
	if (dict[j].match[0] == 0) {	   /* If the current dictionary word hasn't been matched */
	  strncpy(enc_text[i].match, dict[j].word, dict[j].wlen); /* Then we match these words and continue to the next */
	  enc_text[i].match[strlen(enc_text[i].match)] = '\0'; /* encrypted word */
	  strncpy(dict[j].match, enc_text[i].word, enc_text[i].wlen);
	  dict[j].match[strlen(dict[j].match)] = '\0';
	  flag = 1U; break;
	} else {		/*  */
	  if (strncmp(enc_text[i].word, dict[j].match, dict[j].wlen) == 0) {
	    strncpy(enc_text[i].match, dict[j].word, dict[j].wlen);
	    enc_text[i].match[strlen(enc_text[i].match)] = '\0';
	    flag = 1U; break;
	  } else {
	    continue;
	  }
	}
      }
    }
    if (!flag) {
      for (size_t j = 0U; j != enc_text[i].wlen; ++j) {
	enc_text[i].match[j] = '*';
      } enc_text[i].match[enc_text[i].wlen] = '\0';
    }
  }
}

inline void print_decryption(word_t* array, size_t n) {
  for (size_t i = 0U; i != n; ++i) {
    printf("%s ", array[i].match);
  } putchar('\n');
}

inline void clear_matches(word_t* dict, size_t n) {
  for (size_t i = 0U; i != n; ++i ) {
    memset(dict[i].match, 0, dict[i].wlen);
  }
}
