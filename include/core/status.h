// status.h
#pragma once

typedef enum {
    STATUS_OK = 0,
    STATUS_NULL = -1,
    STATUS_INVALID_ARG = -2,
    STATUS_OVERFLOW = -3,
    STATUS_UNDERFLOW = -4,
    STATUS_IMEOUT = -5,
    STATUS_NOT_IMPLEMENTED = -6
} StatusCode;