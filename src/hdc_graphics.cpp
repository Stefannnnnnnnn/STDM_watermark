/*
 * hdc_graphics.cpp
 * Author: Tianyi Li
 * Date: 2024.02.26
 *
 * This file contains functions for initializing a device context (HDC), 
 * setting pen colors, clearing the screen, and drawing pixels on a console window.
 */

#include <Windows.h>

/* Declaration for GetConsoleWindow, used to obtain the console's window handle */
extern "C" WINBASEAPI HWND WINAPI GetConsoleWindow(); // This may show a warning in VS but does not affect functionality

/* Static global variables */
static HWND hWnd = GetConsoleWindow();
static HDC hdc = NULL;
static int _BgColor_, _FgColor_, _Width_, _High;

/***************************************************************************
  Function Name: hdc_init
  Functionality: Initialize the device context with specified colors and size.
  Input Parameters: 
    const int bgcolor: Background color
    const int fgcolor: Foreground color
    const int width: Screen width (in pixels)
    const int high: Screen height (in pixels)
  Return Value: None
  Remarks: This function must be called before drawing.
***************************************************************************/
void hdc_init(const int bgcolor, const int fgcolor, const int width, const int high)
{
    void hdc_release(); // Forward declaration

    /* Release any existing device context to avoid issues with reinitialization */
    hdc_release();

    /* Record the initialization parameters for future use */
    _BgColor_ = bgcolor;
    _FgColor_ = fgcolor;
    _Width_ = width;
    _High = high;

    hdc = GetDC(hWnd);
}

/***************************************************************************
  Function Name: hdc_release
  Functionality: Release the drawing resources.
  Input Parameters: None
  Return Value: None
  Remarks: This function is reentrant.
***************************************************************************/
void hdc_release()
{
    if (hdc) {
        ReleaseDC(hWnd, hdc);
        hdc = NULL;
    }
}

/***************************************************************************
  Function Name: hdc_set_pencolor (overloaded)
  Functionality: Set the pen color using a single RGB value.
  Input Parameters: 
    const int RGB_value: The RGB color value
  Return Value: None
  Remarks: None
***************************************************************************/
void hdc_set_pencolor(const int RGB_value)
{
    SelectObject(hdc, GetStockObject(DC_PEN));
    SetDCPenColor(hdc, RGB_value);
}

/***************************************************************************
  Function Name: hdc_set_pencolor (overloaded)
  Functionality: Set the pen color using separate RGB components.
  Input Parameters: 
    const unsigned char red: Red component (0-255)
    const unsigned char green: Green component (0-255)
    const unsigned char blue: Blue component (0-255)
  Return Value: None
  Remarks: None
***************************************************************************/
void hdc_set_pencolor(const unsigned char red, const unsigned char green, const unsigned char blue)
{
    hdc_set_pencolor(RGB(red, green, blue));
}

/***************************************************************************
  Function Name: hdc_cls
  Functionality: Clear the existing graphics on the screen.
  Input Parameters: None
  Return Value: None
  Remarks: None
***************************************************************************/
void hdc_cls()
{
    /* Change the color during initialization to ensure all pixels are cleared */
    hdc_init(_BgColor_, (_FgColor_ + 1) % 16, _Width_, _High);
    hdc_init(_BgColor_, _FgColor_, _Width_, _High);

    /* Add a delay to prevent missing lines on certain machines */
    Sleep(30);
}

/***************************************************************************
  Function Name: hdc_base_point
  Functionality: Draw a pixel at the specified (x, y) position using the current color.
  Input Parameters: 
    const int x: X coordinate, with the top-left corner as (0, 0)
    const int y: Y coordinate, with the top-left corner as (0, 0)
  Return Value: None
  Remarks: The color used is the current pen color.
***************************************************************************/
void hdc_base_point(const int x, const int y)
{
    MoveToEx(hdc, x - 1, y - 1, NULL);
    LineTo(hdc, x, y);
}