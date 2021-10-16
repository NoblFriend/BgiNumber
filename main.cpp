#include <stdio.h>

typedef unsigned long Digit;
typedef long long ExtDigit;

enum BnSign { BN_POSITIVE, BN_NEGATIVE, BN_ZERO };
enum BnCodes { BN_OK, BN_NULL_OBJECT, BN_NO_MEMORY, BN_DIVIDE_BY_ZERO };
const ExtDigit BN_RADIX = /*1000000000*/4294967296;

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

int _bn_print (bn* t) {
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

int bn_init_string (bn* t, const char* init_string) {
//  if (t->body == nullptr) return BN_NULL_OBJECT;

//  size_t strlen = _strlen(init_string);

//  _bn_realloc(t, strlen/9 + 1);

//  ExtDigit buf = 0;
//  ExtDigit ten_degree = 1;

//  size_t cur_index = 0;

//  for (ssize_t i = strlen-1; i >= 0; i--) {
//      buf += (init_string[i] - '0')*ten_degree;
//      ten_degree *= 10;
//      printf("i = %2zd, buf = %14llu, cur = %d, td = %2llu\n", i, buf, init_string[i] - '0', ten_degree);
//      if (buf >= BN_RADIX) {
//          t->body[cur_index++] = buf % BN_RADIX;
//          buf /= BN_RADIX;
//          ten_degree /= 10000000000;
//          printf("\t t->body[%2zu] = %lu\n", cur_index-1, t->body[cur_index-1]);
//      }
//  }
//  if (buf != 0) t->body[cur_index] = buf;
//  return BN_OK;
}

int bn_init_int (bn* t, int init_int) {
    if (t->body == nullptr) return BN_NULL_OBJECT;
    if (init_int > 0 ) {
        t->sign = BN_POSITIVE;
        t->body[0] = init_int;
    } else if (init_int < 0) {
        t->sign = BN_NEGATIVE;
        t->body[0] = -init_int;
    } 
    return BN_OK;
}

int main() {
    bn* a = bn_new();
    bn_init_string(a, "65544332211"); 
    _bn_print(a);
    bn_delete(a);
    return 0;
}
