#include "counter.h"

void counter_init(Counter* c) {
    c->value = 0;
}

void counter_increment(Counter* c) {
    c->value++;
}

void counter_decrement(Counter* c) {
    c->value--;
}

int counter_get(const Counter* c) {
    return c->value;
}
