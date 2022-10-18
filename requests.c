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

char *compute_get_request(char *host, char *url, char *query_params,
                            char *cookies, char* token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Add url and query params if needed
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Add cookies if needed
    if (cookies != NULL) {
        sprintf(line, "Cookie: %s", cookies);
        compute_message(message, line);
    }

    // Add token JWT header if needed
    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // add final new line
    compute_message(message, "");
    free(line);
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char *body_data,
                            char *cookies, char* token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    // Add url to message
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    strcat(body_data_buffer, body_data);

    // add necessary headers (Content-Type and Content-Length)
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    sprintf(line, "Content-Length: %ld", strlen(body_data_buffer));
    compute_message(message, line);

    // Add cookies and jwt if needed
    if (cookies != NULL) {
      sprintf(line, "Cookie: %s", cookies);
      compute_message(message, line);
    }

    if (token != NULL) {
      sprintf(line, "Authorization: Bearer %s", token);
      compute_message(message, line);
    }

    // add new line at end of header
    compute_message(message, "");   

    // add the actual payload data
    memset(line, 0, LINELEN);
    compute_message(message, body_data_buffer);

    free(line);
    return message;
}

char* compute_delete_request(char *host, char *url,
                            char *cookies, char* token) {

    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (cookies != NULL) {
        sprintf(line, "Cookie: %s", cookies);
        compute_message(message, line);
    }

    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    compute_message(message, "");

    memset(line, 0, LINELEN);
    compute_message(message, body_data_buffer);

    free(line);
    return message;
}
