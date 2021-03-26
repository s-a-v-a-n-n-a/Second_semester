#include <TXLib.h>
#include <emmintrin.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const float horizontal_size = 300;
const float vertical_size   = 300;
const int radius = 2;
const int maximum_iterations_amount = 255;

typedef union array_to_register_floats
{
	__m128 xmm;
	float floats[4];
} xmm;

typedef union array_to_register_ints
{
	__m128i regs;
	int ints[4];
} reg;

typedef struct screen_information
{
	RGBQUAD *screen;
	float horizontal_screen_size;
	float vertical_screen_size;

	float real_horizontal_size;
	float real_vertical_size;

	float center_x;
	float center_y;

	float dx;
	float dy;
}s_information;

s_information *screen_new();
void screen_set_pixel_color(s_information *screen, long long pixel, char red, char green, char blue);
void screen_delete(s_information *screen);
void screen_update_information(s_information *screen);

inline xmm four_float_set_one_value (float value);
inline reg four_int_set_one_value   (int value);
inline xmm four_float_multiply (xmm array_1, xmm array_2);
inline xmm four_float_add      (xmm array_1, xmm array_2);
inline reg four_float_add_with_int (reg array_1, xmm array_2);
inline xmm four_float_sub          (xmm array_1, xmm array_2);
inline xmm four_float_copy (xmm array_with);

inline xmm four_float_compare(xmm array_with, xmm result);
inline int four_flat_make_mask(xmm float_mask);

void draw_mandelbrot();

int main()
{
    draw_mandelbrot();

	return 0;
}

s_information *screen_new()
{
	s_information *screen = (s_information*)calloc(1, sizeof(s_information));
	screen->screen = (RGBQUAD*)txVideoMemory();

	screen->horizontal_screen_size = horizontal_size;
	screen->vertical_screen_size   = vertical_size;

	screen->real_horizontal_size = 2 * radius;
	screen->real_vertical_size   = 2 * radius;

	screen->dx = (screen->real_horizontal_size/screen->horizontal_screen_size);
	screen->dy = (screen->real_vertical_size/screen->vertical_screen_size);

	screen->center_x = 0;
	screen->center_y = 0;

	return screen;
}

void screen_set_pixel_color(s_information *screen, long long pixel, char red, char green, char blue)
{
	screen->screen[pixel] = {(BYTE) red, (BYTE) green, (BYTE) blue};
}

void screen_delete(s_information *screen)
{
	free(screen);
}

void screen_update_information(s_information *screen)
{
	if (txGetAsyncKeyState (VK_RIGHT)) screen->center_x += screen->dx * (txGetAsyncKeyState (VK_SHIFT)? 50.f : 5.f);
    if (txGetAsyncKeyState (VK_LEFT))  screen->center_x -= screen->dx * (txGetAsyncKeyState (VK_SHIFT)? 50.f : 5.f);
    if (txGetAsyncKeyState (VK_DOWN))  screen->center_y -= screen->dy * (txGetAsyncKeyState (VK_SHIFT)? 50.f : 5.f);
    if (txGetAsyncKeyState (VK_UP))    screen->center_y += screen->dy * (txGetAsyncKeyState (VK_SHIFT)? 50.f : 5.f);
    if (txGetAsyncKeyState ('A'))
    {
    	screen->real_horizontal_size /= (txGetAsyncKeyState (VK_SHIFT)? 50.f : 5.f);
    	screen->real_vertical_size   /= (txGetAsyncKeyState (VK_SHIFT)? 50.f : 5.f);
    }
    if (txGetAsyncKeyState ('Z'))
    {
    	screen->real_horizontal_size *= (txGetAsyncKeyState (VK_SHIFT)? 50.f : 5.f);
    	screen->real_vertical_size   *= (txGetAsyncKeyState (VK_SHIFT)? 50.f : 5.f);
    }

    screen->dx = (screen->real_horizontal_size/screen->horizontal_screen_size);
	screen->dy = (screen->real_vertical_size/screen->vertical_screen_size);
}


inline xmm four_float_set_one_value (float value)
{
	xmm result = {.floats = {0, 0, 0, 0}};
	for (int k = 0; k < 4; k++)
		result.floats[k] = value;
	return result;
}

inline reg four_int_set_one_value (int value)
{
	reg result = {.ints = {0, 0, 0, 0}};
	for (int k = 0; k < 4; k++)
		result.ints[k] = value;
	return result;
}

inline xmm four_float_multiply (xmm array_1, xmm array_2)
{
	xmm result = {.floats = {0, 0, 0, 0}};
	for (int k = 0; k < 4; k++)
		result.floats[k] = array_1.floats[k] * array_2.floats[k];
	return result;
}

