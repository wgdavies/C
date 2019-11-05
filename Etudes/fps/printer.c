#include "printer.h"

int main(int argc, char** argv) {
    char* charstring = "printed this";
    char* input = argv[1];
	int   cmd = atoi(input);
    print_ptr printer;

    if (argc != 2 ) {
        printf("enter 0 or 1 to select my_print or some_print\n");
            exit(1);
    } else {
        printf("argv = %d\n", cmd);
    }

    switch(cmd) {
        case 0:
            printer = my_print;
            break;
         case 1:
            printer = some_print;
            break;
         default:
            printer = &my_print;
            break;
    }

    printer(charstring);

    exit(0);
}

