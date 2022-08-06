// This code is part of a problem set for cs50.harvard.edu
// Author: Abenezer Amanuel

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Create an alias for a single byte as represented by an 8-bit unsigned int
typedef uint8_t BYTE;

int BLOCK_SIZE = 512;

// Function prototypes
void out_name(char *c, int count);

int main(int argc, char *argv[])
{
    // Return as error if user input is erroneous
    if (argc != 2)
    {
        printf("Usage ./recover FILENAME.raw\n");
        return 1;
    }

    FILE *input = fopen(argv[1], "r");

    // Return as error if file format is not valid
    if (input == NULL)
    {
        printf("Could not open file\n");
        return 1;
    }

    // Attempt to allocate BLOCK_SIZE bytes for a buffer, return as error if not possible
    BYTE *buffer = malloc(sizeof(BLOCK_SIZE));
    if (buffer == NULL)
    {
        printf("Could not allocate enough memory\n");
        return 2;
    }

    // Attempt to allocate 8 bytes for the name of output file, return as error is not possible
    char *output_name = malloc(8 * sizeof(char));
    if (output_name == NULL)
    {
        printf("Could not allocate enough memory\n");
        return 2;
    }

    // Declare output file that begins to be filled when we land on a header
    int has_started = 0;
    int count = 0;
    out_name(output_name, count);
    FILE *output = fopen(output_name, "w");
    count++;

    // Check each BLOCK_SIZE byte chunk in the input file until we reach the end and instead read a 0 byte chunk
    while (fread(buffer, sizeof(BLOCK_SIZE), 1, input))
    {
        // If we land on a header and...
        if (*buffer == 255 && *(buffer + 1) == 216 && *(buffer + 2) == 255 &&
            (*(buffer + 3) >= 224 && *(buffer + 3) <= 239))
        {
            // ...we had not started reading yet, write current chunk onto output and declare the start of file reading
            if (!has_started)
            {
                fwrite(buffer, sizeof(BLOCK_SIZE), 1, output);
                has_started = 1;
            }
            // ...we had already started reading, create a new output file and write current chunk onto it
            else
            {
                out_name(output_name, count);
                output = fopen(output_name, "w");
                count++;
                fwrite(buffer, sizeof(BLOCK_SIZE), 1, output);
            }
        }
        // If current chunk is not a header...
        else
        {
            // ...write current chunk onto output only if we had already started reading
            if (has_started)
            {
                fwrite(buffer, sizeof(BLOCK_SIZE), 1, output);
            }
        }
    }

    // Close allocated memory sites
    free(buffer);
    free(output_name);

    // Close open files
    fclose(input);
    fclose(output);
}

// Helper function to modify output name to the form ###.jpg based on a count
void out_name(char *c, int count)
{
    // Create an array and fill it up with the digits of count
    int places[] = {0, 0, 0};
    for (int i = 2; i >= 0; i--)
    {
        places[i] = count % 10;
        count /= 10;
    }

    // Modify the output name based on the array
    sprintf(c, "%i%i%i.jpg", places[0], places[1], places[2]);
}