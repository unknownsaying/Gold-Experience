#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define MAX_PATH 1024

// Function to get file type as string
const char* get_file_type(mode_t mode) {
    if (S_ISREG(mode)) return "Regular";
    if (S_ISDIR(mode)) return "Directory";
    if (S_ISLNK(mode)) return "Symbolic Link";
    if (S_ISCHR(mode)) return "Character Device";
    if (S_ISBLK(mode)) return "Block Device";
    if (S_ISFIFO(mode)) return "FIFO/Pipe";
    if (S_ISSOCK(mode)) return "Socket";
    return "Unknown";
}

// Function to get permissions string (e.g., "rwxr-xr-x")
void get_permissions(mode_t mode, char *perms) {
    const char *rwx[] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"};
    strcpy(perms, rwx[(mode >> 6) & 7]);  // Owner
    strcat(perms, rwx[(mode >> 3) & 7]);  // Group
    strcat(perms, rwx[mode & 7]);          // Others
}

// Function to display filesystem tree
void display_tree(const char *path, int depth, int max_depth) {
    if (depth > max_depth) return;
    
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char fullpath[MAX_PATH];
    
    // Indentation for tree structure
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    
    // Get the base name of the path
    const char *basename = strrchr(path, '/');
    if (basename == NULL) basename = path;
    else basename++;
    
    if (depth == 0) {
        printf("📁 / (Root)\n");
    } else {
        printf("📁 %s\n", basename);
    }
    
    if ((dir = opendir(path)) == NULL) return;
    
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
            
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
        
        if (stat(fullpath, &statbuf) == -1) continue;
        
        if (S_ISDIR(statbuf.st_mode)) {
            display_tree(fullpath, depth + 1, max_depth);
        } else {
            // Display files with indentation
            for (int i = 0; i <= depth; i++) {
                printf("  ");
            }
            printf("📄 %s\n", entry->d_name);
        }
    }
    closedir(dir);
}

// Function to display filesystem information
void display_filesystem_info() {
    printf("\n=== LINUX FILESYSTEM STRUCTURE ===\n\n");
    
    // Key directories to examine
    const char *directories[] = {
        "/", "/bin", "/etc", "/home", "/var", "/tmp", "/dev", "/proc", "/usr", "/boot"
    };
    
    for (int i = 0; i < 10; i++) {
        struct stat st;
        char perms[10];
        
        if (stat(directories[i], &st) == 0) {
            get_permissions(st.st_mode, perms);
            printf("%-10s [%s] %s\n", 
                   directories[i], 
                   perms, 
                   get_file_type(st.st_mode));
        }
    }
}

// Function to simulate filesystem navigation
void navigate_filesystem() {
    char cwd[MAX_PATH];
    char command[100];
    
    printf("\n=== FILESYSTEM NAVIGATION SIMULATION ===\n");
    printf("Commands: 'ls', 'pwd', 'cd [dir]', 'info [file]', 'quit'\n\n");
    
    while (1) {
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd");
            break;
        }
        
        printf("[%s] $ ", cwd);
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;
        
        if (strcmp(command, "quit") == 0) break;
        else if (strcmp(command, "pwd") == 0) {
            printf("%s\n", cwd);
        }
        else if (strcmp(command, "ls") == 0) {
            DIR *dir;
            struct dirent *entry;
            
            if ((dir = opendir(cwd)) != NULL) {
                while ((entry = readdir(dir)) != NULL) {
                    struct stat st;
                    char fullpath[MAX_PATH];
                    snprintf(fullpath, sizeof(fullpath), "%s/%s", cwd, entry->d_name);
                    
                    if (stat(fullpath, &st) == 0) {
                        if (S_ISDIR(st.st_mode))
                            printf("📁 ");
                        else if (S_ISREG(st.st_mode))
                            printf("📄 ");
                        else
                            printf("🔗 ");
                        printf("%s\n", entry->d_name);
                    }
                }
                closedir(dir);
            }
        }
        else if (strncmp(command, "cd ", 3) == 0) {
            if (chdir(command + 3) != 0) {
                perror("cd");
            }
        }
        else if (strncmp(command, "info ", 5) == 0) {
            char filepath[MAX_PATH];
            struct stat st;
            snprintf(filepath, sizeof(filepath), "%s/%s", cwd, command + 5);
            
            if (stat(filepath, &st) == 0) {
                char perms[10];
                get_permissions(st.st_mode, perms);
                printf("File: %s\n", command + 5);
                printf("Type: %s\n", get_file_type(st.st_mode));
                printf("Size: %ld bytes\n", st.st_size);
                printf("Permissions: %s\n", perms);
                printf("Owner: %d\n", st.st_uid);
                printf("Last modified: %s", ctime(&st.st_mtime));
            } else {
                printf("File not found\n");
            }
        }
        else {
            printf("Unknown command\n");
        }
    }
}

// Function to create a visual representation
void visualize_hierarchy() {
    printf("\n=== LINUX FILESYSTEM HIERARCHY ===\n\n");
    printf("                        /\n");
    printf("                        │\n");
    printf("        ┌───────┬───────┼───────┬───────┬───────┬───────┐\n");
    printf("        │       │       │       │       │       │       │\n");
    printf("       bin     etc    home    var     tmp     dev     usr\n");
    printf("        │       │       │       │       │       │       │\n");
    printf("        │       │    ┌──┼──┐    │       │       │    ┌──┼──┐\n");
    printf("        │       │    │  │  │    │       │       │    │  │  │\n");
    printf("       ls     passwd user1 logs    data   sda    bin   local share\n");
    printf("\n");
}

// Function to show inode information
void show_inode_example() {
    printf("\n=== INODE STRUCTURE EXAMPLE ===\n\n");
    
    struct stat sb;
    if (stat("/etc/passwd", &sb) == 0) {
        printf("File: /etc/passwd\n");
        printf("Inode number: %lu\n", (unsigned long)sb.st_ino);
        printf("Number of hard links: %lu\n", (unsigned long)sb.st_nlink);
        printf("File size: %lld bytes\n", (long long)sb.st_size);
        printf("Blocks allocated: %lld\n", (long long)sb.st_blocks);
        printf("Block size: %ld bytes\n", (long)sb.st_blksize);
    }
}

int main() {
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║     LINUX FILESYSTEM ILLUSTRATION IN C              ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");
    
    // 1. Display key directories
    display_filesystem_info();
    
    // 2. Show visual hierarchy
    visualize_hierarchy();
    
    // 3. Show inode example
    show_inode_example();
    
    // 4. Display tree structure (limited depth to avoid too much output)
    printf("\n=== FILESYSTEM TREE (Limited to depth 2) ===\n\n");
    display_tree("/", 0, 2);
    
    // 5. Interactive navigation
    printf("\n");
    navigate_filesystem();
    
    return 0;
}