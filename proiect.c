#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <dirent.h>
#include <stdint.h>

DIR *open_director(const char *director_name) 
{
    DIR *dir = opendir(director_name);
    if(dir == NULL)
    {
        perror("Eroare la deschiderea directorului de intrare");
        exit(1);
    }
    return dir;
}

int create_stat_file() 
{
    int statistica_fd = creat("statistica.txt", S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP);
    if(statistica_fd == -1)
    {
        perror("Eroare la crearea fisierului de statistica");
        exit(1);
    }
    return statistica_fd;
}

void close_director(DIR * dir)
{
    if(closedir(dir) == -1)
    {
        perror("Eroare la inchiderea directorului");
    }
}

int read_bmp_info(int input_fd, int *width, int *height, int *size)
{
    if(lseek(input_fd, 18, SEEK_SET) == -1) {
        printf("Eroare la mutarea cursorului");
        exit(1);
    }
    
    if(read(input_fd, width, 4) == -1) {
        printf("Eroare la citirea lungimii");
        exit(1);
    }

    if(read(input_fd, height, 4) == -1) {
        printf("Eroare la citirea inaltimii");
        exit(1);
    }

    if(lseek(input_fd, 2, SEEK_SET) == -1) {
        printf("Eroare la mutarea cursorului");
        exit(1);
    }

    if(read(input_fd, size, 4) == -1) {
        printf("Eroare la citirea dimensiunii in octeti");
        exit(1);
    }
    
    return 0;
}

char *drepturi_acces_user(struct stat *file_info, char *drepturi_user)
{

    if (file_info->st_mode & S_IRUSR) {
        drepturi_user[0] = 'R';
    } else {
        drepturi_user[0] = '-';
    }

    if (file_info->st_mode & S_IWUSR) {
        drepturi_user[1] = 'W';
    } else {
        drepturi_user[1] = '-';
    }

    if (file_info->st_mode & S_IXUSR) {
        drepturi_user[2] = 'X';
    } else {
        drepturi_user[2] = '-';
    }
    drepturi_user[3] = '\0';
    
    return drepturi_user;
}

char *drepturi_acces_grup(struct stat *file_info, char *drepturi_grup)
{

    if (file_info->st_mode & S_IRGRP) {
        drepturi_grup[0] = 'R';
    } else {
        drepturi_grup[0] = '-';
    }

    if (file_info->st_mode & S_IWGRP) {
        drepturi_grup[1] = 'W';
    } else {
        drepturi_grup[1] = '-';
    }

    if (file_info->st_mode & S_IXGRP) {
        drepturi_grup[2] = 'X';
    } else {
        drepturi_grup[2] = '-';
    }
    drepturi_grup[3] = '\0';
    
    return drepturi_grup;
}

char *drepturi_acces_altii(struct stat *file_info, char *drepturi_altii)
{

    if (file_info->st_mode & S_IROTH) {
        drepturi_altii[0] = 'R';
    } else {
        drepturi_altii[0] = '-';
    }

    if (file_info->st_mode & S_IWOTH) {
        drepturi_altii[1] = 'W';
    } else {
        drepturi_altii[1] = '-';
    }

    if (file_info->st_mode & S_IXOTH) {
        drepturi_altii[2] = 'X';
    } else {
        drepturi_altii[2] = '-';
    }
    drepturi_altii[3] = '\0';
    
    return drepturi_altii;
}

void write_statistics(int statistica_fd, const char *file_name, int width, int height, int size, struct stat *file_info)
{
    char buffer_str[128];

    sprintf(buffer_str, "Nume fisier: %s\n", file_name);
    write(statistica_fd,buffer_str,strlen(buffer_str));

    sprintf(buffer_str, "Inaltime: %d\n", height);
    write(statistica_fd,buffer_str,strlen(buffer_str));
    
    sprintf(buffer_str, "Lungime: %d\n", width);
    write(statistica_fd,buffer_str,strlen(buffer_str));

    sprintf(buffer_str, "Dimensiune in octeti: <%d>\n", size);
    write(statistica_fd,buffer_str,strlen(buffer_str));

    sprintf(buffer_str, "Identificatorul utilizatorului: <%d>\n", file_info->st_uid);
    write(statistica_fd,buffer_str,strlen(buffer_str));

    struct tm *modification_time = localtime(&file_info->st_mtime);
    sprintf(buffer_str, "Timpul ultimei modificari: %02d.%02d.%04d\n",
        modification_time->tm_mday, modification_time->tm_mon + 1,
        modification_time->tm_year + 1900);
    write(statistica_fd,buffer_str,strlen(buffer_str));

    sprintf(buffer_str, "Numarul de legaturi: <%ld>\n", file_info->st_nlink);
    write(statistica_fd,buffer_str,strlen(buffer_str));

    char drepturi[4];
    strcpy(drepturi, drepturi_acces_user(file_info, drepturi));
    sprintf(buffer_str, "Drepturi de acces user: %s\n", drepturi);
    write(statistica_fd,buffer_str,strlen(buffer_str));

    strcpy(drepturi, drepturi_acces_grup(file_info, drepturi));
    sprintf(buffer_str, "Drepturi de acces grup: %s\n", drepturi);
    write(statistica_fd,buffer_str,strlen(buffer_str));

    strcpy(drepturi, drepturi_acces_altii(file_info, drepturi));
    sprintf(buffer_str, "Drepturi de acces altii: %s\n", drepturi);
    write(statistica_fd,buffer_str,strlen(buffer_str));

}

