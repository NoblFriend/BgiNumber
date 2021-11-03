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

int _bn_swap (bn* a, bn* b);
int _bn_realloc (bn* src, size_t new_size);
int _bn_remove_leading_zeros (bn* src);

int _bn_print (bn* t);

int _bn_positive_add_to (bn* t, bn const *right);
int _bn_positive_sub_to (bn* t, bn const *right);
int _bn_mul_int (bn* t, unsigned int factor);

//interface
bn* bn_new();
bn* bn_init(const bn* orig);
int bn_delete (bn* t);

int bn_init_int (bn* t, int init_int);
int bn_init_string (bn* t, const char* init_string);

int bn_add_to (bn* t, const bn* right);
int bn_sub_to (bn* t, const bn* right);

int bn_cmp (const bn* left, const bn* right);
int bn_neg (bn* t);
int bn_abs (bn* t);

int main() {
    while (true) {
        bn* a = bn_new();
        bn* b = bn_new();
        char c1 [40] = {};
        char c2 [40] = {};
	    scanf ("%s %s", c1, c2);
        if (c1[0] == 'q') break;
	    bn_init_string (a, c1); 
	    bn_init_string (b, c2); 
	    _bn_print(a);
	    _bn_print(b);
        bn_sub_to(a, b);
	    _bn_print(a);
	    printf("\n");
        bn_delete(a);
        bn_delete(b);
    }
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

int _bn_swap (bn* a, bn* b) {
    if (a == nullptr || b == nullptr) return BN_NULL_OBJECT;

    if (b->size > a->size) {
        bn* tmp = a;
        a = b;
        b = tmp;
    } //now a->size >= b->size
    
    int code = BN_OK;

    code = _bn_realloc (b, a->size);
    if (code != BN_OK) return code;

    for (size_t i = 0; i < a->size; i++) {
        Digit temp = a->body[i];
        a->body[i] = b->body[i];
        b->body[i] = temp;
    }

    code = _bn_remove_leading_zeros (a);
    if (code != BN_OK) return code;

    { //aka swap
        BnSign tmp = a->sign;
        a->sign = b->sign;
        b->sign = tmp;
    }

    return BN_OK;
}

int _bn_realloc (bn* src, size_t new_size) {
    if (src == nullptr) return BN_NULL_OBJECT;
    if (new_size <= src->size) return BN_OK;

    Digit* new_body = new Digit[new_size] ();
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
    for (; real_size > 1 && src->body[real_size-1] == 0; real_size--);

    Digit* new_body = new Digit[real_size] ();
    if (new_body == nullptr) {
        real_size = src->size;
        return BN_NO_MEMORY;
    }
    for (size_t i = 0; i < real_size; i++) {
        new_body[i] = src->body[i];
    }
    delete [] src->body;
    src->body = new_body;
    src->size = real_size;
    if (src->size == 1 && src->body[0] == 0) src->sign = BN_ZERO;
    return BN_OK;
    
}

int _bn_print (bn* t) {
    if (t->body == nullptr) return BN_NULL_OBJECT;

    printf("Size: %zu\n", t->size);
    printf("Body: ");

    if      (t->sign == BN_NEGATIVE) printf("-");
    else if (t->sign == BN_POSITIVE) printf("+");
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

    return BN_OK;
}

int _bn_positive_sub_to (bn* t, bn const *right) {
    if (t == nullptr || right == nullptr) return BN_NULL_OBJECT;
    
    int abscmp = 0;
    if (t->size > right->size) abscmp = 1;
    else if (t->size < right->size) abscmp = -1;
    else {
        for (ssize_t i = t->size - 1; i >= 0; i--) {
            if (t->body[i] != right->body[i]) {
                abscmp = t->body[i] - right->body[i];
                break;
            }
        }
        
    }

    printf("abscmp == %d\n", abscmp);

    if (abscmp == 0) {
        t->size = 1;
        delete [] t->body;
        t->body = new Digit[1];
        t->body[0] = 0;
        t->sign = BN_ZERO;
        return BN_OK;
    }
    
    if (abscmp > 0) {
        t->sign = BN_POSITIVE;
	    Digit credit = 0;
	    for (size_t i = 0; i < right->size; i++) {
	        ExtDigit temp = t->body[i] - right->body[i] - credit;
	        if (temp < 0) {
	            temp += BN_RADIX;
	            credit = 1;
	        } else {
	            credit = 0;
	        }
	        t->body[i] = temp;
	    }
	    if (credit != 0) {
	        size_t i = right->size;
	        for (;/* i < t->size && */t->body[i] == 0; i++) {
	            t->body[i] = BN_RADIX - 1;
	        }
	        t->body[i]--;
	    }
	    _bn_remove_leading_zeros(t);
	    return BN_OK;
    }

    // (if abscmp < 0)
        size_t t_real_size = t->size;
        _bn_realloc (t, right->size);
        t->sign = BN_NEGATIVE;
        Digit credit = 0;
	    for (size_t i = 0; i < t_real_size; i++) {
	        ExtDigit temp = right->body[i] - t->body[i] - credit;
	        if (temp < 0) {
	            temp += BN_RADIX;
	            credit = 1;
	        } else {
	            credit = 0;
	        }
	        t->body[i] = temp;
	    }
	    size_t i = t_real_size;
	    if (credit != 0) { // REWORK!
	        for (;/* i < t->size && */right->body[i] == 0; i++) {
	            t->body[i] = BN_RADIX - 1;
	        }
	        t->body[i] = right->body[i] - 1;
            i++;
	    }
        for (;i < t->size; i++) {
            t->body[i] = right->body[i];
        }
        _bn_remove_leading_zeros(t);
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

    r->body = new Digit[1] ();
    if (r->body == nullptr) {
        delete r;
        return nullptr;
    }

    r->size = 1;
    r->sign = BN_ZERO;
    r->body[0] = 0;
    
    return r;
}

bn* bn_init (const bn* orig) {
    bn* r = new bn;
    if (r == nullptr) return nullptr;

    r->size = orig->size;
    r->sign = orig->sign;
    r->body = new Digit[r->size] ();
    if(r->body == nullptr) {
        delete r;
        return nullptr;
    }

    for (size_t i = 0; i < r->size; i++) {
        r->body[i] = orig->body[i];
    }

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

int bn_add_to (bn* t, const bn* right) {
    int code = BN_OK;
    const BnSign ls = t->sign;
    const BnSign rs = right->sign;
         if (ls == BN_POSITIVE && rs == BN_POSITIVE) {
        code = _bn_positive_add_to (t, right);
    } 
    else if (ls == BN_NEGATIVE && rs == BN_NEGATIVE) {
        code = _bn_positive_add_to (t, right);
    }
    else if (ls == BN_NEGATIVE && rs == BN_POSITIVE) {
        code = _bn_positive_sub_to (t, right); 
        bn_neg (t);
    }
    else if (ls == BN_POSITIVE && rs == BN_NEGATIVE) {
        code = _bn_positive_sub_to (t, right); 
    }
    else if (ls == BN_ZERO     && rs != BN_ZERO    ) {
        bn_delete (t);
        t = bn_init (right);
    } //else if (rs == BN_ZERO) nothing to do
    return code;
}

int bn_sub_to (bn* t, const bn* right) {
    int code = BN_OK;
    const BnSign ls = t->sign;
    const BnSign rs = right->sign;
         if (ls == BN_POSITIVE && rs == BN_POSITIVE) {
        code = _bn_positive_sub_to (t, right);
    } 
    else if (ls == BN_NEGATIVE && rs == BN_NEGATIVE) {
        code = _bn_positive_sub_to (t, right);
        bn_neg (t);
    }
    else if (ls == BN_NEGATIVE && rs == BN_POSITIVE) {
        code = _bn_positive_add_to (t, right); 
    }
    else if (ls == BN_POSITIVE && rs == BN_NEGATIVE) {
        code = _bn_positive_add_to (t, right); 
    }
    else if (ls == BN_ZERO     && rs != BN_ZERO    ) {
        bn_delete (t);
        t = bn_init (right);
        bn_neg (t);
    } //else if (rs == BN_ZERO) nothing to do
    return code;
}

int bn_init_string (bn* t, const char* init_string) {
    if (t == nullptr) return BN_NULL_OBJECT;
    
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
        if (code != BN_OK) return code;
        code = bn_init_int (buf, init_string[i] - '0');
        if (code != BN_OK) return code;
        code = _bn_positive_add_to(t, buf);
        if (code != BN_OK) return code;
    }

    bn_delete(buf);
    return BN_OK;    
}

int bn_cmp (const bn* left, const bn* right) {
//  if (left == nullptr || right == nullptr) return BN_NULL_OBJECT;
    if (left->sign == BN_POSITIVE && right->sign != BN_POSITIVE) return  1;
    if (left->sign == BN_NEGATIVE && right->sign != BN_NEGATIVE) return -1;
    if (left->sign == BN_ZERO     && right->sign == BN_NEGATIVE) return  1;
    if (left->sign == BN_ZERO     && right->sign == BN_POSITIVE) return -1;
    if (left->sign == BN_ZERO     && right->sign == BN_ZERO    ) return  0;

    int f = 1;
    if (left->sign == BN_NEGATIVE) f = -1;

    if (left->size > right->size) return  1*f;
    if (left->size < right->size) return -1*f; 

    for (size_t i = 0; i < left->size; i++) {
        if (left->body[i] != right->body[i]) return left->body[i] - right->body[i];
    }

    return 0;
}

int bn_neg (bn* t) {
    if (t == nullptr) return BN_NULL_OBJECT;
         if (t->sign == BN_POSITIVE) t->sign = BN_NEGATIVE;
    else if (t->sign == BN_NEGATIVE) t->sign = BN_POSITIVE;
    return BN_OK;
}

int bn_abs (bn* t) {
    if (t == nullptr) return BN_NULL_OBJECT;
    if (t->sign != BN_ZERO) t->sign = BN_POSITIVE;
    return BN_OK;
}
