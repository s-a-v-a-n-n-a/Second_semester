#include "sse_instructions.cpp"
#include "screen_functions.cpp"
#include "mixing_and_drawing.cpp"

int main()
{
	// screen_code report = draw_mixed_pictures("Table.bmp", "ccat.png", screen_width/2, screen_height/2);
	// printf("%s", screen_state_text[(int)report]);

	test_fps("Table.bmp", "ccat.png");
	picture_presentation("Table.bmp", "AskhatCat.bmp", "ccat.png", screen_width/2, screen_height/2);

    return 0;
}