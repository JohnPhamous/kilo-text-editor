#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>

// creates a struct to hold the terminal attributes
struct termios orig_attributes;

void die(const char* s) {
    // print error
    perror(s);
    exit(1);
}

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_attributes);
}

void enableRawMode() {

    // saves the old terminal attributes
    tcgetattr(STDIN_FILENO, &orig_attributes);

    // when the program exits, calls the function
    atexit(disableRawMode);

    // sets temp struct with old attributes
    struct termios raw = orig_attributes;

    // turns off the ECHO feature and canonical mode
    // canonical mode off allows us to read input byte-by-byte
    // compared to the standard line-by-line
    // ISIG disables ctrl+z and ctrl+d
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);

    // IXON disables ctrl+s and ctrl+q
    raw.c_lflag &= ~(IXON);

    // disables terminal's output processing
    raw.c_oflag &= ~(OPOST);
    
    // sets the min bytes for read() to return
    raw.c_cc[VMIN] = 0;
    // sets the max bytes for read() to return
    raw.c_cc[VTIME] = 1;

    // sets the terminal's attributes with our settings
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
    enableRawMode();
    
    while (1) {
        // null character by default for input
        char c = '\0';
        read(STDIN_FILENO, &c, 1);
        // iscntrl checks to see if c is a control character
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        }
        else {
            printf("%d ('%c')\r\n", c, c);
        }

        if (c == 'q') {
            break;
        }
    }
    return 0;
}
