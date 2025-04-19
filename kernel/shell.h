#ifndef SHELL_H
#define SHELL_H

void shell_ls();
void shell_mv(const char* source, const char* dest);
void shell_rename(const char* oldname, const char* newname);
void shell_touch(const char* filename);
void shell_rm(const char* filename);
void shell_mkdir(const char* dirname);
void shell_clear();
void shell_help();
void shell_execute(char* input);

#endif 