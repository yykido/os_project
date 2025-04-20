#include "shell.h"
#include "../drivers/screen.h"
#include "../libc/string.h"
#include "../libc/mem.h"
#include <stdint.h>

// Simple file system structure (for demonstration)
#define MAX_FILES 100
#define MAX_DIRS 50
#define MAX_NAME_LENGTH 32
#define MAX_PATH_LENGTH 256

enum EntryType {
    FILE_ENTRY,
    DIRECTORY_ENTRY
};

struct FileSystemEntry {
    char name[MAX_NAME_LENGTH];
    uint32_t size;
    enum EntryType type;
    uint32_t parent_dir;  // Index of parent directory, 0 for root
};

struct FileSystemEntry entries[MAX_FILES + MAX_DIRS];
uint32_t entry_count = 0;
uint32_t current_dir = 0;  // 0 is root directory

// Function to get the index of a directory by name
int find_directory(const char* dirname, uint32_t parent_dir) {
    for (uint32_t i = 0; i < entry_count; i++) {
        if (entries[i].type == DIRECTORY_ENTRY && 
            entries[i].parent_dir == parent_dir && 
            strcmp(entries[i].name, dirname) == 0) {
            return i;
        }
    }
    return -1;  // Directory not found
}

// Function to get the current directory path
void get_current_path(char* path) {
    if (current_dir == 0) {
        strcpy(path, "/");
        return;
    }

    char temp_path[MAX_PATH_LENGTH] = "";
    uint32_t dir = current_dir;

    // Build path in reverse order
    while (dir != 0) {
        char dir_name[MAX_NAME_LENGTH];
        strcpy(dir_name, entries[dir].name);
        strcat(dir_name, "/");
        strcat(dir_name, temp_path);
        strcpy(temp_path, dir_name);
        dir = entries[dir].parent_dir;
    }

    // Add root slash
    strcpy(path, "/");
    strcat(path, temp_path);
}

void shell_ls() {
    char path[MAX_PATH_LENGTH];
    get_current_path(path);
    kprint("Contents of ");
    kprint(path);
    kprint(":\n");
    
    for (uint32_t i = 0; i < entry_count; i++) {
        if (entries[i].parent_dir == current_dir) {
            if (entries[i].type == DIRECTORY_ENTRY) {
                kprint("[DIR] ");
            } else {
                kprint("[FILE] ");
            }
            kprint(entries[i].name);
            kprint("\n");
        }
    }
}

void shell_mkdir(const char* dirname) {
    if (entry_count >= MAX_FILES + MAX_DIRS) {
        kprint("Maximum number of entries reached\n");
        return;
    }

    // Check if directory already exists
    for (uint32_t i = 0; i < entry_count; i++) {
        if (entries[i].parent_dir == current_dir && 
            strcmp(entries[i].name, dirname) == 0) {
            kprint("Directory already exists\n");
            return;
        }
    }

    // Create new directory
    strcpy(entries[entry_count].name, dirname);
    entries[entry_count].size = 0;
    entries[entry_count].type = DIRECTORY_ENTRY;
    entries[entry_count].parent_dir = current_dir;
    entry_count++;
    kprint("Directory created successfully\n");
}

void shell_mv(const char* source, const char* dest) {
    int found = 0;
    for (uint32_t i = 0; i < entry_count; i++) {
        if (entries[i].parent_dir == current_dir && 
            strcmp(entries[i].name, source) == 0) {
            strcpy(entries[i].name, dest);
            found = 1;
            kprint("Entry moved successfully\n");
            break;
        }
    }
    if (!found) {
        kprint("Source entry not found\n");
    }
}

void shell_rename(const char* oldname, const char* newname) {
    int found = 0;
    for (uint32_t i = 0; i < entry_count; i++) {
        if (entries[i].parent_dir == current_dir && 
            strcmp(entries[i].name, oldname) == 0) {
            strcpy(entries[i].name, newname);
            found = 1;
            kprint("Entry renamed successfully\n");
            break;
        }
    }
    if (!found) {
        kprint("Entry not found\n");
    }
}

