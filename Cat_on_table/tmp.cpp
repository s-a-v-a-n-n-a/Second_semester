#include <SFML/Graphics.hpp>

#include <emmintrin.h>
#include <smmintrin.h>
#include <immintrin.h>

#include <ctime>

const int NBZ = -1; // Nothing but zero in shuffle
const int pixels_amount = 8;
const int color_bytes_amount = 4;

const int color_maximum = 256;

const size_t mixing_amount = 500;

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
    "SCREEN DOES NOT EXIST\n",
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

	sf::Sprite sprite;
	sf::Texture texture;
};

static inline void copy_color_from_sfml_color(sf::Color color, screen_information *screen, size_t line, size_t column);

screen_information *screen_new(const char *file_name);
screen_code 		screen_construct(screen_information *screen, const char *file_name);
screen_code 		screen_delete(screen_information *screen);
screen_code 		screen_destruct(screen_information *screen);

screen_code screen_load(sf::Image* image, screen_information *screen);
void screen_picture_output(const char *file_name_to, const char *file_name_from, size_t x_coordinate, size_t y_coordinate);

screen_information *screen_load_from_picture(const char *file_name);
screen_code draw_mixed_pictures(screen_information *screen_to, screen_information *screen_from, size_t x_coordinate, size_t y_coordinate);
inline __m256i sse_load_chars_from_array(unsigned char *array);

inline __m128i sse_move_upper_in_two_positions(__m128i variable_upper, __m128i variable_lower);
inline __m128i sse_dilute_lowers_with_zeroes(__m128i variable);

inline __m256i sse256_dilute_lowers(__m256i variable);
inline __m256i sse256_dilute_uppers(__m256i variable);
inline __m256i sse_dilute_every_upper_forth_bytes(__m128i variable);
inline __m256i sse_dilute_every_lower_forth_bytes(__m128i variable);
inline __m256i sse_inverse_bytes(__m128i variable);
inline void    sse_return_into_array(__m128i upper_values, __m128i lower_values, unsigned char *pointer);

void bonus_screen_picture_output(size_t x_coordinate, size_t y_coordinate);
sf::Sprite bonus_draw_mixed_pictures(const char *file_name_to, const char *file_name_from, size_t x_coordinate, size_t y_coordinate);

int main()
{
	screen_picture_output("Table.bmp", "ccat.png", screen_width/2, screen_height/2);
	//printf("%s", screen_state_text[(int)report]);

	//bonus_screen_picture_output(screen_width/2, screen_height/2);

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
		for (size_t j = 0; j < screen->width; j++)
		{
			sf::Color color = image.getPixel(j, i);
			copy_color_from_sfml_color(color, screen, i, j);
		}

	screen->sprite = sf::Sprite();
	screen->texture = sf::Texture();

	screen->texture.create(screen->width, screen->height);
	screen->sprite.setTexture(screen->texture);

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
	if (!image || !screen)
		return SCREEN_SEGMENT_FAULT;

	sf::Vector2u image_size = (*image).getSize();
	if (image_size.x < screen->width || image_size.y < screen->height)
		return SCREEN_SEGMENT_FAULT;

	for (size_t i = 0; i < screen->height; i++) {
		for (size_t j = 0; j < screen->width; j++)
		{
			sf::Color color(screen->data[i * screen->width + j].r, screen->data[i * screen->width + j].g,
				            screen->data[i * screen->width + j].b, screen->data[i * screen->width + j].a);

			image->setPixel(j, i, color);
		}
	}

	return SCREEN_OK;
}

screen_code screen_copy_data(screen_information *copy_to, screen_information *copy_from)
{
	if (!copy_to || !copy_from || !copy_to->data || !copy_from->data)
		return SCREEN_NULL;

	size_t size = copy_from->width * copy_from->height;
	for (int i = 0; i < size; i++)
	{
		copy_to->data[i].a = copy_from->data[i].a;
		copy_to->data[i].r = copy_from->data[i].r;
		copy_to->data[i].g = copy_from->data[i].g;
		copy_to->data[i].b = copy_from->data[i].b;
	}

	return SCREEN_OK;
}

screen_code screen_copy(screen_information *copy_to, screen_information *copy_from)
{
	if (!copy_to || !copy_from)
		return SCREEN_NULL;

	copy_to->width = copy_from->width;
	copy_to->height = copy_from->height;
	if (screen_copy_data(copy_to, copy_from) != SCREEN_OK)
		return SCREEN_NULL;

	return SCREEN_OK;
}


inline __m256i sse_load_chars_from_array(unsigned char *array)
{
	return _mm256_loadu_si256((__m256i*)array);
}

inline __m256i sse_dilute_every_upper_forth_bytes(__m256i variable)
{
	__m256i mask = _mm256_set_epi8(28, 28, 28, 28, 24, 24, 24, 24, 
								   20, 20, 20, 20, 16, 16, 16, 16,
								   NBZ, NBZ, NBZ, NBZ, NBZ, NBZ, NBZ, NBZ, 
								   NBZ, NBZ, NBZ, NBZ, NBZ, NBZ, NBZ, NBZ);
	__m256i result = _mm256_shuffle_epi8(variable, mask);
	
	return _mm256_cvtepu8_epi16(_mm256_extracti128_si256(result, 1));
}

