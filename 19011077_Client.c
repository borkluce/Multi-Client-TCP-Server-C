#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <client_id>\n", argv[0]);
        return -1;
    }

    int client_id = atoi(argv[1]);
    char userIDString[5];
    sprintf(userIDString, "%d", client_id);

    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Creating socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }
    int cont = 1;
    while(cont==1){
        int choose;
        printf("Choose what you wanna do:\n1: Send a message\n2: Read the messages\n3: List the friends\n4: Add a new friend\n");
        scanf("%d",&choose);
        if(choose==1) {
            // Kullanıcıdan mesaj alıp sunucuya gönderme işlemi
            printf("Enter target user ID and message: ");
            int target_user_id;
            char message[BUFFER_SIZE];
            char combined_message[BUFFER_SIZE];

            if (scanf("%d %[^\n]", &target_user_id, message) != 2) {
                printf("Invalid input.\n");
                return 1;
            }

            sprintf(combined_message, "%d %s", target_user_id, message);

            printf("\ncombined message: %s", combined_message);

            char all_request[BUFFER_SIZE + 5];

            if (strlen(combined_message) + 1 > BUFFER_SIZE) {
                printf("Combined message too long.\n");
                return 1;
            }

            strcpy(all_request, "1 ");
            strcat(all_request, combined_message);

            send(sock, all_request, strlen(all_request), 0);

            // Sunucudan gelen cevabı okuma işlemi
            char server_response[BUFFER_SIZE];
            read(sock, server_response, BUFFER_SIZE);
            printf("\nServer: %s\n", server_response);
        }
        else if(choose==2){
            // Sunucudan gelen cevabı okuma işlemi
            read(sock, buffer, BUFFER_SIZE);
            printf("\nServer: %s\n", buffer);
        }
        else if(choose==3){
            char request[20];
            strcpy(request, "3 ");
            strcat(request, userIDString);
            send(sock, request, strlen(request), 0);
            //gelen listeyi okuma
            char server_response[100];
            read(sock, server_response, BUFFER_SIZE);
            printf("\nFriend List:\n %s\n", server_response);
        }
        else if(choose==4){
            int new_id;
            printf("ID of the person you wanna add as friend: ");
            scanf("%d",&new_id);
            char request[20];
            strcpy(request, "4 ");
            strcat(request, userIDString);
            strcat(request, " ");
            char new_id_string[5];
            sprintf(new_id_string, "%d", new_id);
            strcat(request, new_id_string);
            send(sock, request, strlen(request), 0);
            //islem tamam mesaji
            char server_response[100];
            read(sock, server_response, BUFFER_SIZE);
            printf("\nServer Response: %s\n", server_response);
        }
    }
    return 0;
}