void write_statistics_non_bmp(int statistica_fd, const char *file_name, struct stat *file_info)
{
    char buffer_str[128];

    sprintf(buffer_str, "Nume fisier: %s\n", file_name);
    write(statistica_fd,buffer_str,strlen(buffer_str));

    sprintf(buffer_str, "Dimensiune in octeti: <%ld>\n", file_info->st_size);
    write(statistica_fd,buffer_str,strlen(buffer_str));

    sprintf(buffer_str, "Identificatorul utilizatorului: <%d>\n", file_info->st_uid);
    write(statistica_fd,buffer_str,strlen(buffer_str));

    struct tm *modification_time = localtime(&file_info->st_mtime);
    sprintf(buffer_str, "Timpul ultimei modificari: %02d.%02d.%04d\n",
        modification_time->tm_mday, modification_time->tm_mon + 1,
        modification_time->tm_year + 1900);
    write(statistica_fd,buffer_str,strlen(buffer_str));

    sprintf(buffer_str, "Numarul de legaturi: <%ld>\n", file_info->st_nlink);
    write(statistica_fd,buffer_str,strlen(buffer_str));

    char drepturi[4];
    strcpy(drepturi, drepturi_acces_user(file_info, drepturi));
    sprintf(buffer_str, "Drepturi de acces user: %s\n", drepturi);
    write(statistica_fd,buffer_str,strlen(buffer_str));

    strcpy(drepturi, drepturi_acces_grup(file_info, drepturi));
    sprintf(buffer_str, "Drepturi de acces grup: %s\n", drepturi);
    write(statistica_fd,buffer_str,strlen(buffer_str));

    strcpy(drepturi, drepturi_acces_altii(file_info, drepturi));
    sprintf(buffer_str, "Drepturi de acces altii: %s\n", drepturi);
    write(statistica_fd,buffer_str,strlen(buffer_str));

}

void write_statistics_symbolic_link(int statistica_fd, const char *file_name, struct stat *file_info, const char *dir)
{
        char buffer_str[1024]; //pt a stoca calea fisierului tinta pt leg simbolica
        ssize_t buffer_str_size = readlink(file_name, buffer_str, sizeof(buffer_str) - 1);
        if (buffer_str_size == -1) {
            perror("Eroare la citirea legăturii simbolice");
            exit(1);
        }
        buffer_str[buffer_str_size] = '\0';
        // Obține informații despre fișierul țintă
        //printf("Cale fișier țintă: %s\n", buffer_str);
       
        const char *directory = dir;
        char full_path[1025];
        snprintf(full_path, sizeof(full_path), "%s/%s", directory, buffer_str);

        struct stat target_file_info;
        if (stat(full_path, &target_file_info) == -1) {
            perror("Eroare la obținerea informațiilor despre fișierul țintă");
            exit(1);
        }

        sprintf(buffer_str, "\nNume legatura: %s\n", file_name);
        write(statistica_fd, buffer_str, strlen(buffer_str));

        sprintf(buffer_str, "Dimensiune legatura: %ld\n", buffer_str_size);
        write(statistica_fd, buffer_str, strlen(buffer_str));

        sprintf(buffer_str, "Dimensiune fisier target: %ld\n", target_file_info.st_size);
        write(statistica_fd, buffer_str, strlen(buffer_str));

        char drepturi[4];
        strcpy(drepturi, drepturi_acces_user(&target_file_info, drepturi));
        sprintf(buffer_str, "Drepturi de acces user legatura: %s\n", drepturi);
        write(statistica_fd, buffer_str, strlen(buffer_str));

        strcpy(drepturi, drepturi_acces_grup(&target_file_info, drepturi));
        sprintf(buffer_str, "Drepturi de acces grup legatura: %s\n", drepturi);
        write(statistica_fd, buffer_str, strlen(buffer_str));

        strcpy(drepturi, drepturi_acces_altii(&target_file_info, drepturi));
        sprintf(buffer_str, "Drepturi de acces altii legatura: %s\n", drepturi);
        write(statistica_fd, buffer_str, strlen(buffer_str));
}

