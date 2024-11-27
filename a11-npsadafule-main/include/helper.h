#pragma once

#include <stdint.h>
#include <unistd.h>

// Convert buffer of bytes to a hex string representation
// Return 0 on success and -1 on failure
int convert(uint8_t *buff,     // input buffer containing bytes
            ssize_t buff_size, // size of input buffer
            char *str,         // output string buffer(hex represenatation)
            ssize_t str_size   // Size of output string buffer
);

// Message types
#define MSG_TYPE_CHAT 0
#define MSG_TYPE_TERMINATE 1
