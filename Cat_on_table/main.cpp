#include <SFML/Graphics.hpp>

#include <emmintrin.h>
#include <smmintrin.h>
#include <immintrin.h>

#include <ctime>

const int color_maximum = 256;

const size_t mixint_amount = 500;

const size_t screen_width = 800;
const size_t screen_height = 600;


typedef enum screen_information_code_errors 
{ 
	SCREEN_OK,
    SCREEN_NULL,
    SCREEN_NO_CONSTRUCT,
    SCREEN_SEGMENT_FAULT
} screen_code;

const char *screen_state_text[]
{
    "EVERYTHING IS OKAY\n",
    "LIST DOES NOT EXIST\n",
    "NO MEMORY FOR CONSTRUCTION\n",
    "SEGMENTATION FAULT\n"
};


struct Color 
{
	unsigned char a;
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

struct screen_information 
{
	Color *data;
	size_t width;
	size_t height;
};

static inline void copy_color_from_sfml_color(sf::Color color, screen_information *screen, size_t line, size_t column);

screen_information *screen_new(const char *file_name);
screen_code screen_construct(screen_information *screen, const char *file_name);
screen_code screen_delete(screen_information *screen);
screen_code screen_destruct(screen_information *screen);

screen_code screen_load(sf::Image* image, screen_information *screen);
void screen_picture_output(sf::Sprite sprite, const size_t width, const size_t height);

screen_code draw_mixed_pictures(const char *file_name_to, const char *file_name_from, size_t x_coordinate, size_t y_coordinate);

inline __m128i sse_load_chars_from_array(unsigned char *array);
inline __m128i sse_move_upper_in_two_positions(__m128i variable_upper, __m128i variable_lower);
inline __m128i sse_dilute_lowers_with_zeroes(__m128i variable);
inline __m128i sse_dilute_every_upper_forth_bytes(__m128i variable);
inline __m128i sse_dilute_every_lower_forth_bytes(__m128i variable);
inline __m128i sse_inverse_bytes(__m128i variable);
inline __m128i sse_move_every_second_byte_in_one_variable(__m128i upper_values, __m128i lower_values);

int main()
{
	screen_code report = draw_mixed_pictures("Table.bmp", "AskhatCat.bmp", screen_width/2, screen_height/2);
	printf("%s", screen_state_text[(int)report]);

    return 0;
}

screen_information *screen_new(const char *file_name) 
{
	screen_information *screen = (screen_information*)calloc(1, sizeof(screen_information));
	if (!screen)
		return NULL;
	
	if (screen_construct(screen, file_name) != SCREEN_OK)
	{
		free(screen);
		return NULL;
	}

	return screen;
}

static inline void copy_color_from_sfml_color(sf::Color color, screen_information *screen, size_t line, size_t column)
{
	screen->data[line * screen->width + column].r = color.r;
	screen->data[line * screen->width + column].g = color.g;
	screen->data[line * screen->width + column].b = color.b;
	screen->data[line * screen->width + column].a = color.a;
}

screen_code screen_construct(screen_information *screen, const char *file_name)
{
	sf::Image image;
	image.loadFromFile(file_name);

	sf::Vector2u size = image.getSize();
	screen->width = size.x;
	screen->height = size.y;

	screen->data = (Color*)calloc(screen->width * screen->height, sizeof(Color));
	if (screen->data == NULL)
		return SCREEN_NO_CONSTRUCT;

	for (size_t i = 0; i < screen->height; i++)
		for (size_t j = 0; j + 3 < screen->width; j += 4)
		{
			for (int k = 0; k < 4; k++)
			{	
				sf::Color color = image.getPixel(j + k, i);
				copy_color_from_sfml_color(color, screen, i, j + k);
			}
		}

	return SCREEN_OK;
}

screen_code screen_delete(screen_information *screen)
{
	if (!screen)
		return SCREEN_NULL;

	if (screen_destruct(screen) == SCREEN_NULL)
		return SCREEN_NULL;
	free(screen);

	return SCREEN_OK;
}

screen_code screen_destruct(screen_information *screen)
{
	if (!screen->data)
		return SCREEN_NULL;

	free(screen->data);

	return SCREEN_OK;
}

screen_code screen_load(sf::Image *image, screen_information *screen)
{
	sf::Vector2u image_size = (*image).getSize();
	if (image_size.x < screen->width || image_size.y < screen->height)
		return SCREEN_SEGMENT_FAULT;

	for (size_t i = 0; i < screen->height; i++) {
		for (size_t j = 0; j + 3 < screen->width; j += 4)
		{
			for (int k = 0; k < 4; k++)
			{
				sf::Color color(screen->data[i * screen->width + j + k].r, screen->data[i * screen->width + j + k].g,
				                screen->data[i * screen->width + j + k].b, screen->data[i * screen->width + j + k].a);

				image->setPixel(j + k, i, color);
			}
		}
	}

	return SCREEN_OK;
}

inline __m128i sse_load_chars_from_array(unsigned char *array)
{
	return _mm_loadu_si128((__m128i*)array);
}
inline __m128i sse_move_upper_in_two_positions(__m128i variable_upper, __m128i variable_lower)
{
	__m128 tmp_upper = _mm_castsi128_ps(variable_upper);
	__m128 tmp_lower = _mm_castsi128_ps(variable_lower);

	return _mm_castps_si128(_mm_movehl_ps(tmp_upper, tmp_lower));
}

inline __m128i sse_dilute_lowers_with_zeroes(__m128i variable)
{
	return _mm_cvtepu8_epi16(variable);
}

inline __m128i sse_dilute_every_upper_forth_bytes(__m128i variable)
{
	__m128i mask = _mm_set_epi8(-1, 12, -1, 12, -1, 12, -1, 12, -1, 8, -1, 8, -1, 8, -1, 8);

	return _mm_shuffle_epi8(variable, mask);
}

inline __m128i sse_dilute_every_lower_forth_bytes(__m128i variable)
{
	__m128i mask = _mm_set_epi8(-1, 4, -1, 4, -1, 4, -1, 4, -1, 0, -1, 0, -1, 0, -1, 0);

	return _mm_shuffle_epi8(variable, mask);
}

inline __m128i sse_inverse_bytes(__m128i variable)
{
	return _mm_sub_epi16(_mm_set_epi16(256, 256, 256, 256, 256, 256, 256, 256), variable);
}

inline __m128i sse_move_every_second_byte_in_one_variable(__m128i upper_values, __m128i lower_values)
{
	__m128i returning_back_mask = _mm_set_epi8(14, 12, 10, 8, 6, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1, -1);

	__m128i upper_tmp = _mm_shuffle_epi8(upper_values, returning_back_mask);
	__m128i lower_tmp = _mm_shuffle_epi8(lower_values, returning_back_mask);

	return sse_move_upper_in_two_positions(upper_tmp, lower_tmp);
}

screen_code screen_mix(screen_information *where_to, screen_information *where_from, size_t x_coordinate, size_t y_coordinate)
{
	if (where_to->width < where_from->width || where_to->height < where_from->height || x_coordinate < 0 || y_coordinate < 0)
	{
		return SCREEN_SEGMENT_FAULT;
	}
	if (x_coordinate >= where_to->width)
		x_coordinate = where_to->width - x_coordinate;
	if (y_coordinate >= where_to->height)
		y_coordinate = where_to->height - y_coordinate;

	for (size_t i_front = 0, i_back = x_coordinate; i_front < where_from->height; i_front++, i_back++)
		for (size_t j_front = 0, j_back = y_coordinate; j_front + 3 < where_from->width; j_front += 4, j_back += 4)
		{
			__m128i four_front_pixels = sse_load_chars_from_array((unsigned char*)(where_from->data + i_front * where_from->width + j_front));
			__m128i four_back_pixels  = sse_load_chars_from_array((unsigned char*)(where_to->data + i_back * where_to->width + j_back));

			__m128i two_front_upper_pixels = sse_move_upper_in_two_positions(four_front_pixels, four_front_pixels);
			__m128i two_front_pixels_lower = sse_dilute_lowers_with_zeroes(four_front_pixels);
			__m128i two_front_pixels_upper = sse_dilute_lowers_with_zeroes(two_front_upper_pixels);

			__m128i two_back_upper_pixels = sse_move_upper_in_two_positions(four_back_pixels, four_back_pixels);
			__m128i two_back_pixels_lower = sse_dilute_lowers_with_zeroes(four_back_pixels);
			__m128i two_back_pixels_upper = sse_dilute_lowers_with_zeroes(two_back_upper_pixels);

			
			__m128i two_upper_transparency = sse_dilute_every_upper_forth_bytes(four_front_pixels);
			__m128i two_lower_transparency = sse_dilute_every_lower_forth_bytes(four_front_pixels);


			__m128i two_upper_transparency_inversed = sse_inverse_bytes(two_upper_transparency);
			__m128i two_lower_transparency_inversed = sse_inverse_bytes(two_lower_transparency);


			two_front_pixels_lower = _mm_mullo_epi16(two_lower_transparency, two_front_pixels_lower);
			two_front_pixels_upper = _mm_mullo_epi16(two_upper_transparency, two_front_pixels_upper);

			two_back_pixels_lower = _mm_mullo_epi16(two_lower_transparency_inversed, two_back_pixels_lower);
			two_back_pixels_upper = _mm_mullo_epi16(two_upper_transparency_inversed, two_back_pixels_upper);

			two_front_pixels_lower = _mm_add_epi16(two_front_pixels_lower, two_back_pixels_lower);
			two_front_pixels_upper = _mm_add_epi16(two_front_pixels_upper, two_back_pixels_upper);

			
			two_front_pixels_lower = _mm_srli_epi16(two_front_pixels_lower, 8);
			two_front_pixels_upper = _mm_srli_epi16(two_front_pixels_upper, 8);


			four_front_pixels = sse_move_every_second_byte_in_one_variable(two_front_pixels_upper, two_front_pixels_lower);
			_mm_storeu_si128((__m128i*)(where_to->data + i_back * where_to->width + j_back), four_front_pixels);
		}

	return SCREEN_OK;
}

void screen_picture_output(sf::Sprite sprite, const size_t width, const size_t height)
{
	sf::RenderWindow window(sf::VideoMode(width, height), "Catort");

	while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }
}

