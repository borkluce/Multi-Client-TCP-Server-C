#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CLIENTS 30
#define BUFFER_SIZE 1024
#define FILENAME "users.txt"

typedef struct {
    int id;
    char username[50];
    char number[20];
    // Diğer kullanıcı bilgileri eklenebilir
} User;

void saveUsersToFile(User users[], int userCount) {
    FILE *file = fopen(FILENAME, "w");
    int i;
    if (file == NULL) {
        printf("Dosya acilamadi.\n");
        return;
    }

    for (i = 0; i < userCount; i++) {
        fprintf(file, "%d %s %s\n", users[i].id, users[i].username, users[i].number);
    }

    fclose(file);
}

void addUser(User users[],int userCount, int id, char *name, char *number){
    char subFolderName[5];
    char subFolderPath[50];
    users[userCount].id = id;
    strcpy(users[userCount].username, name);
    strcpy(users[userCount].number, number);

    snprintf(subFolderName, sizeof(subFolderName), "%d", users[userCount].id);
    strcpy(subFolderPath,"users/");
    strcat(subFolderPath, subFolderName);
    mkdir(subFolderPath, 0777);
    char friends_path[50];
    snprintf(friends_path, sizeof(friends_path), "%s/friends.txt", subFolderPath);
    FILE *dosya = fopen(friends_path, "w");
        if (dosya != NULL) {
            fclose(dosya);
            printf("'%s' file is created.\n", friends_path);
        } else {
            printf("Dosya olusturma basarisiz!\n");
        }
}

int main() {
    int i,j;
    int server_fd, client_sockets[MAX_CLIENTS];
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Initialize address structure
    memset(&address, '0', sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Binding the socket to the specified port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }
    
    // Listening for clients
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Server is running...\n");

    // Kullanicilar klasoru

    char* klasor_adi = "users";

    if (mkdir(klasor_adi, 0777) == 0) {
        printf("users folder is created :)\n");
    } else {
        printf("There is a problem with creating folder :(\n");
    }
    
    User users[MAX_CLIENTS];
    int userCount = 0;

    // Örnek kullanıcı ekleme
    addUser(users,userCount,1,"ahmet","5553335");
    userCount++;
    addUser(users,userCount,2,"enis","1212121");
    userCount++;
    addUser(users,userCount,3,"simsir","77777");
    userCount++;

    // Kullanıcıları dosyaya kaydetme işlemi
    saveUsersToFile(users, userCount);

    printf("The users are saved to users.txt file.\n");

    int max_clients = MAX_CLIENTS;
    int activity, max_sd, new_socket, sd;
    fd_set readfds;
    char buffer[BUFFER_SIZE];

    for (i = 0; i < max_clients; i++) {
        client_sockets[i] = 0;
    }

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        for (i = 0; i < max_clients; i++) {
            sd = client_sockets[i];

            if (sd > 0)
                FD_SET(sd, &readfds);

            if (sd > max_sd)
                max_sd = sd;
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(server_fd, &readfds)) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
                perror("Acceptance failed");
                exit(EXIT_FAILURE);
            }

            printf("New client connected. Socket fd is %d, IP is : %s, Port : %d\n",
                   new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            for (i = 0; i < max_clients; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    break;
                }
            }
        }

        for (i = 0; i < max_clients; i++) {
            sd = client_sockets[i];

            if (FD_ISSET(sd, &readfds)) {
                if (read(sd, buffer, BUFFER_SIZE) == 0) {
                  //  printf("\nConnection problem!");
                } else {
                    int request_type;
                    char request[BUFFER_SIZE];
                    sscanf(buffer, "%d %[^\n]", &request_type, request);
                    if(request_type==1){
                        // Alınan mesajın hedef kullanıcı ID'sini ve içeriğini ayırma işlemi
                        int target_user_id;
                        char message[BUFFER_SIZE];
                        sscanf(request, "%d %[^\n]", &target_user_id, message);

                        // İlgili hedef kullanıcıya mesajı iletmek için döngü
                        for (j = 0; j < max_clients; j++) {
                            if (client_sockets[j] != 0 && j + 1 == target_user_id) {
                                send(client_sockets[j], message, strlen(message), 0);

                                // Mesaj gönderen istemciye bilgi gönderme
                                char delivery_message[BUFFER_SIZE];
                                sprintf(delivery_message, "Message delivered to user %d", target_user_id);
                                send(sd, delivery_message, strlen(delivery_message), 0);

                                break;
                            }
                        }
                    } else if(request_type==3){
                        char dosya_yolu[50] = "users/";
                        char friend_list[100];
                        dosya_yolu[6] = request[0];
                        strcat(dosya_yolu,"/friends.txt");
                        FILE *dosya = fopen(dosya_yolu, "r"); // Dosyayı okuma modunda açma

                        if (dosya != NULL) {
                            char satir[100]; // Okunacak satırı tutacak karakter dizisi
                            // Dosya sonuna kadar satır satır okuma ve ekrana yazdırma
                            while (fgets(satir, sizeof(satir), dosya) != NULL) {
                                strcat(friend_list,satir);
                                strcat(friend_list," ");
                            }

                            fclose(dosya); // Dosyayı kapatma
                            send(sd, friend_list, strlen(friend_list), 0);
                        } else {
                            printf("Dosya acilamadi!\n");
                        }
                    } else if(request_type==4){
                        char dosya_yolu[50] = "users/";
                        char friend_list[100];
                        dosya_yolu[6] = request[0];
                        strcat(dosya_yolu,"/friends.txt");

                        FILE *dosya;
                        dosya = fopen(dosya_yolu, "a"); // "a" modu dosyayı ekleme modunda açar

                        if (dosya == NULL) {
                            printf("Dosya acma hatasi!");
                            return 1; // Hata durumunda programdan çık
                        }
                        // Dosyaya yeni satırı ekle
                        fprintf(dosya, "%c\n", request[2]);

                        // Dosyayı kapat
                        fclose(dosya);
                        send(sd, "The user is added", strlen("Kullanici eklendi"), 0);
                    }
                }
            }
        }
    }

    return 0;
}
