#include "counter.h"

StatusCode counter_init(Counter* c, int initValue) {
    if (!c) return STATUS_INVALID_ARG;
    c->value = initValue;
    return STATUS_OK;
}

StatusCode counter_increment(Counter* c) {
    if (!c) return STATUS_INVALID_ARG;
    c->value++;
    return STATUS_OK;
}

StatusCode counter_decrement(Counter* c) {
    if (!c) return STATUS_INVALID_ARG;
    c->value--;
    return STATUS_OK;
}

int counter_get(const Counter* c) {
    return c ? c->value : 0; // defensive: returns 0 if NULL
}