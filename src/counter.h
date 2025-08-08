#pragma once

#include "status.h"

typedef struct {
    int value;
} Counter;

/**
 * @brief Initialize counter with a starting value.
 * 
 * @param c Pointer to Counter instance.
 * @param initValue Initial value to set.
 * @return STATUS_OK on success, STATUS_INVALID_ARG if c is NULL.
 */
StatusCode counter_init(Counter* c, int initValue);

/**
 * @brief Increment counter value by 1.
 * 
 * @param c Pointer to Counter instance.
 * @return STATUS_OK on success, STATUS_INVALID_ARG if c is NULL.
 */
StatusCode counter_increment(Counter* c);

/**
 * @brief Decrement counter value by 1.
 * 
 * @param c Pointer to Counter instance.
 * @return STATUS_OK on success, STATUS_INVALID_ARG if c is NULL.
 */
StatusCode counter_decrement(Counter* c);

/**
 * @brief Get current counter value.
 * 
 * @param c Pointer to Counter instance.
 * @return Current counter value, or 0 if c is NULL.
 */
int counter_get(const Counter* c);

