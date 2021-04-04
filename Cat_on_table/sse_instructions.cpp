#include "sse_instructions.h"


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
	return _mm256_sub_epi16(_mm256_set1_epi16(color_maximum), variable);
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