inline xmm four_float_add (xmm array_1, xmm array_2)
{
	xmm result = {.floats = {0, 0, 0, 0}};
	for (int k = 0; k < 4; k++)
		result.floats[k] = array_1.floats[k] + array_2.floats[k];
	return result;
}

inline reg four_float_add_with_int (reg array_1, xmm array_2)
{
	reg result = {.ints = {0, 0, 0, 0}};
	for (int k = 0; k < 4; k++)
		result.ints[k] = array_1.ints[k] + (int)array_2.floats[k];
	return result;
}

inline xmm four_float_sub (xmm array_1, xmm array_2)
{
	xmm result = {.floats = {0, 0, 0, 0}};
	for (int k = 0; k < 4; k++)
		result.floats[k] = array_1.floats[k] - array_2.floats[k];
	return result;
}

inline xmm four_float_copy (xmm array_with)
{
	xmm result = {.floats = {0, 0, 0, 0}};
	for (int k = 0; k < 4; k++)
		result.floats[k] = array_with.floats[k];
	return result;
}

inline xmm four_float_compare(xmm array_to, xmm array_with)
{
	xmm result = {.floats = {0, 0, 0, 0}};
	for (int k = 0; k < 4; k++)
		if (array_to.floats[k] <= array_with.floats[k])
			result.floats[k] = 1;
		else
			result.floats[k] = 0;
	return result;
}

inline int four_flat_make_mask(xmm float_mask)
{
	int mask = 0;
	for (int k = 0; k < 4; k++)
	{
		mask = mask * 2 + (int) float_mask.floats[k];
	}
	return mask;
}

void draw_mandelbrot()
{
	txCreateWindow (horizontal_size, vertical_size);
    Win32::_fpreset();
    txBegin();

    s_information *mandelbrot_screen = screen_new();

	int height = (int)(mandelbrot_screen->vertical_screen_size);
	int width = (int)(mandelbrot_screen->horizontal_screen_size);

	xmm square_radius = four_float_set_one_value(radius * radius);

	while(true)
	{
		if (GetAsyncKeyState (VK_ESCAPE)) 
			break;

		screen_update_information(mandelbrot_screen);

		float left_corner_x = mandelbrot_screen->center_x - mandelbrot_screen->real_horizontal_size/2;
		float left_corner_y = mandelbrot_screen->center_y - mandelbrot_screen->real_vertical_size/2;

		for (int i = 0; i < height; i++)
		{
			if (GetAsyncKeyState (VK_ESCAPE)) 
				break;

			int current_line = i * width;
			for (int j = 0; j + 3 < width; j += 4)
			{
				xmm points_x_0 = four_float_set_one_value(mandelbrot_screen->dx);
				
				xmm zero_one_two_three = {.floats = {0, 1, 2, 3}}; 
				xmm j_value = four_float_add(four_float_set_one_value((float)j), zero_one_two_three);
				points_x_0 = four_float_add(four_float_multiply(points_x_0, j_value), four_float_set_one_value(left_corner_x));

				xmm points_y_0 = four_float_set_one_value(left_corner_y + (float)i * mandelbrot_screen->dy); 

				xmm points_x = four_float_copy(points_x_0);
	 			xmm points_y = four_float_copy(points_y_0);

				xmm place   = four_float_set_one_value(1);
				reg counter = four_int_set_one_value(0);
				int common_counter = 0;

				while (common_counter < maximum_iterations_amount)
				{
					xmm square_points_x = four_float_multiply (points_x, points_x);
					xmm square_points_y = four_float_multiply (points_y, points_y);

					xmm tmp_points_x = four_float_add(four_float_sub (square_points_x, square_points_y), points_x_0); 
					xmm tmp_points_y = four_float_multiply (points_x, points_y);
					tmp_points_y = four_float_add(four_float_add(tmp_points_y, tmp_points_y), points_y_0);

					xmm tmp_points_square_x = four_float_multiply (tmp_points_x, tmp_points_x);
					xmm tmp_points_square_y = four_float_multiply (tmp_points_y, tmp_points_y);

					xmm distances_from_center = four_float_add (tmp_points_square_x, tmp_points_square_y);

					place = four_float_compare (distances_from_center, square_radius);
					int mask = four_flat_make_mask(place);
					if (!mask)
						break;

					counter = four_float_add_with_int(counter, place);
					points_x = four_float_copy (tmp_points_x); points_y = four_float_copy (tmp_points_y);

					common_counter++;
				}

				for (int k = 0; k < 4; k++)
					screen_set_pixel_color(mandelbrot_screen, current_line + j + k, (char)counter.ints[k], 128, 255 - (char)counter.ints[k]);
			}
		}
		printf ("\t\r%.0lf", txGetFPS());
       	txSleep();
	}

	screen_delete(mandelbrot_screen);
}
