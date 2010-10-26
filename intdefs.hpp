/*
WASTE - intdefs.hpp (File transfer implementation)
*/

#ifdef _WIN32

// not tested but should be OK
typedef unsigned int		uint32_t;
typedef unsigned long long	uint64_t;

#else

// for uint32_t, uint64_t ... (according to target)
#include <stdint.h>

#endif
