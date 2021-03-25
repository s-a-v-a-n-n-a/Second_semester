#include <TXLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const int horizontal_size = 800;
const int vertical_size = 800;
const int radius = 2;
const int maximum_iterations_amount = 255;

const float dx = 0.001;
const float epsilon = 0.0000001;

typedef struct Color
{
	char r;
	char g;
	char b;
	char a;
} s_color;

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
    //Win32::_fpreset();
    txBegin();

    //scr_t scr = (scr_t) *txVideoMemory();
    s_information *mandelbrot_screen = screen_new();

	long long height = mandelbrot_screen->vertical_screen_size;
	long long width = mandelbrot_screen->horizontal_screen_size;

	float center_x = mandelbrot_screen->center_x;
	float center_y = mandelbrot_screen->center_y;

	float left_corner_x = mandelbrot_screen->center_x - mandelbrot_screen->real_horizontal_size/2;
	float left_corner_y = mandelbrot_screen->center_y - mandelbrot_screen->real_vertical_size/2;

	for (int i = 0; i < height; i++)
	{
		long long current_line = i * width;
		for (int j = 0; j < width; j++)
		{
			float point_x_0 = left_corner_x + j * mandelbrot_screen->dx;
			float point_y_0 = left_corner_y + i * mandelbrot_screen->dy;

			float point_x = point_x_0;
			float point_y = point_y_0;

			int counter = 0;
			while (counter < maximum_iterations_amount)
			{
				float square_point_x = point_x * point_x;
				float square_point_y = point_y * point_y;

				float tmp_point_x = square_point_x - square_point_y + point_x_0;
				float tmp_point_y = 2 * point_x * point_y + point_y_0;

				float distance_from_center = tmp_point_x * tmp_point_x +
                                                  tmp_point_y * tmp_point_y;
				if (distance_from_center > radius * radius) //Артёму не понравилось, подумать
					break;

				counter++;

				point_x = tmp_point_x;
				point_y = tmp_point_y;
			}

			//printf("%lld\n", counter);
			screen_set_pixel_color(mandelbrot_screen, current_line + j, (char)counter, 128, 255 - (char)counter, 255);
		}
	}

	printf ("\t\r%.0lf", txGetFPS());
    txSleep();

	screen_delete(mandelbrot_screen);
}
