#include <stdio.h>

typedef unsigned int Digit;
typedef long long ExtDigit;

enum BnSign { BN_POSITIVE, BN_NEGATIVE, BN_ZERO };
enum BnCodes { BN_OK, BN_NULL_OBJECT, BN_NO_MEMORY, BN_DIVIDE_BY_ZERO };
const ExtDigit BN_RADIX = 4294967296;

struct bn_s {
    Digit* body;
    Digit body_size;
    BnSign sign;
};
typedef struct bn_s bn;

size_t _strlen (const char* str) {
    size_t i = 0;
    for (; str[i] != '\0'; i++);
    return i;
}

bn* bn_new() {
    bn* r  = new bn;
    if (r == nullptr) return nullptr;

    r->body = new Digit[1];
    if (r->body == nullptr) {
        delete r;
        return nullptr;
    }

    r->body_size = 1;
    r->sign = BN_ZERO;
    r->body[0] = 0;
    
    return r;
}

int bn_delete (bn* t) {
    if (t == nullptr) return BN_NULL_OBJECT;
    delete[] t->body;
    delete t;
    return BN_OK;
}

int main() {
    bn* a = bn_new();
    int code = bn_delete(a);
    printf("Code is %d\n", code);
    return 0;
}
