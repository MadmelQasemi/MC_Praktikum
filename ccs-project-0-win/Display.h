/*
 * Display.h
 *
 *  Created on: 28.11.2024
 *      Author: qasem
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_
// Size of Display define
#define LARGE_DISPLAY

#include <stdint.h>
#include <stdbool.h> // type bool for giop.h
#include "inc/hw_types.h"
#include "inc/tm4c1294ncpdt.h"
#include <stdio.h>   // Debug only
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>     // GPIO_PIN_X
#include <inc/hw_memmap.h>

// constants for display initialization
#define RST 0x10
#define INITIAL_STATE (0x1F)
#define SOFTWARE_RESET (0x01)
#define SET_PLL_MN (0xE2)
#define START_PLL (0xE0)
#define LOCK_PLL (0xE0)  // same as START_PLL
#define SET_LSHIFT (0xE6)
#define SET_LCD_MODE (0xB0)
#define SET_HORI_PERIOD (0xB4)
#define SET_VERT_PERIOD (0xB6)
#define SET_ADRESS_MODE (0x36)
#define SET_PIXEL_DATA_FORMAT (0xF0)
#define SET_DISPLAY_ON (0x29)
#define SET_DISPLAY_OFF (0x29) // not tested ??
uint32_t sysClock; // Variable for system clock
/* some predefined basic colors to use with names */
enum colors
{
    BLACK = 0x00000000,
    WHITE = 0x00FFFFFF,
    GREY = 0x00AAAAAA,
    RED = 0x00FF0000,
    GREEN = 0x0000FF00,
    BLUE = 0x000000FF,
    YELLOW = 0x00FFFF00
};

// Funktionsprototypen
inline void write_command(unsigned char);
inline void write_data(unsigned char);
inline void window_set(int, int, int, int);
void init_ports_display(void);
void configure_display_controller_large();
void write_rgb(enum colors);
void show_default_display();
void print_axis();
void print_number(int, int, char*,enum colors color);
void draw_bresenham_line(int, int, int, int, enum colors);
void draw_bresenham_circle(int, int, int, enum colors);
void draw_cursor_sign(int,int,int,enum colors);
void print_C(int,int);
void print_H(int,int);
void print_F(int, int);
void print_r(int, int);
void print_V(int,int);
void print_a(int ,int);
void print_doppelpunkt(int, int);
void print_Spanungszahl(int, int);
void print_komma(int);
#endif /* DISPLAY_H_ */
