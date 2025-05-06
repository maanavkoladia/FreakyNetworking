
/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include "stdio_lite.h"
#include <stdarg.h>
/* ================================================== */
/*            GLOBAL VARIABLE DEFINITIONS             */
/* ================================================== */
/* ================================================== */
/*            FUNCTION PROTOTYPES (DECLARATIONS)      */
/* ================================================== */


extern void _putchar(char character);
/* ================================================== */
/*                 FUNCTION DEFINITIONS               */
/* ================================================== */
// Outputs formatted text
void printf_lite(const char* format, ...) {
    va_list args;
    va_start(args, format);

    while (*format) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 'c': { // Character
                    char c = va_arg(args, int);
                    _putchar(c);
                    break;
                }
                case 's': { // String
                    const char* s = va_arg(args, const char*);
                    while (*s) {
                        _putchar(*s++);
                    }
                    break;
                }
                case 'd': { // Signed integer
                    int num = va_arg(args, int);
                    char buf[20];
                    itoa_lite(num, buf); // Assumes itoa_lite handles signed numbers
                    char* buftemp = buf;
                    while (*buftemp) {
                        _putchar(*buftemp++);
                    }
                    break;
                }
                case 'u': { // Unsigned integer
                    unsigned int num = va_arg(args, unsigned int);
                    char buf[20];
                    itoa_lite(num, buf); // You should have a utoa_lite() for unsigned
                    char* buftemp = buf;
                    while (*buftemp) {
                        _putchar(*buftemp++);
                    }
                    break;
                }
                case 'x': { // Hexadecimal (lowercase)
                    unsigned int num = va_arg(args, unsigned int);
                    char hexbuf[20];
                    itoa_hex_lite(num, hexbuf, 0); // 0 = lowercase
                    char* buftemp = hexbuf;
                    while (*buftemp) {
                        _putchar(*buftemp++);
                    }
                    break;
                }
                case 'X': { // Hexadecimal (uppercase)
                    unsigned int num = va_arg(args, unsigned int);
                    char hexbuf[20];
                    itoa_hex_lite(num, hexbuf, 1); // 1 = uppercase
                    char* buftemp = hexbuf;
                    while (*buftemp) {
                        _putchar(*buftemp++);
                    }
                    break;
                }
                case '%': { // Literal '%'
                    _putchar('%');
                    break;
                }
                default: { // Unknown specifier
                    _putchar('%');
                    if (*format) _putchar(*format);
                    break;
                }
            }
        } else {
            _putchar(*format);
        }
        format++;
    }

    va_end(args);
}
// Formats text into a string buffer
void sprintf_lite(char* buffer, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int buf_idx = 0; // Index for buffer

    while (*format) {
        if (*format == '%') {  // Handle format specifiers
            format++;          // Move past '%'
            switch (*format) {
                case 'c': {    // Character
                    char c = va_arg(args, int);
                    buffer[buf_idx++] = c;
                    break;
                }
                case 's': {    // String
                    const char* s = va_arg(args, const char*);
                    while(*s) {
                        buffer[buf_idx++] = *s++;
                    }
                    break;
                }
                case 'd': {    // Decimal (integer)
                    int num = va_arg(args, int);
                    char buf[20]; // Buffer for number string
                    itoa_lite(num, buf); // Convert number to string
                    char *p = buf;
                    while(*p) {
                        buffer[buf_idx++] = *p++;
                    }
                    break;
                }
                case '%': {    // Literal '%'
                    buffer[buf_idx++] = '%';
                    break;
                }
                default: {     // Unknown specifier
                    buffer[buf_idx++] = '%';
                    buffer[buf_idx++] = *format;
                    break;
                }
            }
        } else {               // Regular characters
            buffer[buf_idx++] = *format;
        }
        format++;              // Move to next character
    }

    buffer[buf_idx] = '\0';    // Null-terminate the buffer
    va_end(args);
}

