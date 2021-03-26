#include <TXLib.h>
#include <emmintrin.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const float horizontal_size = 300;
const float vertical_size = 300;
const int radius = 2;
const int maximum_iterations_amount = 255;

typedef union array_to_register
{
	__m128 xmm;
	float floats[4];

	__m128i regs;
	int ints[4];
} xmm;

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

inline void four_float_set_one_value (xmm array_to, float value);
inline void four_int_set_one_value (xmm array_to, int value);
inline void four_float_multiply (xmm array_to, xmm array_1, xmm array_2);
inline void four_float_add (xmm array_to, xmm array_1, xmm array_2);
inline void four_float_add_with_int (xmm array_to, xmm array_1, xmm array_2);
inline void four_float_sub (xmm array_to, xmm array_1, xmm array_2);
inline void four_float_copy (xmm array_to, xmm array_with);

inline void four_float_compare(xmm array_to, xmm array_with, xmm result);
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
	if (txGetAsyncKeyState (VK_RIGHT)) screen->center_x += screen->dx * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
    if (txGetAsyncKeyState (VK_LEFT))  screen->center_x -= screen->dx * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
    if (txGetAsyncKeyState (VK_DOWN))  screen->center_y -= screen->dy * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
    if (txGetAsyncKeyState (VK_UP))    screen->center_y += screen->dy * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
    if (txGetAsyncKeyState ('A'))
    {
    	screen->real_horizontal_size /= (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
    	screen->real_vertical_size   /= (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
    }
    if (txGetAsyncKeyState ('Z'))
    {
    	screen->real_horizontal_size *= (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
    	screen->real_vertical_size   *= (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
    }

    screen->dx = (screen->real_horizontal_size/screen->horizontal_screen_size);
	screen->dy = (screen->real_vertical_size/screen->vertical_screen_size);
}


inline void four_float_set_one_value (xmm array_to, float value)
{
	for (int k = 0; k < 4; k++)
		array_to.floats[k] = value;
}

inline void four_int_set_one_value (xmm array_to, int value)
{
	for (int k = 0; k < 4; k++)
		array_to.ints[k] = value;
}

inline void four_float_multiply (xmm array_to, xmm array_1, xmm array_2)
{
	for (int k = 0; k < 4; k++)
		array_to.floats[k] = array_1.floats[k] * array_2.floats[k];
}

inline void four_float_add (xmm array_to, xmm array_1, xmm array_2)
{
	for (int k = 0; k < 4; k++)
		array_to.floats[k] = array_1.floats[k] + array_2.floats[k];
}

inline void four_float_add_with_int (xmm array_to, xmm array_1, xmm array_2)
{
	for (int k = 0; k < 4; k++)
		array_to.ints[k] = array_1.ints[k] + array_2.ints[k];
}

inline void four_float_sub (xmm array_to, xmm array_1, xmm array_2)
{
	for (int k = 0; k < 4; k++)
		array_to.floats[k] = array_1.floats[k] - array_2.floats[k];
}

inline void four_float_copy (xmm array_to, xmm array_with)
{
	for (int k = 0; k < 4; k++)
		array_to.floats[k] = array_with.floats[k];
}

inline void four_float_compare(xmm array_to, xmm array_with, xmm result)
{
	for (int k = 0; k < 4; k++)
		if (array_to.floats[k] <= array_with.floats[k])
			result.floats[k] = 1;
		else
			result.floats[k] = 0;
}

inline int four_flat_make_mask(xmm float_mask)
{
	int mask = 0;
	for (int k = 0; k < 4; k++)
	{
		mask = mask * 2 + float_mask.floats[k];
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

	xmm square_radius = {}; four_float_set_one_value(square_radius, radius * radius);

	while(true)
	{
		if (GetAsyncKeyState (VK_ESCAPE)) 
			break;

		screen_update_information(mandelbrot_screen);

		for (int i = 0; i < height; i++)
		{
			if (GetAsyncKeyState (VK_ESCAPE)) 
				break;

			long long current_line = i * width;
			for (int j = 0; j + 3 < width; j += 4)
			{
				xmm points_x_0 = {}; four_float_set_one_value(points_x_0, mandelbrot_screen->dx);
				xmm j_value = {}; four_float_set_one_value(j_value, j);
				xmm zero_one_two_three = {}; 
				zero_one_two_three.floats[0] = 0;
				zero_one_two_three.floats[1] = 1;
				zero_one_two_three.floats[2] = 2;
				zero_one_two_three.floats[3] = 3;
				xmm left_corners_x = {}; four_float_set_one_value(left_corners_x, left_corner_x);
				
				four_float_add          (j_value, j_value, zero_one_two_three);
				four_float_multiply     (points_x_0, points_x_0, j_value);
				four_float_add          (points_x_0, points_x_0, left_corners_x);

				xmm points_y_0 = {}; four_float_set_one_value(points_y_0, left_corner_y + i * mandelbrot_screen->dy); 

				xmm points_x = {}; four_float_copy(points_x, points_x_0);
	 			xmm points_y = {}; four_float_copy(points_y, points_y_0);

				xmm place = {}; four_float_set_one_value(place, 1);
				xmm counter = {}; four_int_set_one_value(counter, 0);
				int common_counter = 0;

				while (common_counter < maximum_iterations_amount)
				{
					xmm square_points_x = {}; four_float_multiply (square_points_x, points_x, points_x);
					xmm square_points_y = {}; four_float_multiply (square_points_y, points_y, points_y);

					xmm tmp_points_x = {};
					four_float_sub (tmp_points_x, square_points_x, square_points_y); 
					four_float_add (tmp_points_x, tmp_points_x, points_x_0);

					xmm tmp_points_y = {}; four_float_multiply (tmp_points_y, points_x, points_y);
					four_float_add (tmp_points_y, tmp_points_y, tmp_points_y); four_float_add (tmp_points_y, tmp_points_y, points_y_0);

					xmm tmp_points_square_x = {}; four_float_multiply (tmp_points_square_x, tmp_points_x, tmp_points_x);

					xmm tmp_points_square_y = {}; four_float_multiply (tmp_points_square_y, tmp_points_y, tmp_points_y);

					xmm distances_from_center = {}; four_float_add (distances_from_center, tmp_points_square_x, tmp_points_square_y);

					four_float_compare  (distances_from_center, square_radius, place);
					int mask = four_flat_make_mask(place);
					if (!mask)
						break;

					four_float_add_with_int(counter, counter, place);
					four_float_copy (points_x, tmp_points_x); four_float_copy (points_y, tmp_points_y);

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
