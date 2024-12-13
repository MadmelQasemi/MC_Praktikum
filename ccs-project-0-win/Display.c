/*
 * Display.c
 *
 *  Created on: 28.11.2024
 *      Author: qasem
 */

#include "Display.h"
char numbers[10][10] = { { 0x7E, 0xFF, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xFF,
                           0x7E }, // zero
        { 0x1F, 0x3F, 0x77, 0xE7, 0xC7, 0x87, 0x07, 0x07, 0x07, 0x07 }, // one
        { 0x7E, 0xFF, 0x83, 0x03, 0x0F, 0x3E, 0x78, 0xE0, 0xFF, 0xFF }, // two
        { 0x7E, 0xFF, 0x83, 0x03, 0x3E, 0x3E, 0x03, 0x83, 0xFF, 0x7E }, // three
        { 0x03, 0x0F, 0x1F, 0x3F, 0x63, 0xC3, 0xFF, 0xFF, 0x03, 0x03 }, // four
        { 0xFF, 0xFF, 0xC0, 0xC0, 0xFE, 0xFF, 0x03, 0x83, 0xFF, 0x7E }, // five
        { 0x7E, 0xFF, 0xC0, 0xC0, 0xFE, 0xFF, 0xC3, 0xC3, 0xFF, 0x7E }, // six
        { 0xFF, 0xFF, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0xC0 }, // seven
        { 0x7E, 0xFF, 0xC3, 0xC3, 0x7E, 0x7E, 0xC3, 0xC3, 0xFF, 0x7E }, // eight
        { 0x7E, 0xFF, 0xC3, 0xC3, 0xFF, 0x7F, 0x03, 0x83, 0xFF, 0x7E } // nine
};

char minus[10] = { 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00 };

// für die digitale Darstellung von Zahlen (385 420 y punkte 35)
int zahlen [10] [15] = {
  {5,5,35,5,30,0,0,10,5,5,5,0,0,0,0},//zero
  {1,5,35,0,0,0,0,0,0,0,0,0,0,0,0},//one
  {7,5,20,10,5,10,5,15,5,0,0,0,0,5,10},//two
  {4,5,35,10,5,10,5,10,5,0,0,0,0,0,0},//three
  {3,5,35,5,15,10,5,0,0,0,0,0,0,0,0},//four
  {6,0,0,5,15,15,5,15,5,10,5,5,10,0,0},//five
  {6,0,0,5,30,15,5,15,5,10,5,5,10,0,0},//six
  {2,5,35,10,5,0,0,0,0,0,0,0,0,0,0},//seven
  {5,5,35,5,30,10,5,10,5,5,5,0,0,0,0},//eight
  {5,5,35,5,15,10,5,10,5,5,5,0,0,0,0},//nine
};

/********************************************************************************
 *
 Elementary output functions  => speed optimized as inline
 *********************************************************************************/
