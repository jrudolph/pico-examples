/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#define COLOR_BRG(R, G, B) ((G << 16) | (R << 8) | B)

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

void pattern_snakes(uint len, uint t) {
    for (uint i = 0; i < len; ++i) {
        uint x = (i + (t >> 1)) % 64;
        if (x < 10)
            put_pixel(urgb_u32(0xff, 0, 0));
        else if (x >= 15 && x < 25)
            put_pixel(urgb_u32(0, 0xff, 0));
        else if (x >= 30 && x < 40)
            put_pixel(urgb_u32(0, 0, 0xff));
        else
            put_pixel(0);
    }
}

void pattern_random(uint len, uint t) {
    if (t % 8)
        return;
    for (int i = 0; i < len; ++i)
        put_pixel(rand());
}

void pattern_sparkle(uint len, uint t) {
    if (t % 20)
        return;
    for (int i = 0; i < len; ++i)
        put_pixel(rand() % 32 ? 0 : COLOR_BRG(255,130,80));
}

void pattern_greys(uint len, uint t) {
    int max = 32; // let's not draw too much current!
    t %= max;
    for (int i = 0; i < len; ++i) {
        put_pixel(t * 0x10101);
        if (++t >= max) t = 0;
    }
}

typedef void (*pattern)(uint len, uint t);
const struct {
    pattern pat;
    const char *name;
} pattern_table[] = {
        //{pattern_snakes,  "Snakes!"},
        //{pattern_random,  "Random data"},
        {pattern_sparkle, "Sparkles"},
        //{pattern_greys,   "Greys"},
};

const int PIN_TX = 0;

const int p_len = 12; // first p has 12 leds
const int o_len = 7;  // o has 7 leds
const int p_2_len = 9;
const int o_off = p_len;
const int po_mix_idx = 9; // led 9 is part of o
const int num_leds = 32;
const int p_1_leds[] = {1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int o_leds[] =   {0,0,0,0,0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int o_p_2_leds[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // only want that middle one
const int p_2_leds[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,1,1,1,0,1,1,1,1,0,0,0};
const int excl_leds[]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1,1,1};

const uint32_t p_1_color = COLOR_BRG(240, 5, 2);
const uint32_t p_1_o_color = COLOR_BRG(20, 0, 40);
const uint32_t o_color = COLOR_BRG(12, 35, 120);
const uint32_t o_p2_color = COLOR_BRG(28, 70, 6);
const uint32_t p_2_color = COLOR_BRG(200, 40, 0); ///* 23, 167, 254 */));// 20,135,206
const uint32_t p_2_excl_color = COLOR_BRG(90, 13, 1);
const uint32_t excl_color = COLOR_BRG(150, 12, 8);

void show_letter(const int *letter, uint32_t color, uint t) {
    for (int i = 0; i < num_leds; ++i) {
        if (letter[i])
            put_pixel(color);
        else
            put_pixel(0);
    }
}
void show_all(uint t) {
    for (int i = 0; i < num_leds; ++i) {
        if (p_1_leds[i] && o_leds[i])
            put_pixel(p_1_o_color);
        else if (p_1_leds[i])
            put_pixel(p_1_color);
        else if (o_p_2_leds[i])
            put_pixel(o_p2_color);
        else if (o_leds[i])
            put_pixel(o_color);
        else if (p_2_leds[i] && excl_leds[i])
            put_pixel(p_2_excl_color);
        else if (p_2_leds[i])
            put_pixel(p_2_color);
        else if (excl_leds[i])
            put_pixel(excl_color);
    }
}

void story() {
    // worm finds the light
    // worm moves around randomly
    // worm gets to spin (faster and faster)
    // worm overheats
    // worm heartbeats
    // worm grows a sparkling longer tail
    // fade out
    // fireworks
    // p - o - p - !
    // pop! pulsating
    // pop! static
    // (smiley)
}

int main() {
    //set_sys_clock_48();
    stdio_init_all();
    puts("WS2812 Smoke Test");

    // todo get free sm
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, PIN_TX, 800000, false);

    int t = 0;
    while (1) {
        for (int i = 0; i < 100; ++i) {
            show_letter(p_1_leds, p_1_color, i);
            sleep_ms(10);
        }
        for (int i = 0; i < 100; ++i) {
            show_letter(o_leds, o_color, i);
            sleep_ms(10);
        }
        for (int i = 0; i < 100; ++i) {
            show_letter(p_2_leds, p_2_color, i);
            sleep_ms(10);
        }
        for (int i = 0; i < 100; ++i) {
            show_letter(excl_leds, excl_color, i);
            sleep_ms(10);
        }
        for (int i = 0; i < 100; ++i) {
            show_all(i);
            sleep_ms(10);
        }
        // int pat = rand() % count_of(pattern_table);
        // int dir = (rand() >> 30) & 1 ? 1 : -1;
        // puts(pattern_table[pat].name);
        // puts(dir == 1 ? "(forward)" : "(backward)");
        // for (int i = 0; i < 1000; ++i) {
        //     pattern_table[pat].pat(num_leds, t);
        //     sleep_ms(10);
        //     t += dir;
        // }
    }
}