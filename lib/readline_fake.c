#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "readline_fake.h"

static char buffer[2048];

/* Fake readline function. */
char* readline(char* prompt) {
    // Output our prompt.
    fputs(prompt, stdout);

    // Read a line of user input, max 2048.
    fgets(buffer, 2048, stdin);

    // Prepare string for copying buffer content.
    char* copy = malloc(strlen(buffer) + 1);

    // Copy buffer to new string.
    strcpy(copy, buffer);

    // Set last element of new string to string terminator.
    copy[strlen(copy) - 1] = '\0';

    return copy;
}

/* Fake add_history function */
void add_history(char* unused) {}

