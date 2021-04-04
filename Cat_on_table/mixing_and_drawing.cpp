#include "mixing_and_drawing.h"


inline screen_code screen_mix(screen_information *where_to, screen_information *where_from, size_t x_coordinate, size_t y_coordinate)
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

void picture_presentation(const char *back_ground, const char *default_foreground, const char *additional_foreground, size_t x_coordinate, size_t y_coordinate)
{
	sf::Sprite sprite;
	sf::Texture texture;
	texture.create(screen_width, screen_height);

	sprite.setTexture(texture);

	screen_information *current_screen_1 = screen_new(back_ground);
	screen_information *infliction_screen_1 = screen_new(default_foreground);

	screen_information *current_screen_2 = screen_new(back_ground);
	screen_information *infliction_screen_2 = screen_new(additional_foreground);

	sf::Image image_1;
	sf::Image image_2;

	screen_mix(current_screen_1, infliction_screen_1, x_coordinate - infliction_screen_1->width, y_coordinate);
	image_1.create(current_screen_1->width, current_screen_1->height);
    screen_load(&image_1, current_screen_1);
	
	screen_mix(current_screen_2, infliction_screen_2, x_coordinate - infliction_screen_2->width, y_coordinate);
	image_2.create(current_screen_2->width, current_screen_2->height);
    screen_load(&image_2, current_screen_2);

	sf::RenderWindow window(sf::VideoMode(screen_width, screen_height), "Cat");

	texture.update(image_1);

	while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::KeyPressed)
            {
            	if (event.key.code == sf::Keyboard::Up)
            	{
            		texture.update(image_2);
            	}
            	else
            	{
            		texture.update(image_1);
            	}
            }
        }
        window.draw(sprite);
        window.display();
    }

    screen_delete(current_screen_1);
    screen_delete(infliction_screen_1);
    screen_delete(current_screen_2);
    screen_delete(infliction_screen_2);
}

void test_fps(const char *file_name_to, const char *file_name_from)
{
	screen_information *current_screen    = screen_new(file_name_to);
	screen_information *infliction_screen = screen_new(file_name_from);

	unsigned start_time = clock();
	for (int i = 0; i < mixing_amount; i++)
	{
		screen_mix(current_screen, infliction_screen, 0, 0);
	}
	unsigned end_time = clock();
	printf("Time of mixing: %f sec\n", (end_time - start_time)/(float)mixing_amount);
}

void screen_picture_output(sf::Sprite sprite, const size_t width, const size_t height)
{
	sf::RenderWindow window(sf::VideoMode(width, height), "Cat");

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
	for (int i = 0; i < mixing_amount; i++)
	{
		report = screen_mix(current_screen, infliction_screen, x_coordinate, y_coordinate);
		if (report != SCREEN_OK)
			return SCREEN_SEGMENT_FAULT;
	}
	unsigned end_time = clock();
	printf("Time of mixing: %f sec\n", (end_time - start_time)/(float)mixing_amount);

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