#include <stdio.h>

int rot13(int c){
    if('a' <= c && c <= 'z'){
        return rot13b(c,'a');
    } else if ('A' <= c && c <= 'Z') {
        return rot13b(c, 'A');
    } else {
        return c;
    }
}

int rot13b(int c, int basis){
    c = (((c-basis)+13)%26)+basis;
    return c;
}

int main() {
// "I have really enjoyed eating salsa with Saurav."                                                                  
    char MESSAGE[] = "V unir ernyyl rawblrq rngvat fnyfn jvgu Fnheni.";
    int skips[] = { 1, 4, 6, 7, 6, 5, 4, 7 }
    int c, i, j, k;

    for ( i = 0 ; i < sizeof(MESSAGE) ; ++i ) {
        for ( j = 0 ; j < sizeof(skips) ; ++j ) {
            c = (int)MESSAGE[i];
            c = rot13(c);
            putchar(c);
        }
    }

    return 0;
}
