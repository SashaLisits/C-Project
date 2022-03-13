
#include "assembler.h"

int getBits(word w, int from, int to) {
    word temp;
    temp.val = w.val;
    temp.val = temp.val << (19 - to); // number of bits to the left
    // TODO לבדוק שאכן מתבצעת הזזה אריתמטית - כלומר סיבית הסימן נשארת
    temp.val = temp.val >> (19 - (to - from)); // the number of bits to the right
    return (int) temp.val;
}

void setBits(word *w, int from, int to, int value) {
    int size = to - from;
    int mask = -1;// mask = 11...1
    mask = mask << to; // mask = 1...1 0...0 (number of '0' is to)
    
    int mask2 = 1;
    mask2 = mask2 << from; // mask2 = 0...0 1 0...0
    mask2 = mask2 - 1; // mask2 = 0...0 1...1
    
    mask = mask | mask2;     // mask = 1...1 0...0 1...1 (the zeros are at "from-to")
    w->val = w->val & mask; // zeros at "from-to" bits
    
    // make value only on 'size' bits:
    unsigned v = (unsigned) value;
    v = value << (19 - size);
    v = v >> (19 - to); // place the bits on "from-to"
    
    w->val = w->val | v; // at the number to the specific
}
