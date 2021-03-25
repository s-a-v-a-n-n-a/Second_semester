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


void draw_mandelbrot()
{
	txCreateWindow (horizontal_size, vertical_size);
    Win32::_fpreset();
    txBegin();

    s_information *mandelbrot_screen = screen_new();

	long long height = mandelbrot_screen->vertical_screen_size;
	long long width = mandelbrot_screen->horizontal_screen_size;

	float center_x = mandelbrot_screen->center_x;
	float center_y = mandelbrot_screen->center_y;

	float left_corner_x = mandelbrot_screen->center_x - mandelbrot_screen->real_horizontal_size/2;
	float left_corner_y = mandelbrot_screen->center_y - mandelbrot_screen->real_vertical_size/2;

	float square_radius = radius * radius;

	while(true)
	{
		if (GetAsyncKeyState (VK_ESCAPE)) break;

		if (txGetAsyncKeyState (VK_RIGHT)) mandelbrot_screen->center_x    += mandelbrot_screen->dx * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
        if (txGetAsyncKeyState (VK_LEFT))  mandelbrot_screen->center_x    -= mandelbrot_screen->dx * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
        if (txGetAsyncKeyState (VK_DOWN))  mandelbrot_screen->center_y    -= mandelbrot_screen->dy * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
        if (txGetAsyncKeyState (VK_UP))    mandelbrot_screen->center_y    += mandelbrot_screen->dy * (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
        if (txGetAsyncKeyState ('A'))      {mandelbrot_screen->real_horizontal_size /= (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
        									mandelbrot_screen->real_vertical_size   /= (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
        								}
        if (txGetAsyncKeyState ('Z'))      {mandelbrot_screen->real_horizontal_size *= (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
        									mandelbrot_screen->real_vertical_size   *= (txGetAsyncKeyState (VK_SHIFT)? 100.f : 10.f);
        								}

        float center_x = mandelbrot_screen->center_x;
		float center_y = mandelbrot_screen->center_y;

		float left_corner_x = mandelbrot_screen->center_x - mandelbrot_screen->real_horizontal_size/2;
		float left_corner_y = mandelbrot_screen->center_y - mandelbrot_screen->real_vertical_size/2;

        mandelbrot_screen->dx = (mandelbrot_screen->real_horizontal_size/mandelbrot_screen->horizontal_screen_size);
		mandelbrot_screen->dy = (mandelbrot_screen->real_vertical_size/mandelbrot_screen->vertical_screen_size);

		for (int i = 0; i < height; i++)
		{
			if (GetAsyncKeyState (VK_ESCAPE)) break;

			long long current_line = i * width;
			for (int j = 0; j + 3 < width; j += 4)
			{
				float points_x_0[4] = {left_corner_x + j * mandelbrot_screen->dx,
										left_corner_x + (j + 1) * mandelbrot_screen->dx,
										left_corner_x + (j + 2) * mandelbrot_screen->dx,
										left_corner_x + (j + 3) * mandelbrot_screen->dx};

				float points_y_0[4] = {left_corner_y + i * mandelbrot_screen->dy,
										left_corner_y + i * mandelbrot_screen->dy,
										left_corner_y + i * mandelbrot_screen->dy,
										left_corner_y + i * mandelbrot_screen->dy};

				float points_x[4] = {points_x_0[0], points_x_0[1], points_x_0[2], points_x_0[3]};
				float points_y[4] = {points_y_0[0], points_y_0[1], points_y_0[2], points_y_0[3]};

				unsigned char place[4] = {1, 1, 1, 1};
				unsigned char mask_of_place = 0xf;

				int counter[4] = {0, 0, 0, 0};
				int common_counter = 0;

				while (mask_of_place && common_counter < maximum_iterations_amount)
				{
					float square_points_x[4] = {};
					for (int k = 0; k < 4; k++) square_points_x[k] = points_x[k] * points_x[k];
				 	float square_points_y[4] = {};
				 	for (int k = 0; k < 4; k++) square_points_y[k] = points_y[k] * points_y[k];

					float tmp_points_x[4] = {};
					for (int k = 0; k < 4; k++) tmp_points_x[k] = square_points_x[k] - square_points_y[k] + points_x_0[k];
					float tmp_points_y[4] = {};
					for (int k = 0; k < 4; k++) tmp_points_y[k] = 2 * points_x[k] * points_y[k] + points_y_0[k];

					float distances_from_center[4] = {};
					for (int k = 0; k < 4; k++) distances_from_center[k] = tmp_points_x[k] * tmp_points_x[k] + tmp_points_y[k] * tmp_points_y[k];

					for (int k = 0; k < 4; k++)
					{
						if (distances_from_center[k] > square_radius)
							place[k] = 0;
						if(place[k])
							counter[k]++;

						points_x[k] = tmp_points_x[k];
						points_y[k] = tmp_points_y[k];
					}

					mask_of_place = (((place[3] * 2) + place[2]) * 2 + place[1]) * 2 + place[0];
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
