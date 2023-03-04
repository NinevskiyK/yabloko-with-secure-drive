#include "string.h"
#include "errno.h"

int strncmp(const char* s1, const char* s2, size_t size) {
    while (size && *s1 && *s2 && *s1 == *s2) {
        size--;
        s1++;
        s2++;
    }
    if (!size) {
        return 0;
    }
    return (unsigned char)(*s1) - (unsigned char)(*s2);
}


int strcmp(const char* s1, const char* s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (unsigned char)(*s1) - (unsigned char)(*s2);
}

int strtoi(char *s) {
    int res = 0;
    for (; *s != '\0'; ++s) {
        if ('0' > *s || *s > '9') {
            m_errno = NAN_ERR;
            return 0;
        }
        if (__builtin_mul_overflow(res, 10, &res) || __builtin_add_overflow(res, *s - '0', &res)) {
            m_errno = OVERFLOW_ERR;
            return 0;
        }
    }
    return res;
}

void memset(void* b, char c, size_t len) {
    char* p = b;
    for (size_t i = 0; i < len; ++i) {
        p[i] = c;
    }
}
