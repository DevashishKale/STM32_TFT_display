#ifndef DISPLAY_H
#define DISPLAY_H

#include "stm32f4xx_hal.h"
#include <stdlib.h>
#include "main.h"

void Display_Round_Icon_40x40(const unsigned int icon[], unsigned int x0, unsigned int y0, unsigned int r);
void Display_Square_Icon_40x40(const unsigned int icon[], unsigned int x0, unsigned int y0);
void Display_Back_Icon(unsigned int x0, unsigned int y0);
void Display_Menu(void);
void Display_Text(void);
void Display_Picture(void);
void Display_Color_Picture(void);
void Display_BMW_Picture(void);

#endif /* DISPLAY_H */
