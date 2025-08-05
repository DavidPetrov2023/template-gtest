#ifndef COUNTER_H
#define COUNTER_H

typedef struct {
    int value;
} Counter;

void counter_init(Counter* c);
void counter_increment(Counter* c);
void counter_decrement(Counter* c);
int counter_get(const Counter* c);

#endif
