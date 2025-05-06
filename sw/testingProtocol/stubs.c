#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "UDP.h"
#include "mac.h"
#include <string.h>
#include "printf.h"
#include "stubs.h"
#include "Networking_Globs.h"
#include "DHCP.h"

extern char *tx_outfile;
extern char *rx_infile;

void ethernetTX(uint8_t* payload, uint16_t size){
    for (int i = 0; i < size; i++) {
        if (i % 16 == 0) printf("%04x  ", i);
        printf("%02x ", payload[i]);
        if ((i + 1) % 16 == 0 || i + 1 == size) printf("\n");
    }
    printf("\n\n");

    FILE* fptr = fopen(tx_outfile, "w");
    for (int i = 0; i < size; i++) {
        if (i % 16 == 0) fprintf(fptr, "%06x  ", i);
        fprintf(fptr, "%02x ", payload[i]);
        if ((i + 1) % 16 == 0 || i + 1 == size) fprintf(fptr, "\n");
    }
    fclose(fptr);
}

uint8_t rx_buffer[MTU+50];
void ethernetRX() {
    FILE *fptr = fopen(rx_infile, "r");
    if (fptr == NULL) {
        printf("Error opening file\n");
        return;
    }

    char line[128];  // Buffer to hold each line (adjust size if needed)
    int i = 0;

    // Read each line
    while (fgets(line, sizeof(line), fptr) != NULL) {
        // Skip the address part (e.g., '0000', '0010', etc.)
        char *hex_part = strchr(line, ' ');
        if (hex_part == NULL) continue;  // No hex data, continue to next line

        hex_part++;  // Move past the space to start reading hex values

        // Tokenize the line by spaces
        char *token = strtok(hex_part, " \n");
        while (token != NULL) {
            // Convert each token (hex string) to a byte
            uint8_t byte = (uint8_t) strtol(token, NULL, 16);
            rx_buffer[i++] = byte;

            token = strtok(NULL, " \n");  // Get the next token
        }
    }

    fclose(fptr);
    macRX(rx_buffer, i);  // Pass the byte buffer to MAC layer (adjust the second argument if needed)
}

uint8_t dhcp_rx_buf[MTU];
void userRXData(uint8_t* payload, uint16_t size) {
    printf("========== Received Payload (%u bytes) ==========\n", size);
    for (uint16_t i = 0; i < size; i++) {
        if (i % 8 == 0) {
            printf("%04X: ", i);  // offset label
        }

        printf("%02X ", payload[i]);

        if ((i + 1) % 8 == 0 || i == size - 1) {
            printf("\n");
        }
    }
    printf("=================================================\n");
}
