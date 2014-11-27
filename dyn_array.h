#ifndef DYN_ARRAY_H
#define DYN_ARRAY_H

#include "virtualization.h"

typedef struct Array_T * Array;
typedef struct Queue_T * Queue;

Queue Queue_new(unsigned hint, size_t size);
Array Array_new(unsigned hint, size_t size);

Word Queue_pop(Queue queue);
void Queue_push(Queue queue, Word value);
Word* Array_at(Array array, unsigned pos);

void Queue_free(Queue* seq);
void Array_free(Array* seq);

#endif // DYN_ARRAY_H
