#include <stdio.h>
#include <string.h>
// #include <strings.h>

int main (void) {
	char* restrict name = "hello";
	char* restrict nombre = ".world";
	char* nn = strncat(name, nombre, 6);

	printf("name: %s\nnombre: %s\nnn: %s\n", name, nombre, nn);

	return 0;
}
