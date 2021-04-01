#include <SFML/Graphics.hpp>
#include <assert.h>
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

int main()
{
	screen_code report = draw_mixed_pictures("Table.bmp", "AskhatCat.bmp", 0, 0);
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

screen_code screen_mix(screen_information *where_to, screen_information *where_from, size_t x_coordinate, size_t y_coordinate)
{
	if (where_to->width < where_from->width || where_to->height < where_from->height)
		return SCREEN_SEGMENT_FAULT;

	for (size_t i = x_coordinate; i < where_from->height; i++)
		for (size_t j = y_coordinate; j < where_from->width; j++)
		{
			Color pixel = {0, 0, 0, 0};

			Color front_pixel = where_from->data[i * where_from->width + j];
			Color back_pixel = where_to->data[i * where_to->width + j];

			int transparency = (int)front_pixel.a;

			pixel.r = ((int)front_pixel.r * transparency + (int)back_pixel.r * ((int)color_maximum - transparency)) >> (int)8;
			pixel.g = ((int)front_pixel.g * transparency + (int)back_pixel.g * ((int)color_maximum - transparency)) >> (int)8;
			pixel.b = ((int)front_pixel.b * transparency + (int)back_pixel.b * ((int)color_maximum - transparency)) >> (int)8;

			pixel.a = color_maximum - 1;

			where_to->data[i * where_to->width + j] = pixel;
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
