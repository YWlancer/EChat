#ifndef _passwd_h_
#define _passwd_h_

extern "C" int check_passwd(char *cstr_name, char *cstr_passwd);
extern "C" int add_user(char *cstr_name, char *cstr_passwd);

#endif
