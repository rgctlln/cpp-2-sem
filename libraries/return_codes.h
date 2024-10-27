#pragma once

#ifndef SUCCESS
// The operation completed successfully
#define SUCCESS 0
#endif

#ifndef ERROR_CANNOT_OPEN_FILE
// File can't be opened
#define ERROR_CANNOT_OPEN_FILE 1
#endif

#ifndef ERROR_NOTENOUGH_MEMORY
// Not enough memory, memory allocation failed
#define ERROR_NOTENOUGH_MEMORY 2
#endif

#ifndef ERROR_DATA_INVALID
// The data is invalid
#define ERROR_DATA_INVALID 3
#endif

#ifndef ERROR_ARGUMENTS_INVALID
// The cmd line's args or number of parameters (argv) is incorrect
#define ERROR_ARGUMENTS_INVALID 4
#endif

#ifndef ERROR_FORMAT_INVALID
// Incorrect file format
#define ERROR_FORMAT_INVALID 5
#endif

#ifndef ERROR_UNSUPPORTED
// Unsupported functionality
#define ERROR_UNSUPPORTED 20
#endif

#ifndef ERROR_UNKNOWN
// Other errors
#define ERROR_UNKNOWN 250
#endif