void shell_touch(const char* filename) {
    if (entry_count >= MAX_FILES + MAX_DIRS) {
        kprint("Maximum number of entries reached\n");
        return;
    }
    
    strcpy(entries[entry_count].name, filename);
    entries[entry_count].size = 0;
    entries[entry_count].type = FILE_ENTRY;
    entries[entry_count].parent_dir = current_dir;
    entry_count++;
    kprint("File created successfully\n");
}

void shell_rm(const char* name) {
    int found = 0;
    for (uint32_t i = 0; i < entry_count; i++) {
        if (entries[i].parent_dir == current_dir && 
            strcmp(entries[i].name, name) == 0) {
            // If it's a directory, check if it's empty
            if (entries[i].type == DIRECTORY_ENTRY) {
                int has_children = 0;
                for (uint32_t j = 0; j < entry_count; j++) {
                    if (entries[j].parent_dir == i) {
                        has_children = 1;
                        break;
                    }
                }
                if (has_children) {
                    kprint("Directory is not empty\n");
                    return;
                }
            }
            // Remove the entry by shifting all entries after it
            for (uint32_t j = i; j < entry_count - 1; j++) {
                entries[j] = entries[j + 1];
            }
            entry_count--;
            found = 1;
            kprint("Entry deleted successfully\n");
            break;
        }
    }
    if (!found) {
        kprint("Entry not found\n");
    }
}

void shell_clear() {
    clear_screen();
}

void shell_help() {
    kprint("Available commands:\n");
    kprint("ls - List directory contents\n");
    kprint("mkdir <dirname> - Create a new directory\n");
    // kprint("mv <source> <dest> - Move or rename a file/directory\n");
    kprint("rename <oldname> <newname> - Rename a file/directory\n");
    kprint("touch <filename> - Create a new file\n");
    kprint("rm <name> - Remove a file or directory\n");
    kprint("clear - Clear the screen\n");
    kprint("help - Show this help message\n");
    kprint("quit - Exit the shell\n");
}

// Helper function to parse command arguments
void parse_command(char* input, char* command, char* arg1, char* arg2) {
    int i = 0;
    int j = 0;
    
    // Parse command
    while (input[i] != ' ' && input[i] != '\0') {
        command[j++] = input[i++];
    }
    command[j] = '\0';
    
    // Skip spaces
    while (input[i] == ' ') i++;
    
    // Parse first argument
    j = 0;
    while (input[i] != ' ' && input[i] != '\0') {
        arg1[j++] = input[i++];
    }
    arg1[j] = '\0';
    
    // Skip spaces
    while (input[i] == ' ') i++;
    
    // Parse second argument
    j = 0;
    while (input[i] != ' ' && input[i] != '\0') {
        arg2[j++] = input[i++];
    }
    arg2[j] = '\0';
}

void shell_execute(char* input) {
    if (strcmp(input, "END") == 0) {
        kprint("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    }
    if (strcmp(input, "clear") == 0) {
        shell_clear();
        return;
    }
    if (strcmp(input, "help") == 0) {
        shell_help();
        return;
    }
    if (strcmp(input, "ls") == 0) {
        shell_ls();
        return;
    }
    if (strcmp(input, "quit") == 0) {
        kprint("Exiting shell. Bye!\n");
        asm volatile("hlt");
    }

    // Handle commands with arguments
    char command[32] = "";
    char arg1[32] = "";
    char arg2[32] = "";
    parse_command(input, command, arg1, arg2);

    if (strcmp(command, "mkdir") == 0 && arg1[0] != '\0') {
        shell_mkdir(arg1);
    } 
    // else if (strcmp(command, "mv") == 0 && arg1[0] != '\0' && arg2[0] != '\0') {
    //     shell_mv(arg1, arg2);

    else if (strcmp(command, "rename") == 0 && arg1[0] != '\0' && arg2[0] != '\0') {
        shell_rename(arg1, arg2);
    } else if (strcmp(command, "touch") == 0 && arg1[0] != '\0') {
        shell_touch(arg1);
    } else if (strcmp(command, "rm") == 0 && arg1[0] != '\0') {
        shell_rm(arg1);
    } else {
        kprint("Unknown command or missing arguments. Type 'help' for available commands.\n");
    }
} 