
/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include <stdint.h>
#include "string_lite.h"
#include <stddef.h>
/* ================================================== */
/*            GLOBAL VARIABLE DEFINITIONS             */
/* ================================================== */


/* ================================================== */
/*            FUNCTION PROTOTYPES (DECLARATIONS)      */
/* ================================================== */


/* ================================================== */
/*                 FUNCTION DEFINITIONS               */
/* ================================================== */

// Returns the length of the string 'str'
int strlen_lite(const char* str) {
    int len  = 0;
    while(*str){
        len++;
        str++;
    }
    return len;
}

// Copies the string 'str' into 'dest'
void strcpy_lite(char* dest, const char* src) {
    while(*src){
        *dest++ = *src++; 
    }
    *dest = '\0'; //copy null over
}

// Copies at most 'n' characters from 'src' into 'dest'
void strncpy_lite(char* dest, const char* src, int n) {
    int ct = 0;

    // Copy characters until 'n' or end of src
    while (*src && ct < n) {
        *dest++ = *src++;
        ct++;
    }

    // Fill remaining space with nulls
    while (ct < n) {
        *dest++ = '\0';
        ct++;
    }
}

// Concatenates 'str' to the end of 'dest'
void strcat_lite(char* dest, const char* src) {
    // Find the end of dest
    while (*dest) {
        dest++;
    }

    // Copy src into dest
    while (*src) {
        *dest++ = *src++;
    }

    *dest = '\0'; // Null-terminate
}


// Concatenates at most 'n' characters of 'str' to 'dest'
void strncat_lite(char* dest, const char* src, int n) {
    // Find the end of dest
    while (*dest) {
        dest++;
    }

    // Append at most 'n' characters from src
    int ct = 0;
    while (*src && ct < n) {
        *dest++ = *src++;
        ct++;
    }

    *dest = '\0'; // Always null-terminate
}


// Compares two strings lexicographically
int strcmp_lite(const char* str1, const char* str2) {
    // Compare characters one by one
    while (*str1 && *str2) {
        if (*str1 < *str2) {
            return -1; // str1 is smaller
        } else if (*str1 > *str2) {
            return 1; // str1 is larger
        }
        str1++;
        str2++;
    }

    // At this point, one string might end before the other
    if (*str1) {  // str1 is longer
        return 1;
    } else if (*str2) {  // str2 is longer
        return -1;
    }

    return 0; // Strings are equal
}


// Compares at most 'n' characters of two strings
int strncmp_lite(const char* str1, const char* str2, int n) {
    int i = 0;

    // Compare characters until n is reached or a null terminator is found
    while (i < n && *str1 && *str2) {
        if (*str1 < *str2) {
            return -1; // str1 is smaller
        } else if (*str1 > *str2) {
            return 1; // str1 is larger
        }
        str1++;
        str2++;
        i++;
    }

    // If 'n' characters compared or both strings ended, return 0
    if (i == n) {
        return 0; // Considered equal within 'n' characters
    }

    // Handle cases where one string ends before the other
    if (*str1) return 1;  // str1 is longer
    if (*str2) return -1; // str2 is longer

    return 0; // Equal
}


static char *global_saveptr = NULL; // Global save pointer for strtok_lite

char *strtok_lite(char *str, const char *delim) {
    // If str is NULL, continue from the previous token
    if (str == NULL) {
        str = global_saveptr;
    }

    // If there's no saved pointer, return NULL
    if (str == NULL) {
        return NULL;
    }

    // Skip leading delimiters
    while (*str && strchr_lite(delim, *str)) {
        str++;
    }

    // If we reached the end of the string, reset and return NULL
    if (*str == '\0') {
        global_saveptr = NULL;
        return NULL;
    }

    // Mark the beginning of the token
    char *tokenStart = str;

    // Find the end of the token
    while (*str && !strchr_lite(delim, *str)) {
        str++;
    }

    // Null-terminate the token if we found a delimiter
    if (*str) {
        *str = '\0';
        str++;
    }

    // Save the next position globally
    global_saveptr = str;

    return tokenStart;
}

char *strchr_lite(const char *str, int c) {
    // Iterate through the string
    while (*str) {
        if (*str == (char)c) {
            return (char *)str; // Return pointer to the found character
        }
        str++;
    }

    // Check if the character is '\0'
    if (c == '\0') {
        return (char *)str; // Return pointer to the null terminator
    }

    return NULL; // Character not found
}

long atol_lite(const char* str) {
    long result = 0;
    int sign = 1;

    // Skip whitespace
    while (*str == ' ' || *str == '\t') {
        str++;
    }

    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }

    return result * sign;
}

char* strstr_lite(const char* haystack, const char* needle) {
    if (!*needle) return (char*)haystack;

    for (; *haystack; haystack++) {
        const char* h = haystack;
        const char* n = needle;

        while (*h && *n && *h == *n) {
            h++;
            n++;
        }

        if (!*n) {
            return (char*)haystack;
        }
    }

    return NULL;
}

void memset_lite(void *ptr, uint8_t value, uint32_t size){
    uint8_t *p = (uint8_t *)ptr;
    while (size--) {
        *p++ = value;
    }
}

void memcpy_lite(void *dest, const void *src, uint32_t size){
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    while (size--) {
        *d++ = *s++;
    }
}

void memmove_lite(void *dest, const void *src, uint32_t size) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;

    if (d == s || size == 0) {
        // Nothing to do
        return;
    }

    if (d < s) {
        // No overlap or safe to copy forward
        while (size--) {
            *d++ = *s++;
        }
    } else {
        // Overlapping and dest > src, copy backwards
        d += size;
        s += size;
        while (size--) {
            *--d = *--s;
        }
    }
}