inline __m256i sse256_dilute_lowers(__m256i variable)
{
	return _mm256_cvtepu8_epi16(_mm256_extracti128_si256(variable, 0));
}

inline __m256i sse256_dilute_uppers(__m256i variable)
{
	__m256i variable_copy = variable;
	
	return _mm256_cvtepu8_epi16(_mm256_extracti128_si256(variable_copy, 1));
}

inline __m256i sse_dilute_every_lower_forth_bytes(__m256i variable)
{
	__m256i mask = _mm256_set_epi8(NBZ, NBZ, NBZ, NBZ, NBZ, NBZ, NBZ, NBZ, 
								   NBZ, NBZ, NBZ, NBZ, NBZ, NBZ, NBZ, NBZ, 
								   12, 12, 12, 12, 8, 8, 8, 8, 
								   4, 4, 4, 4, 0, 0, 0, 0);
	__m256i result = _mm256_shuffle_epi8(variable, mask);
	
	return _mm256_cvtepu8_epi16(_mm256_extracti128_si256(result, 0));
}

inline __m256i sse_inverse_bytes(__m256i variable)
{
	return _mm256_sub_epi16(_mm256_set1_epi16(256), variable);
}

inline void sse_return_into_array(__m256i upper_values, __m256i lower_values, unsigned char *pointer)
{
	__m128i returning_back_mask = _mm_set_epi8(15, 13, 11, 9, 7, 5, 3, 1,
											   NBZ, NBZ, NBZ, NBZ, NBZ, NBZ, NBZ, NBZ);

	__m128i upper_first_half  = _mm256_extracti128_si256(upper_values, 1);
	__m128i upper_second_half = _mm256_extracti128_si256(upper_values, 0);
	upper_first_half  = _mm_shuffle_epi8(upper_first_half, returning_back_mask);
	upper_second_half = _mm_shuffle_epi8(upper_second_half, returning_back_mask);
	
	__m128i upper = _mm_castps_si128(_mm_movehl_ps(_mm_castsi128_ps(upper_first_half), _mm_castsi128_ps(upper_second_half)));

	__m128i lower_first_half  = _mm256_extracti128_si256(lower_values, 1);
	__m128i lower_second_half = _mm256_extracti128_si256(lower_values, 0);
	lower_first_half  = _mm_shuffle_epi8(lower_first_half, returning_back_mask);
	lower_second_half = _mm_shuffle_epi8(lower_second_half, returning_back_mask);
	
	__m128i lower = _mm_castps_si128(_mm_movehl_ps(_mm_castsi128_ps(lower_first_half), _mm_castsi128_ps(lower_second_half)));

	_mm_storeu_si128((__m128i*)pointer, lower);
	_mm_storeu_si128((__m128i*)(pointer + pixels_amount/2 * color_bytes_amount), upper);
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
		for (size_t j_front = 0, j_back = y_coordinate; j_front + pixels_amount - 1 < where_from->width; j_front += pixels_amount, j_back += pixels_amount)
		{
			__m256i four_front_pixels = sse_load_chars_from_array((unsigned char*)(where_from->data + i_front * where_from->width + j_front));
			__m256i four_back_pixels  = sse_load_chars_from_array((unsigned char*)(where_to->data + i_back * where_to->width + j_back));

			__m256i two_front_pixels_lower = sse256_dilute_lowers(four_front_pixels);
			__m256i two_front_pixels_upper = sse256_dilute_uppers(four_front_pixels);
			
			__m256i two_back_pixels_lower = sse256_dilute_lowers(four_back_pixels);
			__m256i two_back_pixels_upper = sse256_dilute_uppers(four_back_pixels);
			
			__m256i two_upper_transparency = sse_dilute_every_upper_forth_bytes(four_front_pixels);
			__m256i two_lower_transparency = sse_dilute_every_lower_forth_bytes(four_front_pixels);


			__m256i two_upper_transparency_inversed = sse_inverse_bytes(two_upper_transparency);
			__m256i two_lower_transparency_inversed = sse_inverse_bytes(two_lower_transparency);

			two_front_pixels_lower = _mm256_mullo_epi16(two_lower_transparency, two_front_pixels_lower);
			two_front_pixels_upper = _mm256_mullo_epi16(two_upper_transparency, two_front_pixels_upper);

			two_back_pixels_lower = _mm256_mullo_epi16(two_lower_transparency_inversed, two_back_pixels_lower);
			two_back_pixels_upper = _mm256_mullo_epi16(two_upper_transparency_inversed, two_back_pixels_upper);

			two_front_pixels_lower = _mm256_add_epi16(two_front_pixels_lower, two_back_pixels_lower);
			two_front_pixels_upper = _mm256_add_epi16(two_front_pixels_upper, two_back_pixels_upper);

			sse_return_into_array(two_front_pixels_upper, two_front_pixels_lower, 
				                 (unsigned char*)(where_to->data + i_back * where_to->width + j_back));
		}

	return SCREEN_OK;
}


