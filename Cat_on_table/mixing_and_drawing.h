#include <ctime>

const size_t mixing_amount = 500;

const size_t screen_width  = 800;
const size_t screen_height = 600;


inline screen_code screen_mix(screen_information *where_to, screen_information *where_from, size_t x_coordinate, size_t y_coordinate);

void picture_presentation(const char *back_ground, const char *default_foreground, const char *additional_foreground, size_t x_coordinate, size_t y_coordinate);
void test_fps            (const char *file_name_to, const char *file_name_from);


void  screen_picture_output(sf::Sprite sprite, const size_t width, const size_t height);
screen_code draw_mixed_pictures(const char *file_name_to, const char *file_name_from, size_t x_coordinate, size_t y_coordinate);
