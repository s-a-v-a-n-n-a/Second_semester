#include <TXLib.h>
#include <emmintrin.h>

//---------------------------------------------------------
//cd Desktop\Компиляторы\DOS\DOSBox\Doc\Mandelbrot
//
//g++ -msse2 ManBrot.cpp
//---------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const float horizontal_size = 300;
const float vertical_size   = 300;
const int radius = 2;
const int maximum_iterations_amount = 255;

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
inline void screen_set_pixel_color(s_information *screen, long long pixel, char red, char green, char blue);
void screen_delete(s_information *screen);
inline void screen_update_information(s_information *screen);
inline void screen_put_color(s_information *screen, const int index, const int variable);

inline __m128 sse_copy_value(const __m128 value);

inline __m128 sse_count_square(const __m128 value);
inline __m128 sse_count_euclidean_distance(const __m128 x, const __m128 y);

inline __m128 sse_count_re_complex_square(const __m128 x, const __m128 y);
inline __m128 sse_count_im_complex_square(const __m128 x, const __m128 y);

inline __m128i sse_add_floats_and_ints(const __m128 floats, const __m128i ints);

inline __m128 sse_x_coordinate_from_left_corner(const float left_corner_x, const float offset_x, const float dx);
inline __m128 sse_y_coordinate_from_left_corner(const float left_corner_y, const float offset_y, const float dy);

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

inline void screen_set_pixel_color(s_information *screen, long long pixel, char red, char green, char blue)
{
	screen->screen[pixel] = {(BYTE) red, (BYTE) green, (BYTE) blue};
}

void screen_delete(s_information *screen)
{
	free(screen);
}

inline void screen_update_information(s_information *screen)
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

inline void screen_put_color(s_information *screen, const int index, const int variable)
{
	int red = 255 * (variable < 255);
	int blue = 64 * (variable < 64);
	screen_set_pixel_color(screen, index, red, 255 - variable, blue);
}

inline __m128 sse_copy_value(const __m128 value)
{
	return _mm_move_ss(value, value);
}

inline __m128 sse_count_square(const __m128 value)
{
	return _mm_mul_ps(value, value);
}

inline __m128 sse_count_euclidean_distance(const __m128 x, const __m128 y)
{
	return _mm_add_ps(_mm_mul_ps(x, x), _mm_mul_ps(y, y));
}

inline __m128 sse_count_re_complex_square(const __m128 x, const __m128 y)
{
	__m128 square_x = sse_count_square(x);
	__m128 square_y = sse_count_square(y);

	return _mm_sub_ps(square_x, square_y);
}

inline __m128 sse_count_im_complex_square(const __m128 x, const __m128 y)
{
	__m128 xy = _mm_mul_ps(x, y);

	return _mm_add_ps(xy, xy);
}

inline __m128i sse_add_floats_and_ints(const __m128 floats, const __m128i ints)
{
	__m128i floats_in_ints = _mm_castps_si128(floats);

	return _mm_sub_epi32(ints, floats_in_ints);
}

//left_corner_x + j * dx
inline __m128 sse_x_coordinate_from_left_corner(const float left_corner_x, const float offset_x, const float dx)
{
	__m128 zero_one_two_three = _mm_set_ps(3.f, 2.f, 1.f, 0.f);
	__m128 y_coordinate_tmp_value = _mm_add_ps(_mm_set_ps1((float)offset_x), zero_one_two_three);

	return _mm_add_ps(_mm_mul_ps(_mm_set_ps1(dx), y_coordinate_tmp_value), _mm_set_ps1(left_corner_x));
}

inline __m128 sse_y_coordinate_from_left_corner(const float left_corner_y, const float offset_y, const float dy)
{
	return _mm_set_ps1(left_corner_y + (float)offset_y * dy);
}

void draw_mandelbrot()
{
	txCreateWindow (horizontal_size, vertical_size);
    Win32::_fpreset();
    txBegin();

    s_information *mandelbrot_screen = screen_new();

	int height = (int)(mandelbrot_screen->vertical_screen_size);
	int width  = (int)(mandelbrot_screen->horizontal_screen_size);

	__m128 square_radius = _mm_set_ps1(radius);
	square_radius = sse_count_square(square_radius);

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
				__m128 points_x_0 = sse_x_coordinate_from_left_corner(left_corner_x, j, mandelbrot_screen->dx);
				__m128 points_y_0 = sse_y_coordinate_from_left_corner(left_corner_y, i, mandelbrot_screen->dy);

				__m128 points_x = sse_copy_value(points_x_0);
	 			__m128 points_y = sse_copy_value(points_y_0);

				__m128 place    = _mm_set_ps1(1.f);
				__m128i counter = _mm_setzero_si128();
				int common_counter = 0;

				while (true)
				{
					__m128 tmp_points_x = _mm_add_ps(sse_count_re_complex_square(points_x, points_y), points_x_0);
					__m128 tmp_points_y = _mm_add_ps(sse_count_im_complex_square(points_x, points_y), points_y_0);
					
					__m128 distances_from_center = sse_count_euclidean_distance(tmp_points_x, tmp_points_y);
					
					place = _mm_cmple_ps(distances_from_center, square_radius);
					
					int mask = _mm_movemask_ps(place);
					if (!mask || !(common_counter < maximum_iterations_amount))
						break;

					counter = sse_add_floats_and_ints(place, counter);
					
					points_x = sse_copy_value(tmp_points_x);
					points_y = sse_copy_value(tmp_points_y);

					common_counter++;
				}
				
				int *color_values = (int*) &counter;
				for (int k = 0; k < 4; k++)
				{
					screen_put_color(mandelbrot_screen, current_line + j + k, color_values[k]);
				}
			}
		}
		printf ("\t\r%.0lf", txGetFPS());
       	txSleep();
	}

	screen_delete(mandelbrot_screen);
}
