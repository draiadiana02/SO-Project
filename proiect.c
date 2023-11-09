#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

int open_input_file(const char *file_name) 
{
    int input_fd = open(file_name, O_RDONLY);
    if(input_fd == -1)
    {
        perror("Eroare la deschiderea fisierului de intrare");
        exit(1);
    }
    return input_fd;
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

void close_file(int file)
{
    if(close(file) == -1)
    {
        perror("Eroare la inchiderea fisierului");
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

int main(int argc, char *argv[])
{
    if(argc != 2) 
    {
        printf("Usage %s <fisier_intrare>\n", argv[0]);
        exit(1);
    }
    int input_fd = open_input_file(argv[1]);
    int statistica_fd = create_stat_file();
    

    //variabilele pentru statistici
    int width, height, size;
    struct stat file_info;
    if (fstat(input_fd, &file_info) == -1) 
    {
        perror("Eroare la obtinerea informatiilor despre fisier");
        exit(1);
    }

    read_bmp_info(input_fd, &width, &height, &size);
    write_statistics(statistica_fd, argv[1], width, height, size, &file_info);

    close_file(input_fd);
    close_file(statistica_fd);

    return 0;
}