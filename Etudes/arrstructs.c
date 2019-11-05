#include<stdio.h>
 
/**
 * structure is defined above all functions so it is global.
 */
 
struct company
{
    char name[20];
    char ceo[20];
    float revenue; // in $
    float pps; // price per stock in $
};
 
void print_struct(const struct company str_arr[]);
 
int main()
{
 
    struct company companies[3] = {
                           {"Country Books", "Tim Green", 999999999, 1300 },
                           {"Country Cooks", "Jim Green", 9999999, 700 },
                           {"Country Hooks", "Sim Green", 99999, 300 },
                   };
    print_struct(companies);
 
    return 0;
}
 
void print_struct(const struct company str_arr[])
{
    int i;
 
    printf("sizeof(company): %lu\n", sizeof(*str_arr));
    printf("sizeof(i): %lu\n", sizeof(i));
/*
    for(i= 0; i<3; i++) {
        printf("%s", str_arr[i].name);
        printf("%s", str_arr[i].ceo);
        printf("%f", str_arr[i].revenue);
        printf("%f", str_arr[i].pps);
    }
*/
    for(i= 0; i<3; i++) {
        printf("Name: %s\n", str_arr[i].name);
        printf("CEO: %s\n", str_arr[i].ceo);
        printf("Revenue: %.2f\n", str_arr[i].revenue);
        printf("Price per stock : %.2f\n", str_arr[i].pps);
        printf("\n");
    }
/* */
}

