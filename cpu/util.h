//
// Created by dimitrije on 9/12/17.
//

#ifndef EMULATOR_M0_UTIL_H
#define EMULATOR_M0_UTIL_H


#include <cstdint>

static inline uint32_t neg(const uint32_t i)
{
    return i >> 31;
}
static inline uint32_t pos(const uint32_t i)
{
    return (~i) >> 31;
}


// used to update the flags

static inline bool add_carry(const uint32_t a, const uint32_t b, const uint32_t c)
{
    return static_cast<bool>((neg(a) & neg(b)) |
                             (neg(a) & pos(c)) |
                             (neg(b) & pos(c)));
}

static inline bool add_overflow(const uint32_t a, const uint32_t b, const uint32_t c)
{
    return static_cast<bool>((neg(a) & neg(b) & pos(c)) |
                             (pos(a) & pos(b) & neg(c)));
}

static inline bool sub_carry(const uint32_t a, const uint32_t b, const uint32_t c)
{
    return static_cast<bool>((neg(a) & pos(b)) |
                             (neg(a) & pos(c)) |
                             (pos(b) & pos(c)));
}

static inline bool sub_overflow(const uint32_t a, const uint32_t b, const uint32_t c)
{
    return static_cast<bool>((neg(a) & pos(b) & pos(c)) |
                             (pos(a) & neg(b) & neg(c)));
}


#endif //EMULATOR_M0_UTIL_H
