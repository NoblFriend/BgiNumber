#include <stdio.h>

typedef unsigned long Digit;
typedef long long ExtDigit;

enum BnSign { BN_POSITIVE, BN_NEGATIVE, BN_ZERO };
enum BnCodes { BN_OK, BN_NULL_OBJECT, BN_NO_MEMORY, BN_DIVIDE_BY_ZERO };
const ExtDigit BN_RADIX = 4294967296;

struct bn_s {
    Digit* body;
    size_t size;
    BnSign sign;
};
typedef struct bn_s bn;

size_t _strlen (const char* str) {
    size_t i = 0;
    for (; str[i] != '\0'; i++);
    return i;
}

bn* _bn_realloc (bn* src, size_t new_size) {
    if (new_size <= src->size) return src;

    Digit* new_body = new Digit[new_size];
    for (size_t i = 0; i < src->size; i++) {
        new_body[i] = src->body[i];
    }
    delete [] src->body;
    src->body = new_body;
    src->size = new_size;
    return src;
}

bn* bn_new() {
    bn* r  = new bn;
    if (r == nullptr) return nullptr;

    r->body = new Digit[1];
    if (r->body == nullptr) {
        delete r;
        return nullptr;
    }

    r->size = 1;
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

int _bn_print(bn* t) {
    if (t->body == nullptr) return BN_NULL_OBJECT;

    printf("Size: %zu\n", t->size);
    printf("Body: ");

    if (t->sign == BN_NEGATIVE) printf("-");
    else printf("+");
    for (size_t i = t->size - 1; i > 0; i--) {
        printf("%010lu\'",t->body[i]);
    }
    printf("%010lu\n", t->body[0]);
    return BN_OK;
}

int bn_init_int(bn* t, int init_int) {
    t->body[0] = init_int;
    return BN_OK;
}

int main() {
    bn* a = bn_new();
    bn_init_int(a, 123152); 
    _bn_print(a);
    bn_delete(a);
    return 0;
}