void write_statistics_director(int statistica_fd, const char *file_name, struct stat *file_info)
{
    char buffer_str[256];
    sprintf(buffer_str, "Nume director: %s\n", file_name);
    write(statistica_fd, buffer_str, strlen(buffer_str));

    sprintf(buffer_str, "Identificatorul utilizatorului: %d\n", file_info->st_uid);
    write(statistica_fd, buffer_str, strlen(buffer_str));

    char drepturi[4];
    strcpy(drepturi, drepturi_acces_user(file_info, drepturi));
    sprintf(buffer_str, "Drepturi de acces user: %s\n", drepturi);
    write(statistica_fd, buffer_str, strlen(buffer_str));

    strcpy(drepturi, drepturi_acces_grup(file_info, drepturi));
    sprintf(buffer_str, "Drepturi de acces grup: %s\n", drepturi);
    write(statistica_fd, buffer_str, strlen(buffer_str));

    strcpy(drepturi, drepturi_acces_altii(file_info, drepturi));
    sprintf(buffer_str, "Drepturi de acces altii: %s\n", drepturi);
    write(statistica_fd, buffer_str, strlen(buffer_str));
}

void write_new_line(int statistica_fd)
{
    char newLine[2] = "\n";
    write(statistica_fd, newLine,strlen(newLine) );
}

void count_lines(char *output_file_path, int pipe_fd[2])
{
    int num_linii = 0;
    char linie[512]; // Dimensiunea maximă a unei linii

    // Deschide fișierul pentru citire
    int statistica_fd_citire = open(output_file_path, O_RDONLY);
    if (statistica_fd_citire == -1) {
        perror("Eroare la deschiderea fișierului de statistica pentru citire");
        exit(1);
    }

    // Numără liniile utilizând read()
    char caracter;
    while (read(statistica_fd_citire, &caracter, 1) > 0) {
        if (caracter == '\n') {
            num_linii++;
        }
    }

    // Trimite numărul de linii către procesul părinte prin pipe
    write(pipe_fd[1], &num_linii, sizeof(int));

    // Închide fișierul și pipe-ul
    close(statistica_fd_citire);
}

void process_entry(const char *file_name, const char *output_dir, const char *entry_name, int pipe_fd[2])
{
    // Construiește calea completă pentru fișierul de ieșire
    char output_file_path[512];
    sprintf(output_file_path, "%s/%s_statistica.txt", output_dir, entry_name);

    // Deschide fișierul de ieșire în directorul corespunzător
    int statistica_fd = open(output_file_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);
    if (statistica_fd == -1) {
        perror("Eroare la deschiderea fișierului de statistica");
        exit(1);
    }

    // Construiește calea completă pentru fiecare intrare
    char filePath[512];
    sprintf(filePath, "%s/%s", file_name, entry_name);
    write(statistica_fd, filePath, strlen(filePath));

    // Variabilele pentru statistici
    int width, height, size;
    struct stat file_info;
    if (stat(filePath, &file_info) == -1) {
        perror("Eroare la obtinerea informatiilor despre fisier");
        exit(1);
    }

    // În caz că fisierul dat e legatura simbolica
    struct stat file_info2;
    if (lstat(filePath, &file_info2) == -1) {
        perror("Eroare la obținerea informațiilor despre leg simbolica");
        exit(EXIT_FAILURE);
    }

    // Verific extensia fisierului pentru a vedea de ce tip e, daca e bmp
    char *ext = strrchr(entry_name, '.');

    if (ext && strcmp(ext, ".bmp") == 0) {
        // Daca e fisier bmp
        int input_fd = open(filePath, O_RDONLY);
        read_bmp_info(input_fd, &width, &height, &size);
        write_statistics(statistica_fd, entry_name, width, height, size, &file_info);
    } else if (S_ISDIR(file_info.st_mode)) {
        // Este un director
        char something[20] = "este director\n";
        write(statistica_fd, something, strlen(something));
        write_statistics_director(statistica_fd, entry_name, &file_info);
    } else if (S_ISLNK(file_info2.st_mode)) {
        // Este o legătură simbolică
        char something[20] = "este leg simbolica\n";
        write(statistica_fd, entry_name, strlen(entry_name));
        write_statistics_symbolic_link(statistica_fd, filePath, &file_info2, file_name);
    } else if (S_ISREG(file_info.st_mode) && S_ISLNK(file_info2.st_mode) == 0) {
        // Daca e fisier obisnuit, fara bmp
        char something[200] = "este fisier obisnuit fara bmp\n";
        write(statistica_fd, something, strlen(something));
        write_statistics_non_bmp(statistica_fd, entry_name, &file_info);
    }
    count_lines(output_file_path, pipe_fd);
    close(statistica_fd);
}

