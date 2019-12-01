#include "passwd.h"
#include <string.h>

#define Streq(s1, s2) (strcmp((s1), (s2)) == 0)

/* for test */
int check_passwd(const char *name, const char *passwd)
{
    if (Streq(name, "abc") && Streq(passwd, "123")) {
        return 0;
    }
    return -1;
}

/* todo */
int add_user(const char *name, const char *passwd)
{
    return -1;
}