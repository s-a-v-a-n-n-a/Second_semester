#include <SFML/Graphics.hpp>

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
screen_code 		screen_construct(screen_information *screen, const char *file_name);
screen_code 		screen_delete(screen_information *screen);
screen_code 		screen_destruct(screen_information *screen);

screen_code         screen_load(sf::Image* image, screen_information *screen);