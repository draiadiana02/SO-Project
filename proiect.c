#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>

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

void process_entry(const char *file_name, const char *output_dir, const char *entry_name)
{
        // Construiește calea completă pentru fiecare intrare
        char filePath[512];
        char newLine[2] = "\n";
        write(statistica_fd, newLine,strlen(newLine) );
        sprintf(filePath, "%s/%s", file_name, entry_name);
        write(statistica_fd, filePath, strlen(filePath));
        write(statistica_fd, newLine,strlen(newLine) );
        
        //variabilele pentru statistici
    int width, height, size;
    struct stat file_info;
    if (stat(filePath, &file_info) == -1)
    {
        perror("Eroare la obtinerea informatiilor despre fisier");
        exit(1);
    }

    // in caz ca fisierul dat e legatura simbolica
    struct stat file_info2;
    if (lstat(filePath, &file_info2) == -1) {
        perror("Eroare la obținerea informațiilor despre leg simbolica");
        exit(EXIT_FAILURE);
    }

    //verific extensia fisierului pentru a vedea de ce tip e, daca e bmp
    char *ext = strrchr(entry_name, '.');
    
     if (ext && strcmp(ext, ".bmp") == 0) {
        // daca e fisier bmp
        int input_fd = open(filePath, O_RDONLY);
        read_bmp_info(input_fd, &width, &height, &size);
        write_statistics(statistica_fd, entry_name, width, height, size, &file_info);
    }
    else if (S_ISDIR(file_info.st_mode)) 
    {
         // Este un director
        char something[20] = "este director\n";
        write(statistica_fd, something,strlen(something) );
        write_statistics_director(statistica_fd, entry_name, &file_info);
       
    }
    else if (S_ISLNK(file_info2.st_mode))
    {
        // Este o legătură simbolică
        char something[20] = "este leg simbolica\n";
        write(statistica_fd, entry_name,strlen(entry_name));
        write_statistics_symbolic_link(statistica_fd, filePath, &file_info2, file_name);
    }
    else if (S_ISREG(file_info.st_mode) && S_ISLNK(file_info2.st_mode) == 0)
    {
        // daca e fisier obisnuit, fara bmp
        char something[200] = "este fisier obisnuit fara bmp\n";
        write(statistica_fd, something,strlen(something) );
        write_statistics_non_bmp(statistica_fd,  entry_name, &file_info);

    }
}

void process_dir(const char *input_dir, const char *output_dir) {
    DIR *dir = open_director(input_dir);

    // Parcurge fiecare intrare din director
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
    // Ignoră intrările curente și părinte
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
        continue;
    }
        process_entry(input_dir, output_dir, entry->d_name);
    }
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
    
    int statistica_fd = create_stat_file();
    process_dir(argv[1], argv[2]);
    
    close(statistica_fd);
    return 0;
}