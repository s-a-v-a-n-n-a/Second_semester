#include "screen_functions.h"

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
		for (size_t j = 0; j < screen->width; j++)
		{
			sf::Color color(screen->data[i * screen->width + j].r, screen->data[i * screen->width + j].g,
				            screen->data[i * screen->width + j].b, screen->data[i * screen->width + j].a);

			image->setPixel(j, i, color);
		}
	}

	return SCREEN_OK;
}