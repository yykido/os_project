#ifndef STRING_H
#define STRING_H

int strcmp(const char s1[], const char s2[]);
void strcpy(char dest[], const char src[]);
void strcat(char dest[], const char src[]);
void int_to_ascii(int n, char str[]);
void hex_to_ascii(int n, char str[]);
void reverse(char s[]);
int strlen(char s[]);
void append(char s[], char n);
void backspace(char s[]);

#endif
