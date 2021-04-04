#include <emmintrin.h>
#include <smmintrin.h>
#include <immintrin.h>

const int NBZ                = -1;//"Nothing but zero" in shuffle
const int pixels_amount      = 8;
const int color_bytes_amount = 4;

const int color_maximum      = 256;

inline __m256i sse_load_chars_from_array(unsigned char *array);

inline __m128i sse_move_upper_in_two_positions(__m128i variable_upper, __m128i variable_lower);
inline __m128i sse_dilute_lowers_with_zeroes  (__m128i variable);

inline __m256i sse256_dilute_lowers(__m256i variable);
inline __m256i sse256_dilute_uppers(__m256i variable);
inline __m256i sse_dilute_every_upper_forth_bytes(__m128i variable);
inline __m256i sse_dilute_every_lower_forth_bytes(__m128i variable);
inline __m256i sse_inverse_bytes      (__m128i variable);
inline void    sse_return_into_array  (__m128i upper_values, __m128i lower_values, unsigned char *pointer);