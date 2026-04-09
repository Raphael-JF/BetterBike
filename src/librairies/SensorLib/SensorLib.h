#pragma once

#include <stdint.h>
#include <stddef.h>

#if defined(ARDUINO)
#include <Arduino.h>
#endif

#ifndef log_e
#define log_e(...) ((void)0)
#endif

#ifndef log_w
#define log_w(...) ((void)0)
#endif

#ifndef log_i
#define log_i(...) ((void)0)
#endif

#ifndef log_d
#define log_d(...) ((void)0)
#endif

#ifndef isBitSet
#define isBitSet(value, bit)            (((value) & (1UL << (bit))) == (1UL << (bit)))
#endif