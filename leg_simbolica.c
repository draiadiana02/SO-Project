#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *file_path = argv[1];
    struct stat file_info;

    // Obține informații despre fișier
    if (lstat(file_path, &file_info) == -1) {
        perror("Eroare la obținerea informațiilor despre fișier");
        exit(EXIT_FAILURE);
    }

    // Verifică dacă fișierul este o legătură simbolică
    if (S_ISLNK(file_info.st_mode)) {
        char target_path[1024];
        ssize_t target_path_size = readlink(file_path, target_path, sizeof(target_path) - 1);
        if (target_path_size == -1) {
            perror("Eroare la citirea legăturii simbolice");
            exit(EXIT_FAILURE);
        }
        target_path[target_path_size] = '\0';

        // Obține informații despre fișierul țintă
        struct stat target_file_info;
        if (lstat(target_path, &target_file_info) == -1) {
            perror("Eroare la obținerea informațiilor despre fișierul țintă");
            exit(EXIT_FAILURE);
        }

        // Verifică dacă fișierul țintă este un fișier obișnuit
        if (S_ISREG(target_file_info.st_mode)) {
            printf("Este o legătură simbolică către un fișier obișnuit\n");
        } else {
            printf("Este o legătură simbolică, dar țintește către altceva decât un fișier obișnuit\n");
        }
    } else {
        printf("Nu este o legătură simbolică\n");
    }

    return 0;
}