void snprintf_lite(char* buffer, int buffer_size, const char* format, ...) {
    va_list args;
    va_start(args, format);

    int buf_idx = 0; // Index for buffer

    // Ensure buffer_size is at least 1 to allow null-termination
    if (buffer_size <= 0) {
        va_end(args);
        return;
    }

    while (*format && buf_idx < buffer_size - 1) { // Leave space for '\0'
        if (*format == '%') {  // Handle format specifiers
            format++;          // Move past '%'
            switch (*format) {
                case 'c': {    // Character
                    char c = va_arg(args, int);
                    if (buf_idx < buffer_size - 1) { // Check space
                        buffer[buf_idx++] = c;
                    }
                    break;
                }
                case 's': {    // String
                    const char* s = va_arg(args, const char*);
                    while (*s && buf_idx < buffer_size - 1) { // Check space
                        buffer[buf_idx++] = *s++;
                    }
                    break;
                }
                case 'd': {    // Decimal (integer)
                    int num = va_arg(args, int);
                    char buf[20]; // Buffer for number string
                    itoa_lite(num, buf); // Convert number to string
                    char *p = buf;
                    while (*p && buf_idx < buffer_size - 1) { // Check space
                        buffer[buf_idx++] = *p++;
                    }
                    break;
                }
                case '%': {    // Literal '%'
                    if (buf_idx < buffer_size - 1) {
                        buffer[buf_idx++] = '%';
                    }
                    break;
                }
                default: {     // Unknown specifier
                    if (buf_idx < buffer_size - 1) {
                        buffer[buf_idx++] = '%';
                    }
                    if (buf_idx < buffer_size - 1) {
                        buffer[buf_idx++] = *format;
                    }
                    break;
                }
            }
        } else {               // Regular characters
            if (buf_idx < buffer_size - 1) { // Check space
                buffer[buf_idx++] = *format;
            }
        }
        format++;              // Move to next character
    }

    // Null-terminate the string
    buffer[buf_idx] = '\0';
    va_end(args);
}


void itoa_lite(int32_t num, char* buffer) {
    int i = 0;                // Index for buffer
    int isNegative = 0;       // Flag for negative numbers

    // Handle zero case
    if (num == 0) {
        buffer[i++] = '0';    // Store '0'
        buffer[i] = '\0';     // Null-terminate the string
        return;
    }

    // Handle negative numbers
    if (num < 0) {
        isNegative = 1;
        // Handle INT_MIN (-2147483648) directly
        if (num == -2147483648) {
            num = 2147483647; // Process digits, adjust last digit later
        } else {
            num = -num;
        }
    }

    // Process digits (extract in reverse order)
    while (num != 0 && i < 10) { // Leave space for '-' and '\0'
        int rem = num % 10;
        buffer[i++] = rem + '0'; // Convert digit to character
        num = num / 10;
    }

    // Adjust for INT_MIN overflow (set last digit as '8')
    if (isNegative && num == 2147483647) {
        buffer[0] = '8'; // Replace the first digit
    }

    // Add '-' for negative numbers
    if (isNegative && i < 11) {
        buffer[i++] = '-';
    }

    // Null-terminate the string
    buffer[i] = '\0';

    // Reverse the string (excluding '\0')
    int start = 0;
    int end = i - 1; // Exclude '\0' while reversing
    while (start < end) {
        char temp = buffer[start];
        buffer[start++] = buffer[end];
        buffer[end--] = temp;
    }
}

void itoa_hex_lite(unsigned int value, char *buf, int uppercase) {
    const char *digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
    char temp[20];
    int i = 0;

    if (value == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }

    while (value > 0) {
        temp[i++] = digits[value % 16];
        value /= 16;
    }

    // Reverse into buf
    for (int j = 0; j < i; j++) {
        buf[j] = temp[i - j - 1];
    }
    buf[i] = '\0';
}