inline void write_command(unsigned char command)
{
    GPIO_PORTM_DATA_R = command;        // Write command byte
    GPIO_PORTL_DATA_R = 0x11; // Chip select = 0, Command mode select = 0, Write state = 0
    GPIO_PORTL_DATA_R = 0x1F;           // Initial state
}
/********************************************************************************/
inline void write_data(unsigned char data)
{
    GPIO_PORTM_DATA_R = data;           // Write data byte
    GPIO_PORTL_DATA_R = 0x15;           // Chip select = 0, Write state = 0
    GPIO_PORTL_DATA_R = 0x1F;           // Initial state
}
/********************************************************************************/
inline void window_set( min_x, min_y, max_x, max_y)
{
    write_command(0x2A);           // Set row address x-axis
    write_data(min_x >> 8);        // Set start  address           (high byte)
    write_data(min_x);             // as above                     (low byte)
    write_data(max_x >> 8);        // Set stop address             (high byte)
    write_data(max_x);             // as above                     (low byte)
    write_command(0x2B);           // Set column address (y-axis)
    write_data(min_y >> 8);        // Set start column address     (high byte)
    write_data(min_y);             // as above                     (low byte)
    write_data(max_y >> 8);        // Set stop column address      (high byte)
    write_data(max_y);             // as above                     (low byte)
}
/********************************************************************************/
void init_ports_display(void)
{
    // Set Port M Pins 0-7: used as Output of LCD Data
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);     // enable clock-gate Port M
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM))
        ;     // wait until clock ready
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, 0xFF);
    // Set Port L Pins 0-4: used as Output of LCD Control signals:
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);  // Clock Port Q
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL))
        ;
    GPIOPinTypeGPIOOutput(
            GPIO_PORTL_BASE,
            GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);
}
void configure_display_controller_large(void) // 800 x 480 pixel ???
{
//////////////////////////////////////////////////////////////////////////////////
    GPIO_PORTL_DATA_R = INITIAL_STATE;      // Initial state
    GPIO_PORTL_DATA_R &= ~RST;              // Hardware reset
    SysCtlDelay(10000);                     // wait >1 ms
    GPIO_PORTL_DATA_R |= RST;               //
    SysCtlDelay(12000);                     // wait >1 ms

    write_command(SOFTWARE_RESET);          // Software reset
    SysCtlDelay(120000);                    // wait >10 ms

    write_command(SET_PLL_MN);               // Set PLL Freq to 120 MHz
    write_data(0x24);                        //
    write_data(0x02);                        //
    write_data(0x04);                        //

    write_command(START_PLL);                // Start PLL
    write_data(0x01);                        //
    SysCtlDelay(10000);                      // wait 1 ms

    write_command(START_PLL);                // Lock PLL
    write_data(0x03);                        //
    SysCtlDelay(10000);                      // wait 1 ms

    write_command(SOFTWARE_RESET);           // Software reset
    SysCtlDelay(100000);                     // wait 10 ms

    write_command(0xe6);                    // Set pixel clock frequency
    write_data(0x01);                       // KRR Set LCD Pixel Clock 9MHz
    write_data(0x70);                       // KRR
    write_data(0xA3);                       // KRR

    write_command(SET_LCD_MODE);          // SET LCD MODE SIZE, manual p. 44
    write_data(0x20);                     // ..TFT panel 24bit
    write_data(0x00);                     // ..TFT mode
    write_data(0x03);                 // SET horizontal size = 800-1 (high byte)
    write_data(0x1F);                  // SET horizontal size = 800-1 (low byte)
    write_data(0x01);                   // Set vertical size = 480-1 (high byte)
    write_data(0xDF);                    // Set vertical size = 480-1 (low byte)
    write_data(0x00);      // Even line RGB sequence / Odd line RGB sequence RGB

    write_command(SET_HORI_PERIOD);       // Set Horizontal Period
    write_data(0x03); // Horizontal total period (display + non-displayed)  (high byte)
    write_data(0x5E); // Horizontal total period (display + non-display) (low byte)
    write_data(0x00); // Non-displayed period between the start of the horizontal sync (LLINE) signal and the first displayed data.
    write_data(0x46); // Low byte of the non-display period between the start of the horizontal sync (LLINE) signal and the first display data
    write_data(0x09);                     // Set the vertical sync width
    write_data(0x00);               // Set horiz.Sync pulse start    (high byte)
    write_data(0x08);                // Set horiz.Sync pulse start    (low byte)
    write_data(0x00);                     //

    write_command(SET_VERT_PERIOD);       // Set vertical periods, manual  p. 49
    write_data(0x01); // Vertical total period (display + non-displayed) in lines (high byte)
    write_data(0xFE);                  // as above (low byte) = total 510  lines
    write_data(0x00);                       //
    write_data(0x0C); // The non-displayed period in lines between the start of the frame and the first
                      // display data = 12 line.s
    write_data(0x0A);                // Set the vertiacla sync width = 10 pixels
    write_data(0x00);      // Set vertical sync pulse start position (high byte)
    write_data(0x04); // as above (low byte) = total sync pulse start position is 4 lines

    write_command(SET_ADRESS_MODE); // Pixel address counting = flip display , manual p. 36
    write_data(0x03);         // necessary to match with touch screen addressing

//  write_command(0x0A);                    // Power control mode not tested in detail
//  write_data(0x1C);

    write_command(SET_PIXEL_DATA_FORMAT); // set pixel data format 8bit manual p. 78
    write_data(0x00);

    write_command(SET_DISPLAY_ON);           // Set display on  manual p. 78
}
/********************************************************************************/
void write_rgb(enum colors color)
{
    int i;
    write_command(0x2C);
    for (i = 0; i < (40 * 40); i++) // set pixels
    {
        write_data((color >> 16) & 0xff); // red
        write_data((color >> 8) & 0xff); // green
        write_data((color) & 0xff); // blue
    }
}
/**********************************************************************/
void show_default_display()
{
    int x, y;
    enum colors color;

    // Display initialization
#ifdef LARGE_DISPLAY
#define MAX_X 800
#define MAX_Y 480
#endif
    configure_display_controller_large();  // initalize and  configuration

    printf("Start Background Pixel by Pixel set\n"); // for debug only

    // set pixel by pixel to change the background colors
    color = BLACK;
    window_set(0, 0, MAX_X - 1, MAX_Y - 1); // set single position see B.4  // to do faster ?
    write_command(0x2C); //write pixel command
    for (x = 0; x <= (MAX_X) - 1; x++)
    {
        for (y = 0; y <= (MAX_Y) - 1; y++)
        {
            write_data((color >> 16) & 0xff); // red
            write_data((color >> 8) & 0xff); // green
            write_data((color) & 0xff); // blue
        }
    }
    printf("Background ready \n"); // for debug only

    /**********************************************************************/
    // Rahmen links oben
    color = YELLOW;

    window_set(30, 45, 600, 45);
    write_rgb(color);

    window_set(30, 350, 600, 350);
    write_rgb(color);

    window_set(30, 45, 30, 350);
    write_rgb(color);

    window_set(600, 45, 600, 350);
    write_rgb(color);

    // untere Rahmen
    color = RED;

    window_set(30, 360, 750, 360);
    write_rgb(color);

    window_set(30, 450, 750, 450);
    write_rgb(color);

    window_set(30, 360, 30, 450);
    write_rgb(color);

    window_set(750, 360, 750, 450);
    write_rgb(color);

    // Unterteilung von untere kästchen
    window_set(270, 360, 270, 450);
    write_rgb(color);

    window_set(510, 360, 510, 450);
    write_rgb(color);

    // kästchen oben
    color = BLUE;

    window_set(630, 80, 730, 80);
    write_rgb(color);

    window_set(630, 160, 730, 160);
    write_rgb(color);

    window_set(630, 80, 630, 160);
    write_rgb(color);

    window_set(730, 80, 730, 160);
    write_rgb(color);

    // kästchen unten
    window_set(630, 180, 730, 180);
    write_rgb(color);

    window_set(630, 260, 730, 260);
    write_rgb(color);

    window_set(630, 180, 630, 260);
    write_rgb(color);

    window_set(730, 180, 730, 260);
    write_rgb(color);

    // kästchen in kästchen

    window_set(650, 200, 710, 200);
    write_rgb(YELLOW);

    window_set(650, 240, 710, 240);
    write_rgb(YELLOW);

    window_set(650, 200, 650, 240);
    write_rgb(YELLOW);

    window_set(710, 200, 710, 240);
    write_rgb(YELLOW);

    draw_bresenham_line(680, 230, 680 - 8, 230 - 20, GREEN);
    draw_bresenham_line(680, 230, 680 + 8, 230 - 20, GREEN);

    print_axis();
    print_C(55, 380);
    print_C(295, 380);
    print_H(55, 380);
    print_H(295, 380);
    print_number(85, 380, numbers[1], GREEN);
    print_number(325, 380, numbers[2], GREEN);
    print_F(55, 410);
    print_F(295, 410);
    print_r(80, 420);
    print_r(320, 420);
    print_V(130, 420);
    print_V(370, 420);
    print_V(530, 420);
    draw_cursor_sign(710, 400, 20, BLUE);
    draw_cursor_sign(680, 120, 20, YELLOW);


    // default wert anzeigen:
    print_Spanungszahl(160,0);
    print_komma(180);
    print_Spanungszahl(188,0);
    print_Spanungszahl(208,0);
    print_Spanungszahl(228,0);

    print_Spanungszahl(400,0);
    print_komma(420);
    print_Spanungszahl(428,0);
    print_Spanungszahl(448,0);
    print_Spanungszahl(468,0);

    print_Spanungszahl(560,0);
    print_komma(580);
    print_Spanungszahl(588,0);
    print_Spanungszahl(608,0);
    print_Spanungszahl(628,0);

}
/**********************************************************************/
void print_number(int xbegin, int ybegin, char *zahl_array, enum colors color)
{
    if (zahl_array != NULL)
    {
        char *number;
        char temp;
        number = zahl_array;
        temp = *number;
        int x, y, count = 0;

        for (y = ybegin; y <= ybegin + 10; y++)
        {
            for (x = xbegin; x <= xbegin + 8; x++)
            {
                if (((temp) & 0x80) == 0x80) // wenn die letzte zahl 1 dann print
                {
                    window_set(x, y, x, y);
                    write_rgb(color);
                }

                temp = (temp) << 1; // number shiften
            }

            number++;
            temp = *number;
            count++;
            if (count >= 9)
            {
                break;
            }
        }
    }
    else
    {
        printf("Null Pointer wurde übergeben!\n");
    }
}
/**********************************************************************/
void print_axis()
{
    int iteration, j = 1, n = 2;
    enum colors color;
    color = WHITE;
    int x_achse = 60, y_achse = 50;

    // y axis
    window_set(x_achse, y_achse, x_achse, 340);
    write_rgb(color);

    // x axis
    window_set(x_achse, 200, 590, 200);
    write_rgb(color);

    // scale X
    print_number(x_achse + 5, y_achse + 155, numbers[0], YELLOW);
    for (iteration = 0; iteration <= 8; iteration++)
    {
        x_achse += 50;
        window_set(x_achse, 198, x_achse, 202);
        write_rgb(color);
        if (iteration >= 9)
            break;
        print_number(x_achse, y_achse + 155, numbers[iteration + 1], YELLOW);
    }

    // last scale x
    window_set(570, 198, 570, 202);
    write_rgb(color);
    print_number(570 - 10, y_achse + 155, numbers[1], YELLOW);
    print_number(570, y_achse + 155, numbers[0], YELLOW);

    // scale Y
    for (iteration = 0; iteration <= 4; iteration++)
    {
        y_achse += 50;
        window_set(58, y_achse, 62, y_achse);
        write_rgb(color);
        if (y_achse < 200 && y_achse > 50)
        {
            print_number(75, y_achse, numbers[n], YELLOW);
            n--;
        }
        if (y_achse > 200)
        {
            print_number(65, y_achse, minus, YELLOW);
            print_number(75, y_achse, numbers[j], YELLOW);
            j++;
        }
    }
}
/**********************************************************************/
void draw_bresenham_line(int x1, int y1, int x2, int y2, enum colors color)
{

    int error, error2; //Variablen

    int deltaX = abs(x2 - x1), sX = x1 < x2 ? 1 : -1;
    int deltaY = -abs(y2 - y1), sY = y1 < y2 ? 1 : -1;

    //nun Fehler berechnen:
    error = deltaX + deltaY;

    while (1)
    {
        // angepasste Punkt zeichnen:
        window_set(x1, y1, x1, y1);
        write_rgb(color);

        // überprüfen, ob das ende erreicht wurde
        if (x1 == x2 && y1 == y2)
        {
            break;
        }

        // hier fehler aktualisieren!
        error2 = 2 * error;

        // abhängig von der Steigung x1 und y1 anpassen
        if (error2 >= deltaY)
        {
            error += deltaY; //fehler anpassen
            x1 += sX;        //abstand anpassen
        }
        if (error2 <= deltaX)
        {
            error += deltaX;
            y1 += sY;
        }

    }
}
/**********************************************************************/
void draw_bresenham_circle(int x_mitte, int y_mitte, int r, enum colors color)
{
    int x = 0;
    int y = r;
    int d = 3 * 2 - r;

    while (x <= y)
    {
        //Pixel setzen und dann Fehler aktualisieren: (Oktantenweise)

        window_set(x_mitte + x, y_mitte + y, x_mitte + x, y_mitte + y);
        write_rgb(color);

        window_set(x_mitte - x, y_mitte + y, x_mitte - x, y_mitte + y);
        write_rgb(color);

        window_set(x_mitte - x, y_mitte - y, x_mitte - x, y_mitte - y);
        write_rgb(color);

        window_set(x_mitte + x, y_mitte - y, x_mitte + x, y_mitte - y);
        write_rgb(color);

        window_set(x_mitte - y, y_mitte - x, x_mitte - y, y_mitte - x);
        write_rgb(color);

        window_set(x_mitte - y, y_mitte + x, x_mitte - y, y_mitte + x);
        write_rgb(color);

        window_set(x_mitte + y, y_mitte - x, x_mitte + y, y_mitte - x);
        write_rgb(color);

        window_set(x_mitte + y, y_mitte + x, x_mitte + y, y_mitte + x);
        write_rgb(color);

        if (d < 0)
        {
            d = d + 2 * x + 3;
        }
        else
        {
            // diagonale Schritt: y muss verringert werden
            d = d + 2 * x - 2 * y + 5;
            y--;
        }
        // egal ob horizontal oder diagonal: x muss erhöht werden
        x++;
    }
}
/**********************************************************************/
void print_C(int x_mitte, int y_mitte)
{
    enum colors color;
    color = GREEN;
    int r = 10;
    int x = 0, y = r, d = 3 * 2 - r;

    draw_bresenham_line(x_mitte, y_mitte + r, x_mitte + 5, y_mitte + r, GREEN); // y koordinate gespiegelt! +r unten -r oben
    draw_bresenham_line(x_mitte, y_mitte - r, x_mitte + 5, y_mitte - r, GREEN);

    while (x <= y)
    {
        window_set(x_mitte - y, y_mitte - x, x_mitte - y, y_mitte - x);
        write_rgb(color);

        window_set(x_mitte - y, y_mitte + x, x_mitte - y, y_mitte + x);
        write_rgb(color);

        window_set(x_mitte - x, y_mitte + y, x_mitte - x, y_mitte + y);
        write_rgb(color);

        window_set(x_mitte - x, y_mitte - y, x_mitte - x, y_mitte - y);
        write_rgb(color);

        if (d < 0)
        {
            d = d + 2 * x + 3;
        }
        else
        {
            d = d + 2 * x - 2 * y + 5;
            y--;
        }
        x++;
    }
}
/**********************************************************************/
void print_H(int x_mitte, int y_mitte)
{
    window_set(x_mitte + 10, y_mitte - 10, x_mitte + 10, y_mitte + 10);
    write_rgb(GREEN);

    window_set(x_mitte + 25, y_mitte - 10, x_mitte + 25, y_mitte + 10);
    write_rgb(GREEN);

    window_set(x_mitte + 10, y_mitte, x_mitte + 25, y_mitte);
    write_rgb(GREEN);
}
/**********************************************************************/
void print_F(int x_mitte, int y_mitte)
{
    window_set(x_mitte, y_mitte - 10, x_mitte, y_mitte + 10);
    write_rgb(GREY);
    window_set(x_mitte, y_mitte - 10, x_mitte + 10, y_mitte - 10);
    write_rgb(GREY);
    window_set(x_mitte, y_mitte, x_mitte + 8, y_mitte);
    write_rgb(GREY);
}
/**********************************************************************/
void print_r(int x_mitte, int y_mitte)
{
    window_set(x_mitte - 10, y_mitte - 12, x_mitte - 10, y_mitte);
    write_rgb(GREY);
    enum colors color;
    color = GREY;
    int r = 10;
    int x = 0, y = r, d = 3 * 2 - r;
    while (x <= y)
    {
        window_set(x_mitte - y, y_mitte - x, x_mitte - y, y_mitte - x);
        write_rgb(color);

        window_set(x_mitte - x, y_mitte - y, x_mitte - x, y_mitte - y);
        write_rgb(color);

        if (d < 0)
        {
            d = d + 2 * x + 3;
        }
        else
        {
            d = d + 2 * x - 2 * y + 5;
            y--;
        }
        x++;
    }

}
/**********************************************************************/
void print_V(int x_mitte, int y_mitte)
{
    draw_bresenham_line(x_mitte, y_mitte, x_mitte - 10, y_mitte - 35, RED);
    draw_bresenham_line(x_mitte, y_mitte, x_mitte + 10, y_mitte - 35, RED);

}
/**********************************************************************/
void print_a(int x_mitte, int y_mitte)
{
    enum colors color;
    color = BLUE;
    int r = 7;
    int x = 0, y = r, d = 3 * 2 - r;

    draw_bresenham_line(x_mitte, y_mitte + r, x_mitte + 5, y_mitte + r, BLUE);
    draw_bresenham_line(x_mitte, y_mitte - r, x_mitte + 5, y_mitte - r, BLUE);

    while (x <= y)
    {
        window_set(x_mitte - y, y_mitte - x, x_mitte - y, y_mitte - x);
        write_rgb(color);

        window_set(x_mitte - x, y_mitte - y, x_mitte - x, y_mitte - y);
        write_rgb(color);

        window_set(x_mitte - y, y_mitte + x, x_mitte - y, y_mitte + x);
        write_rgb(color);

        window_set(x_mitte - x, y_mitte + y, x_mitte - x, y_mitte + y);
        write_rgb(color);

        window_set(x_mitte - y, y_mitte - x - 10, x_mitte - y,
                   y_mitte - x - 10);
        write_rgb(color);

        window_set(x_mitte - x, y_mitte - y - 10, x_mitte - x,
                   y_mitte - y - 10);
        write_rgb(color);

        window_set(x_mitte + x, y_mitte - y - 10, x_mitte + x,
                   y_mitte - y - 10);
        write_rgb(color);

        if (d < 0)
        {
            d = d + 2 * x + 3;
        }
        else
        {
            d = d + 2 * x - 2 * y + 5;
            y--;
        }
        x++;
    }
    window_set(x_mitte + 6, y_mitte - 13, x_mitte + 6, y_mitte + 6);
    write_rgb(BLUE);

}
/**********************************************************************/
void draw_cursor_sign(int x, int y, int r, enum colors color)
{
    draw_bresenham_circle(x, y, r, BLUE);
    print_a(x, y + 5);

}
/**********************************************************************/
void print_komma(int x)//
{
    enum colors color;
    color = WHITE;
    window_set(x, 417, x+3, 427);
    write_rgb(color);
}
/**********************************************************************/
void print_Spanungszahl(int x,int nummer)// (x,y) oben rechts
{
    enum colors color;
    color = WHITE;
    int p=0,zahl[15];
    for (p=0;p<15;p++){
        zahl[p] = zahlen [nummer][p];
   }
    int x_plus,y_plus,i=zahl[0],y = 385;
    int x_min,x_max,y_min,y_max;
    int position_plus [14] = {10,0,0,0,0,15,0,30,5,0,10,20,0,20};
    for(p=0;p<i;p++){
        x_plus = zahl [1+p*2];
        y_plus = zahl [2+p*2];
     if(x_plus!=0){
         x_min = x+position_plus [p*2];
         x_max = x +position_plus [p*2]+ x_plus;
         y_min = y+position_plus [1+p*2];
         y_max = y +position_plus [1+p*2]+ y_plus;
    window_set(x_min, y_min, x_max, y_max);
    write_rgb(color);
     }
    }
    }
/**********************************************************************/
void print_doppelpunkt(int x_mitte, int y_mitte)
{
    enum colors color;
    color = GREY;
    window_set(x_mitte, y_mitte, x_mitte + 3, y_mitte + 3);
    write_rgb(color);
    window_set(x_mitte, y_mitte -32, x_mitte + 3, y_mitte - 29);
    write_rgb(color);
}
/**********************************************************************/