screen_code draw_mixed_pictures(const char *file_name_to, const char *file_name_from, size_t x_coordinate, size_t y_coordinate)
{
	screen_code report = SCREEN_OK;

	screen_information *current_screen = screen_new(file_name_to);
	screen_information *infliction_screen = screen_new(file_name_from);

	if (!current_screen || !infliction_screen)
		return SCREEN_NO_CONSTRUCT;

	if (x_coordinate > current_screen->width || y_coordinate > current_screen->height)
		return SCREEN_SEGMENT_FAULT;

	sf::Sprite sprite;
	sf::Texture texture;
	texture.create(current_screen->width, current_screen->height);

	sprite.setTexture(texture);
	
	unsigned start_time = clock();
	for (int i = 0; i < mixint_amount; i++)
	{
		report = screen_mix(current_screen, infliction_screen, x_coordinate, y_coordinate);
		if (report != SCREEN_OK)
			return SCREEN_SEGMENT_FAULT;
	}
	unsigned end_time = clock();
	printf("Time of mixing: %f sec\n", (end_time - start_time)/(float)mixint_amount);

	sf::Image image;
	
	image.create(current_screen->width, current_screen->height);
    report = screen_load(&image, current_screen);
    if (report != SCREEN_OK)
    	return SCREEN_SEGMENT_FAULT;
    texture.update(image);

    screen_picture_output(sprite, current_screen->width, current_screen->height);

    report = screen_delete(current_screen);
    if (report != SCREEN_OK)
    	return SCREEN_SEGMENT_FAULT;
    report = screen_delete(infliction_screen);
    if (report != SCREEN_OK)
    	return SCREEN_SEGMENT_FAULT;

    return report;
}
