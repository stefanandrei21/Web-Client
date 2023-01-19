#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url,char*jwt,
                            char *cookies)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));


    sprintf(line, "GET %s HTTP/1.1", url);
    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    if(jwt!= NULL) {
         sprintf(line, "Authorization: Bearer %s", jwt);
    }
    compute_message(message, line);
    if (cookies != NULL) {
      
       sprintf(line, "Cookie: %s", cookies);
       compute_message(message,line);
    }
    
    compute_message(message, "");
    return message;
}

char *compute_delete_request(char *host, char *url, char*jwt, char *cookies)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    sprintf(line, "DELETE %s HTTP/1.1", url);

    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    if(jwt!= NULL) {
         sprintf(line, "Authorization: Bearer %s", jwt);
    }
    compute_message(message, line);
    if (cookies != NULL) {
       sprintf(line, "Cookie: %s", cookies);
       compute_message(message,line);
    }
    
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char *body_data, char *cookies,char *jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    

    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
 
    
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    if(jwt!= NULL) {
         sprintf(line, "Authorization: Bearer %s", jwt);
         compute_message(message, line);
    }
    
   
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    int contentLen = 0;

 
    contentLen = strlen(body_data);
    
    sprintf(line, "Content-Length: %d", contentLen);
    compute_message(message, line);

    if (cookies != NULL) {
      sprintf(line, "Cookie: %s", cookies);
       compute_message(message,line);
    }
    
    compute_message(message, "");
    
    sprintf(line, "%s", body_data);
    compute_message(message, body_data);

    free(line);
    return message;
}