void convert_to_gray(const char *filePath)
{
    int width, height, size;
    struct stat file_info;
    if (stat(filePath, &file_info) == -1) {
        perror("Eroare la obtinerea informatiilor despre fisier");
        exit(1);
    }
    int input_fd = open(filePath, O_RDWR);
    read_bmp_info(input_fd, &width, &height, &size);
    int nr_pixels = width * height;

    // Calcularea padding-ului pe linie
    int padding = (4 - (width * 3) % 4) % 4;

    // Deplasare la începutul zonei Raster Data
    lseek(input_fd, 54, SEEK_SET);

    // Parcurgerea fiecarui pixel si aplicarea formulei de conversie la tonuri de gri
    for (int i = 0; i < nr_pixels; ++i) {
        uint8_t pixel[3];

        // Citirea valorilor RGB ale pixelului
        if (read(input_fd, pixel, sizeof(pixel)) == -1) {
            perror("Eroare la citirea pixelului din fisier");
            exit(1);
        }

        // Aplicarea formulei pentru conversie la tonuri de gri
        uint8_t grayscale = 0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0];

        // Deplasare la locația corectă în fișier pentru a suprascrie valorile
        lseek(input_fd, 3, SEEK_CUR);
        write(input_fd, &grayscale, sizeof(uint8_t));
        write(input_fd, &grayscale, sizeof(uint8_t));
        write(input_fd, &grayscale, sizeof(uint8_t));
        
        lseek(input_fd, padding, SEEK_CUR);
    }
    // Inchiderea fisierului BMP
    close(input_fd);
}


void process_dir(const char *input_dir, const char *output_dir) {
    DIR *dir = open_director(input_dir);
    int num_processes = 0;

    int pfd[2];
    if (pipe(pfd) < 0) {
        perror("Eroare la crearea pipe-ului");
        exit(EXIT_FAILURE);
    }
    // Parcurge fiecare intrare din director
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
    // Ignoră intrările curente și părinte
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
        continue;
    }
   
    int pid = fork();
    num_processes++;

    if (pid == -1)
    {
        perror("Eroare la fork");
        exit(-1);
    }
    if (pid == 0)
    {
        close(pfd[0]);
        process_entry(input_dir, output_dir, entry->d_name, pfd);
        close(pfd[1]);
        exit(2);
    }
    char *ext = strrchr(entry->d_name, '.');

    // if (ext && strcmp(ext, ".bmp") == 0) {
        
    //     pid_t pid2;
        
    //     pid2 = fork();
    //     num_processes++;
    //     if (pid2 == -1)
    //     {
    //         perror("Eroare la fork");
    //         exit(-1);
    //     }
    //     if (pid2 == 0)
    //     {
    //         // calea completă pentru imaginea bmp
    //         char bmp_path[512];
    //         sprintf(bmp_path, "%s/%s", input_dir, entry->d_name);
    //         convert_to_gray(bmp_path);
    //         exit(2);
    //     }
    // }

}
    printf("%d", num_processes);
    close(pfd[1]);
    int status;
    for (int i = 0; i < num_processes; ++i) {
        pid_t pid = wait(&status);
        printf("S-a încheiat procesul cu PID-ul %d și codul %d\n", pid, WEXITSTATUS(status));

        int num_linii_fiu;
        read(pfd[0], &num_linii_fiu, sizeof(int));

        printf("Numărul de linii scrise de către procesul cu PID-ul %d: %d\n", pid, num_linii_fiu);
    }
    close(pfd[0]);
    // Închide directorul de intrare
    close_director(dir);
}

int main(int argc, char *argv[])
{
    if(argc != 3) /* ./program <director_intrare> <director iesire> */
    {
        printf("Usage %s <director_intrare> <director_iesire> \n", argv[0]);
        exit(1);
    }
    
    process_dir(argv[1], argv[2]);

    return 0;
}