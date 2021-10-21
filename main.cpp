#include <stdio.h>

typedef unsigned long Digit;
typedef long long ExtDigit;

enum BnSign { BN_POSITIVE, BN_NEGATIVE, BN_ZERO };
enum BnCodes { BN_OK, BN_NULL_OBJECT, BN_NO_MEMORY, BN_DIVIDE_BY_ZERO };
const ExtDigit BN_RADIX = 10;
//const ExtDigit BN_RADIX = 4294967296;

struct bn_s {
    Digit* body;
    size_t size;
    BnSign sign;
};
typedef struct bn_s bn;

//internal
size_t _strlen (const char* str);

size_t _max (size_t a, size_t b);
size_t _min (size_t a, size_t b);

int _bn_realloc (bn* src, size_t new_size);
int _bn_remove_leading_zeros (bn* src);

int _bn_print (bn* t);

int _bn_positive_add_to (bn* t, bn const *right);
int _bn_mul_int (bn* t, unsigned int factor);

//interface
bn* bn_new();
int bn_delete (bn* t);

int bn_init_int (bn* t, int init_int);
int bn_init_string (bn* t, const char* init_string);


int main() {
    bn* a = bn_new();
    bn_init_string (a, "3198371873131448783729381039813982309"); 
    _bn_print(a);
    for(size_t i = 0; i < 12; i++) {
        _bn_positive_add_to(a, a);
        _bn_print(a);
    }
    printf("\n");
    bn_delete(a);
    return 0;
}

size_t _strlen (const char* str) {
    size_t i = 0;
    for (; str[i] != '\0'; i++);
    return i;
}

size_t _max (size_t a, size_t b) {
    return a > b ? a : b;
}

size_t _min (size_t a, size_t b) {
    return a < b ? a : b;
}


int _bn_realloc (bn* src, size_t new_size) {
    if (src == nullptr) return BN_NULL_OBJECT;
    if (new_size <= src->size) return BN_OK;

    Digit* new_body = new Digit[new_size];
    if (new_body == nullptr) return BN_NO_MEMORY;

    for (size_t i = 0; i < src->size; i++) {
        new_body[i] = src->body[i];
    }
    delete [] src->body;
    src->body = new_body;
    src->size = new_size;
    return BN_OK;
}

int _bn_remove_leading_zeros (bn* src) {
    if (src == nullptr) return BN_NULL_OBJECT;
    
    size_t real_size = src->size;
    for (; src->body[real_size-1] == 0; real_size--);

    Digit* new_body = new Digit[real_size];
    if (new_body == nullptr) {
        real_size = src->size;
        return BN_NO_MEMORY;
    }
    for (size_t i = 0; i < src->size; i++) {
        new_body[i] = src->body[i];
    }
    delete [] src->body;
    src->body = new_body;
    src->size = real_size;
    return BN_OK;
    
}

int _bn_print (bn* t) {
    if (t->body == nullptr) return BN_NULL_OBJECT;

    printf("Size: %zu\n", t->size);
    printf("Body: ");

    if (t->sign == BN_NEGATIVE) printf("-");
    else printf("+");
    if (BN_RADIX == 10) {
        for (size_t i = t->size - 1; i > 0; i--) {
            printf("%01lu",t->body[i]);
        }
        printf("%01lu\n", t->body[0]);
    } else {
        for (size_t i = t->size - 1; i > 0; i--) {
            printf("%010lu\'",t->body[i]);
        }
        printf("%010lu\n", t->body[0]);
    }
    return BN_OK;
}

int _bn_positive_add_to (bn* t, bn const *right) {
    size_t min_size = right->size;
    if (t->size < right->size) {
        _bn_realloc(t, right->size);
    }
    ExtDigit buf = 0;
    
    for (size_t i = 0 ; i < right->size; i++) {
        buf += t->body[i] + right->body[i];
        t->body[i] = buf % BN_RADIX;
        buf /= BN_RADIX;
    }
    for (size_t i = right->size; i < t->size; i++) {
        buf += t->body[i];
        t->body[i] = buf % BN_RADIX;
        buf /= BN_RADIX;
    }
    if (buf != 0) {
        _bn_realloc(t, t->size+1);
        t->body[t->size-1] = buf;
    }

//  _bn_remove_leading_zeros(t);

    return BN_OK;
}

int _bn_mul_int (bn* t, unsigned int factor) {
    ExtDigit buf = 0; 

    for (size_t i = 0; i < t->size; i++) {
        buf += t->body[i]*ExtDigit(factor);
        t->body[i] = buf % BN_RADIX;
        buf /= BN_RADIX;
    }
    if (buf != 0) {
        if (_bn_realloc (t, t->size+1) == BN_NO_MEMORY) return BN_NO_MEMORY;
        // TODO BN_NO_MEMORY safety support
        t->body[t->size-1] = buf; 
    }
    return BN_OK;
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

int bn_init_int (bn* t, int init_int) {
    if (t->body == nullptr) return BN_NULL_OBJECT;
    if (init_int > 0 ) {
        t->sign = BN_POSITIVE;
        t->body[0] = init_int;
    } else if (init_int < 0) {
        t->sign = BN_NEGATIVE;
        t->body[0] = -init_int;
    } else {
        t->sign = BN_ZERO;
        t->body[0] = 0;
    }
    return BN_OK;
}

int bn_init_string (bn* t, const char* init_string) {
    size_t i = 0;
    
    t->sign = BN_POSITIVE;
    if (init_string[0] == '+') i++;
    if (init_string[0] == '-') {
        t->sign = BN_NEGATIVE;
        i++;
    }

    size_t str_size = _strlen (init_string);
    
    int code = BN_OK;
    bn* buf = bn_new();

    for (; i < str_size; i++) {
        code = _bn_mul_int (t, 10);
        code = bn_init_int (buf, init_string[i] - '0');
        code = _bn_positive_add_to(t, buf);
    }
    return code;    
}