//main func
void screen_picture_output(const char *file_name_to, const char *file_name_from, size_t x_coordinate, size_t y_coordinate)
{
	screen_information *main_screen = screen_load_from_picture(file_name_to);
	screen_information *up = screen_load_from_picture(file_name_from);

	draw_mixed_pictures(main_screen, up, x_coordinate, y_coordinate);

	sf::RenderWindow window(sf::VideoMode(main_screen->width, main_screen->height), "Cat");

	while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(main_screen->sprite);
        window.display();
    }
}

screen_information *screen_load_from_picture(const char *file_name)
{
	screen_information *current_screen = screen_new(file_name);

	return current_screen;
}

screen_code draw_mixed_pictures(screen_information *screen_to, screen_information *screen_from, size_t x_coordinate, size_t y_coordinate)
{
	screen_code report = SCREEN_OK;

	if (!screen_to || !screen_from)
		return SCREEN_NO_CONSTRUCT;

	if (x_coordinate > screen_to->width || y_coordinate > screen_to->height)
		return SCREEN_SEGMENT_FAULT;

	unsigned start_time = clock();
	for (int i = 0; i < mixing_amount; i++)
	{
		report = screen_mix(screen_to, screen_from, x_coordinate, y_coordinate);
		if (report != SCREEN_OK)
			return SCREEN_SEGMENT_FAULT;
	}
	unsigned end_time = clock();
	printf("Time of mixing: %f sec\n", (end_time - start_time)/(float)mixing_amount);

	sf::Image image;
	
	image.create(screen_to->width, screen_to->height);
    report = screen_load(&image, screen_to);
    if (report != SCREEN_OK)
    	return SCREEN_SEGMENT_FAULT;
    screen_to->texture.update(image);

    return report;
}

// void bonus_screen_picture_output(size_t x_coordinate, size_t y_coordinate)
// {
// 	sf::RenderWindow window(sf::VideoMode(screen_width, screen_height), "Cat");

// 	// create two pictures: of a standing cat and of a not standing one
// 	// save is_standing flag, =true on keypressed, =false on realease
// 	// draw the one that is corresponding to flag (only when key is pressed)

// 	bool is_standing = false;
// 	while (window.isOpen())
//     {
//         sf::Event event;
//         while (window.pollEvent(event))
//         {
//             if (event.type == sf::Event::Closed)
//                 window.close();
//         }

//         window.clear();
//         while (window.pollEvent(event))
//         {
//             if (event.type == sf::Event::KeyPressed)
//             {
//             	if (event.key.code == sf::Keyboard::Up)
//             	{
//             		sf::Sprite sprite = bonus_draw_mixed_pictures("Table.bmp", "ccat.png", x_coordinate, y_coordinate);
//             		window.draw(sprite);

//             		printf("AAA\n");
//             	}
//             	else
//             	{
//             		sf::Sprite sprite = bonus_draw_mixed_pictures("Table.bmp", "AskhatCat.bmp", x_coordinate, y_coordinate);
//             		window.draw(sprite);

//             		printf("AAA\n");
//             	}
//             }

//             if (event.type == sf::Event::KeyReleased) 
//             {
//             	if (event.key.code == sf::Keyboard::Up) 
//             	{
//             		sf::Sprite sprite = bonus_draw_mixed_pictures("Table.bmp", "AskhatCat.bmp", x_coordinate, y_coordinate);
//             		window.draw(sprite);
//             	}
//             }

//             sf::Sprite sprite = bonus_draw_mixed_pictures("Table.bmp", "AskhatCat.bmp", x_coordinate, y_coordinate);
//             window.draw(sprite);
//         }
//         window.display();
//     }
// }

// sf::Sprite bonus_draw_mixed_pictures(const char *file_name_to, const char *file_name_from, size_t x_coordinate, size_t y_coordinate)
// {
// 	screen_code report = SCREEN_OK;

// 	screen_information *current_screen    = screen_new(file_name_to);
// 	screen_information *infliction_screen = screen_new(file_name_from);

// 	sf::Sprite sprite;
// 	sf::Texture *tx = new sf::Texture;
// 	sf::Texture &texture = *tx;
// 	texture.create(current_screen->width, current_screen->height);

// 	sprite.setTexture(texture);

// 	if (x_coordinate > current_screen->width || y_coordinate > current_screen->height)
// 		return sprite;
	
// 	screen_mix(current_screen, infliction_screen, x_coordinate, y_coordinate);

// 	sf::Image image;
	
// 	image.create(current_screen->width, current_screen->height);
//     screen_load(&image, current_screen);
//     texture.update(image);

//     return sprite;
// }