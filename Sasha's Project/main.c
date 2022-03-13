#include "assembler.h"

void printBin(word x);

void printBin2(int x) {
    unsigned int mask = 1 << (sizeof(int) * 8 - 1); // המספר שהסיבית הכי שמאלית שלו היא 1 וכל השאר 0
    while (mask) {
        if ((x & mask) == 0)  // בודק האם הסיבית הנוכחית היא 0
            printf("0");
        else // הסיבית הנוכחית היא 1
            printf("1");
        mask >>= 1; // נקדם את mask אחד ימינה (כלומר את הסיבית היחידה שהיא 1)
    }
    printf("\n");
}

int getBits(word w, int from, int to);

void setBits(word *w, int from, int to, int value);

int main() {
    word x;
    x.val = 262144;
    x.val += 750;
    printBin(x);
    setBits(&x, 5, 14, -6);
    printBin(x);
    int y = x.val;
    y >> 8;
    printBin2(y);
}


void printBin(word x) {
    unsigned int mask = 1 << (20 - 1);
    while (mask) {
        if ((x.val & mask) == 0)
            printf("0");
        else
            printf("1");
        mask >>= 1;
    }
    printf("\n");
}
