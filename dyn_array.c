#include "dyn_array.h"

#define SIZEOF_WORD 4

#define SCALEFACOTR 2

static inline void Array_resize(Array array);
static inline void Queue_resize(Queue queue);

struct Array_T {
        unsigned length;
        Word* values;
};

struct Queue_T {
        unsigned head;
        unsigned length;
        struct Array_T array;
};

Queue Queue_new(unsigned hint) {
        Queue queue = malloc(sizeof(struct Queue_T));
        queue->head = 0;
        queue->length = 0;
        queue->array.length = hint;
        queue->array.values = malloc(SIZEOF_WORD * hint);
        return queue;
}

Array Array_new(unsigned hint) {
        Array array = malloc(sizeof(struct Array_T));
        array->values = malloc(SIZEOF_WORD * hint);
        array->length = hint;
        return array;
}

Word Queue_pop(Queue queue) {
        Word x = UM_NULL;
        if (queue->length == 0) return UM_NULL;
        x = queue->array.values[(queue->head%queue->array.length)];
        queue->head = (queue->head+1)%queue->array.length;
        --queue->length;
        return x;
}

void Queue_push(Queue queue, Word value) {
        unsigned i;
        if (queue->length == queue->array.length) {
                Queue_resize(queue);
        }
        i = queue->length++;
        queue->array.values[((queue->head+i)%queue->array.length)] = value;
}


Word* Array_at(Array array, unsigned pos) {
        return &(array->values[pos]);
}

static inline void Array_resize(Array array) {
        array->length *= SCALEFACOTR;
        array->values = realloc(array->values, SIZEOF_WORD*array->length);
        assert(array->values != NULL);
}

static inline void Queue_resize(Queue queue) {
        Array_resize(&queue->array);
}

void Queue_free(Queue* queue) {
        free((*queue)->array.values);
        free(*queue);
}
void Array_free(Array* array) {
        free((*array)->values);   
        free(*array);
}