#include <stdio.h>
#include <stdlib.h>

#ifdef _linux
#include <readline_fake.h>
#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

int main(int argc, char** argv) {
    // Print version and exit information.
    puts("Lispy Version 0.0.0.0.1");
    puts("Press Ctrl+c to Exit\n");

    // Never ending loop.
    while (1) {
        // Output our prompt to get input.
        // The function strips the trailing newline character 
        char* input = readline("lispy> ");

        // Add input to history.
        add_history(input);

        // Echo input back to user. Append newline stripped by readline().
        printf("No, you're a %s\n", input);

        // Imported from stdlib.h, free retrived input.
        free(input);
    }

    return 0;
}


