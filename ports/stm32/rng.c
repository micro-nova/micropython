/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2018 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "rng.h"

#if MICROPY_HW_ENABLE_RNG

#define RNG_TIMEOUT_MS (10)

uint32_t rng_randint(uint32_t min, uint32_t max) {
    if (min==max) {
        return 0;
    }

    // Enable the RNG peripheral if it's not already enabled
    if (!(RNG->CR & RNG_CR_RNGEN)) {
        __HAL_RCC_RNG_CLK_ENABLE();
        RNG->CR |= RNG_CR_RNGEN;
    }

    // Wait until the RNG is ready
    while (!(RNG->SR & RNG_SR_DRDY)) {
    }

    return (RNG->DR%(max-min))+min;
}

STATIC mp_obj_t pyb_rng_randint(mp_obj_t min, mp_obj_t max) {
    return mp_obj_new_int(rng_randint(mp_obj_get_int(min), mp_obj_get_int(max)));
}
MP_DEFINE_CONST_FUN_OBJ_2(pyb_rng_randint_obj, pyb_rng_randint);

uint32_t rng_get(void) {
    // Enable the RNG peripheral if it's not already enabled
    if (!(RNG->CR & RNG_CR_RNGEN)) {
        __HAL_RCC_RNG_CLK_ENABLE();
        RNG->CR |= RNG_CR_RNGEN;
    }

    // Wait for a new random number to be ready, takes on the order of 10us
    uint32_t start = HAL_GetTick();
    while (!(RNG->SR & RNG_SR_DRDY)) {
        if (HAL_GetTick() - start >= RNG_TIMEOUT_MS) {
            return 0;
        }
    }

    // Get and return the new random number
    return RNG->DR;
}

// Return a 30-bit hardware generated random number.
STATIC mp_obj_t pyb_rng_get(void) {
    return mp_obj_new_int(rng_get() >> 2);
}
MP_DEFINE_CONST_FUN_OBJ_0(pyb_rng_get_obj, pyb_rng_get);

#endif // MICROPY_HW_ENABLE_RNG
