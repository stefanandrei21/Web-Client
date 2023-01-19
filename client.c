#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

#define THE_HOST "34.241.4.235:8080"
#define REGISTER "/api/v1/tema/auth/register"
#define LOGIN "/api/v1/tema/auth/login"
#define ACCESS "/api/v1/tema/library/access"
#define BOOKS "/api/v1/tema/library/books"
#define LOGOUT "/api/v1/tema/auth/logout"
#define JSON "application/json"
#define MY_IP "34.241.4.235"
#define MY_PORT 8080
int main(int argc, char *argv[])
{
    char *message;
    char *response;
    char cookie[201];
    int log_access = 0;
    char jwt[301];
    int jwt_access = 0;
    char *my_error = malloc(500);

    while(1) {
        printf("INSERT COMMAND:register, login, access_library, all_books, book, add_book, delete_book, logout\n");
        char my_comm[50];
        scanf("%s", my_comm);
        if (strcmp(my_comm, "register") == 0) {
            int sockfd = open_connection(MY_IP, MY_PORT, AF_INET, SOCK_STREAM, 0);
            char user[101], pass[101];
            printf("username:");
            scanf("%s", user);
            printf("password:");
            scanf("%s", pass);
            // Creez un json de genul {"username":"andsadasda","password":"sdadas"} si il trimit cu un post request

            char my_content[1000];
            strcpy(my_content,"{\"username\":\"");
            strcat(my_content, user);
            strcat(my_content, "\",");
            strcat(my_content,"\"password\":\"");
            strcat(my_content, pass);
            strcat(my_content,"\"}");
        
            message = compute_post_request(THE_HOST, REGISTER , JSON, my_content, NULL, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
          
            my_error = strstr(response, "error");
            if(my_error != NULL) {
                printf("EROARE\n");
            } else {
                printf("Inregistrare realizata cu succes\n");
            }
            close_connection(sockfd);
        } else if (strcmp(my_comm, "login") == 0){
            int sockfd = open_connection(MY_IP, MY_PORT, AF_INET, SOCK_STREAM, 0);

            //citesc de la tastatura username si parola {"username":"student","password":"student"}
            char user[101], pass[101];
            printf("username:");
            scanf("%s", user);
            printf("password:");
            scanf("%s", pass);

            //creez mesajul json
            char my_content[1000];
            strcpy(my_content,"{\"username\":\"");
            strcat(my_content, user);
            strcat(my_content, "\",");
            strcat(my_content,"\"password\":\"");
            strcat(my_content, pass);
            strcat(my_content,"\"}");
            message = compute_post_request(THE_HOST, LOGIN,  JSON, my_content, NULL, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            
            //tratez erorile si daca nu sunt iau cookieul pentru a demonstra ca sunt logat ulterior.
            my_error = strstr(response, "error");
            if(my_error != NULL) {
                printf("EROARE\n");
            } else {
                strcpy(cookie, strstr(response, "connect.sid"));
                sscanf(cookie,"%[^;]" ,cookie);
                log_access = 1;
                printf("Logare realizata cu succes\n");
            }
           
            close_connection(sockfd);
        } else if (strcmp(my_comm, "access_library") == 0) {
            //daca nu sunt logat trec mai departe
            if(log_access == 0){
                printf("Logare esuata incearca din nou\n");
                continue;
            }
            int sockfd = open_connection(MY_IP, MY_PORT, AF_INET, SOCK_STREAM, 0);
        
            
            message =  compute_get_request(THE_HOST, ACCESS, NULL, cookie);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            //tratez eroare, iar daca intoarce mesajul corespunzator parsez tokenul jwt
            my_error = strstr(response, "error");
            if(my_error != NULL) {
                printf("EROARE\n");
            } else {
                strcpy(jwt, strstr(response,"\":\""));
                sscanf(jwt+3, "%[^:}]", jwt);
                int length = strlen(jwt);
                *(jwt + length - 1) = '\0';  
                jwt_access = 1;

                printf("Am primit acces la librarie\n");
            }
            
            close_connection(sockfd);
        } else if (strcmp(my_comm, "all_books") == 0) {
            //daca nu sunt logat sau nu am acces la librarie iese
            if(log_access == 0 || jwt_access == 0){
                printf("Token ul de access in librarie nu este bun\n");
                continue;
            }
            int sockfd = open_connection(MY_IP, MY_PORT, AF_INET, SOCK_STREAM, 0);
        
            //trimit get requestul 
            message =  compute_get_request(THE_HOST, BOOKS, jwt, cookie);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            //verific daca am erori daca nu am parsez raspunsul si afisez
            my_error = strstr(response, "error");
            if(my_error != NULL) {
                printf("EROARE\n");
            } else {
                my_error = strstr(response, "[{");
                printf("%s\n", my_error);
            }
            my_error = NULL;
            close_connection(sockfd);
        }
        else if (strcmp(my_comm, "book") == 0) {
             if(log_access == 0 || jwt_access == 0){
                printf("Token ul de access in librarie nu este bun\n");
                continue;
            }
            printf("BOOK ID:");
            char book_id[100];
            scanf("%s", book_id);
            int sockfd = open_connection(MY_IP, MY_PORT, AF_INET, SOCK_STREAM, 0);
            
            //trimit get requestul
            char my_url[200] =  BOOKS;
            strcat(my_url, "/");
            strcat(my_url, book_id);
            message =  compute_get_request(THE_HOST, my_url, jwt, cookie);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            //verific daca am erori daca nu am parsez raspunsul si afisez
            my_error = strstr(response, "error");
            if(my_error != NULL) {
                printf("EROARE\n");
            } else {
                my_error = strstr(response,"\"title\"");
                printf("%s\n", my_error);
            }
            my_error = NULL;
           
            
            close_connection(sockfd);
        } else if (strcmp(my_comm, "add_book") == 0) {
            if(log_access == 0 || jwt_access == 0){
                printf("Token ul de access in librarie nu este bun\n");
                continue;
            }
            int sockfd = open_connection(MY_IP, MY_PORT, AF_INET, SOCK_STREAM, 0);
            char my_title[200], autor[200], gen[200], editor[200], nr_p[200];
                
            printf("titlu:");
            scanf("%s", my_title);
            printf("autor:");
            scanf("%s", autor);
            printf("gen:");
            scanf("%s", gen);
            printf("editor:");
            scanf("%s", editor);
            printf("Numar de pagini:");
            scanf("%s", nr_p);
           
           //creez jsonul de forma: {"title":"titlu","author":"autor","genre":"gen","page_count":"numar","publisher":"publ"};
            char my_content[2000];
            strcpy(my_content,"{\"title\":\"");
            strcat(my_content, my_title);
            strcat(my_content, "\",");
            strcat(my_content,"\"author\":\"");
            strcat(my_content, autor);
            strcat(my_content, "\",");
            strcat(my_content,"\"genre\":\"");
            strcat(my_content, gen);
            strcat(my_content, "\",");
            strcat(my_content,"\"page_count\":\"");
            strcat(my_content, nr_p);
            strcat(my_content, "\",");
            strcat(my_content,"\"publisher\":\"");
            strcat(my_content, editor);
            strcat(my_content,"\"}");
            
            
           //trimit post requestul
            message = compute_post_request(THE_HOST, BOOKS, JSON, my_content, cookie, jwt);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            
            //tratez eroare
            my_error = strstr(response, "error");
            if(my_error != NULL) {
                printf("EROARE: date introduse incorect\n");
            } else {
                printf("Carte adaugata cu succes\n");
            }
       
            close_connection(sockfd);
        } else if (strcmp(my_comm, "delete_book") == 0) {
            //verific accesul la librarie si daca sunt logat
            if(log_access == 0 || jwt_access == 0){
                printf("Token ul de access in librarie nu este bun\n");
                continue;
            }
            char book_id[100];
            //citesc de la tastatura id-ul
            printf("BOOK ID:\n");
            scanf("%s", book_id);
            int sockfd = open_connection(MY_IP, MY_PORT, AF_INET, SOCK_STREAM, 0);
            
            char my_url[200] =  BOOKS;
            strcat(my_url, "/");
            strcat(my_url, book_id);
            
            //trimit delete requestul
            message =  compute_delete_request(THE_HOST, my_url ,jwt, cookie);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            my_error = strstr(response, "error");
            if(my_error != NULL) {
                printf("EROARE: id incorect\n");
            } else {
                printf("Cartea a fost stearsa corects\n");
            }
          
            
            close_connection(sockfd);
        } else if (strcmp(my_comm, "logout") == 0) {
            if(log_access == 0 ){
                printf("Ca sa te deloghezi trebuie sa fi logat\n");
                continue;
            }
            
            int sockfd = open_connection(MY_IP, MY_PORT, AF_INET, SOCK_STREAM, 0);
            
 
            message =  compute_get_request(THE_HOST, LOGOUT , jwt, cookie);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            my_error = strstr(response, "error");
                
            if(my_error != NULL) {
                printf("EROARE\n");
            } else {
                printf("DELOGAT\n");
            }

            jwt_access = 0;
            log_access = 0;

            close_connection(sockfd);
        } else if(strcmp(my_comm, "exit") == 0){
            free(my_error);
            return 0;
        }
       
    }
  
      

    return 0;
}
