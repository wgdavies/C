// C program for writing
// struct to file
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// a struct to read and write
struct person
{
    int id;
    char fname[20];
    char lname[20];
};

int main ()
{
    FILE *outfile;
    size_t fout = 0;

    // open file for writing
    outfile = fopen ("person.dat", "w");
    if (outfile == NULL)
    {
        fprintf(stderr, "\nError opend file\n");
        exit (1);
    }

    struct person input1 = {1, "rohan", "sharma"};
    struct person input2 = {2, "mahendra", "dhoni"};

    // write struct to file
    fout = fwrite (&input1, sizeof(struct person), 1, outfile);
    fout += fwrite (&input2, sizeof(struct person), 1, outfile);

    if(fout == 2)
        printf("contents to file written successfully !\n");
    else
        printf("error writing file !\n");

    // close file
    fclose (outfile);

    return 0;
}
