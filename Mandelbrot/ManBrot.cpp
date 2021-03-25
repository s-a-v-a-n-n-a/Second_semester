#include <TXLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const int horizontal_size = 300;
const int vertical_size = 300;
const int radius = 2;
const int maximum_iterations_amount = 255;

const float dx = 0.001;
const float epsilon = 0.0000001;

typedef struct screen_information
{
	RGBQUAD *screen;
	int horizontal_screen_size;
	int vertical_screen_size;

	float real_horizontal_size;
	float real_vertical_size;

	float center_x;
	float center_y;

	float dx;
	float dy;
}s_information;

s_information *screen_new();
void screen_set_pixel_color(s_information *screen, long long pixel, char red, char green, char blue, char alpha);
void screen_delete(s_information *screen);
void screen_update_information(s_information *screen);

void four_float_set_one_value (float *array_to, float value);
void four_int_set_one_value (int *array_to, int value);
void four_float_multiply (float *array_to, float *array_1, float *array_2);
void four_float_add (float *array_to, float *array_1, float *array_2);
void four_float_add_with_int (int *array_to, int *array_1, float *array_2);
void four_float_sub (float *array_to, float *array_1, float *array_2);
void four_float_copy (float *array_to, float *array_with);

void four_float_compare(float *array_to, float *array_with, float *result);
int four_flat_make_mask(float *float_mask);

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

void screen_set_pixel_color(s_information *screen, long long pixel, char red, char green, char blue, char alpha)
{
	screen->screen[pixel] = {(BYTE) red, (BYTE) green, (BYTE) blue};
}

float screen_get_dx(s_information *screen)
{
	return screen->dx;
}

float screen_get_dy(s_information *screen)
{
	return screen->dy;
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


void four_float_set_one_value (float *array_to, float value)
{
	for (int k = 0; k < 4; k++)
		array_to[k] = value;
}

void four_int_set_one_value (int *array_to, int value)
{
	for (int k = 0; k < 4; k++)
		array_to[k] = value;
}

void four_float_multiply (float *array_to, float *array_1, float *array_2)
{
	for (int k = 0; k < 4; k++)
		array_to[k] = array_1[k] * array_2[k];
}

void four_float_add (float *array_to, float *array_1, float *array_2)
{
	for (int k = 0; k < 4; k++)
		array_to[k] = array_1[k] + array_2[k];
}

void four_float_add_with_int (int *array_to, int *array_1, float *array_2)
{
	for (int k = 0; k < 4; k++)
		array_to[k] = array_1[k] + (int)array_2[k];
}

void four_float_sub (float *array_to, float *array_1, float *array_2)
{
	for (int k = 0; k < 4; k++)
		array_to[k] = array_1[k] - array_2[k];
}

void four_float_copy (float *array_to, float *array_with)
{
	for (int k = 0; k < 4; k++)
		array_to[k] = array_with[k];
}

void four_float_compare(float *array_to, float *array_with, float *result)
{
	for (int k = 0; k < 4; k++)
		if (array_to[k] <= array_with[k])
			result[k] = 1;
		else
			result[k] = 0;
}

int four_flat_make_mask(float *float_mask)
{
	int mask = 0;
	for (int k = 0; k < 4; k++)
	{
		mask = mask * 2 + float_mask[k];
	}
	return mask;
}

void draw_mandelbrot()
{
	txCreateWindow (horizontal_size, vertical_size);
    Win32::_fpreset();
    txBegin();

    s_information *mandelbrot_screen = screen_new();

	long long height = mandelbrot_screen->vertical_screen_size;
	long long width = mandelbrot_screen->horizontal_screen_size;

	float square_radius[4] = {}; four_float_set_one_value(square_radius, radius * radius);

	while(true)
	{
		if (GetAsyncKeyState (VK_ESCAPE)) 
			break;

		screen_update_information(mandelbrot_screen);

		float center_x = mandelbrot_screen->center_x;
		float center_y = mandelbrot_screen->center_y;

		float left_corner_x = mandelbrot_screen->center_x - mandelbrot_screen->real_horizontal_size/2;
		float left_corner_y = mandelbrot_screen->center_y - mandelbrot_screen->real_vertical_size/2;

		for (int i = 0; i < height; i++)
		{
			if (GetAsyncKeyState (VK_ESCAPE)) 
				break;

			long long current_line = i * width;
			for (int j = 0; j + 3 < width; j += 4)
			{
				float points_x_0[4]         = {}; four_float_set_one_value(points_x_0, mandelbrot_screen->dx);
				float j_value[4]            = {}; four_float_set_one_value(j_value, j);
				float zero_one_two_three[4] = {0, 1, 2, 3};
				float left_corners_x[4]     = {}; four_float_set_one_value(left_corners_x, left_corner_x);
				
				four_float_add          (j_value, j_value, zero_one_two_three);
				four_float_multiply     (points_x_0, points_x_0, j_value);
				four_float_add          (points_x_0, points_x_0, left_corners_x);

				float points_y_0[4] = {}; four_float_set_one_value(points_y_0, left_corner_y + i * mandelbrot_screen->dy);

				float points_x[4] = {}; four_float_copy(points_x, points_x_0);
	 			float points_y[4] = {}; four_float_copy(points_y, points_y_0);

				float place[4] = {}; four_float_set_one_value(place, 1);
				int counter[4] = {}; four_int_set_one_value(counter, 0);
				int common_counter = 0;

				while (common_counter < maximum_iterations_amount)
				{
					float square_points_x[4] = {}; four_float_multiply (square_points_x, points_x, points_x);
					float square_points_y[4] = {}; four_float_multiply (square_points_y, points_y, points_y);

					float tmp_points_x[4] = {}; 
					four_float_sub (tmp_points_x, square_points_x, square_points_y); 
					four_float_add (tmp_points_x, tmp_points_x, points_x_0);

					float tmp_points_y[4]    = {}; four_float_multiply (tmp_points_y, points_x, points_y);
					four_float_add (tmp_points_y, tmp_points_y, tmp_points_y); four_float_add (tmp_points_y, tmp_points_y, points_y_0);

					float tmp_points_square_x[4] = {}; four_float_multiply (tmp_points_square_x, tmp_points_x, tmp_points_x);

					float tmp_points_square_y[4] = {}; four_float_multiply (tmp_points_square_y, tmp_points_y, tmp_points_y);

					float distances_from_center[4] = {}; four_float_add (distances_from_center, tmp_points_square_x, tmp_points_square_y);

					four_float_compare  (distances_from_center, square_radius, place);
					int mask = four_flat_make_mask(place);
					if (!mask)
						break;

					four_float_add_with_int(counter, counter, place);
					four_float_copy (points_x, tmp_points_x); four_float_copy (points_y, tmp_points_y);

					common_counter++;
				}

				for (int k = 0; k < 4; k++)
					screen_set_pixel_color(mandelbrot_screen, current_line + j + k, (char)counter[k], 128, 255 - (char)counter[k], 255);
			}
		}
		printf ("\t\r%.0lf", txGetFPS());
       	txSleep();
	}

	screen_delete(mandelbrot_screen);
}
