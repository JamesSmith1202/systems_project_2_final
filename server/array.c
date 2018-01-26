#include "array.h"

void init_arr(Array *a, size_t initialSize) {
  a->array = (struct chat_room *)malloc(initialSize * sizeof(struct chat_room *));
  a->len = 0;
  a->size = initialSize;
}

void insert(Array *a, struct chat_room element) {
  if (a->len == a->size) {
    a->size *= 2;
    a->array = (struct chat_room *)realloc(a->array, a->size * sizeof(struct chat_room *));
  }
  a->array[a->len++] = element;
}

void free_arr(Array *a) {
  free(a->array);
  a->array = NULL;
  a->len = a->size = 0;
}