#include <stdio.h>

// Delcare a global array of 2048 - a buffer for user input.
// The 'static' keyword makes the variable local to this file.
static char input[2048];

int main(int argc, char** argv) {
    // Print version and exit information.
    puts("Lispy Version 0.0.0.0.1");
    puts("Press Ctrl+c to Exit\n");

    // Never ending loop.
    while (1) {
        // Output our prompt.
        // fputs is like puts, but it doesn't add a newline character, it
        // requires a file to write to, we use 'stdout' here.
        fputs("lispy> ", stdout);

        // Read a line of user input, max 2048. It requires a file to write
        // from, we use 'stdin' here.
        fgets(input, 2048, stdin);

        // Echo input back to user.
        printf("No, you're a %s", input);
    }

    return 0;
}

