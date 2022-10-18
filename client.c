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
#include "parson.h"

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd, id, run_server = 1;
    char* cookie = calloc(LINELEN, sizeof(char));
    char* token = calloc(LINELEN, sizeof(char));
    char* host = calloc(LINELEN, sizeof(char));
    char* command = calloc(LINELEN, sizeof(char));
    char* username = calloc(LINELEN, sizeof(char));
    char* password = calloc(LINELEN, sizeof(char));
    strcpy(host, "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com");

    char title[LINELEN];
    char author[LINELEN];
    char genre[LINELEN];
    char publisher[LINELEN];
    int page_count;

    while (run_server) {

        fscanf(stdin, "%s", command);

        if (strcmp(command, "register") == 0) {

            fprintf(stdout, "username=");
            fscanf(stdin, "%s", username);

            fprintf(stdout, "password=");
            fscanf(stdin, "%s", password);

            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            char* serialized_string = NULL;
            json_object_set_string(root_object, "username", username);
            json_object_set_string(root_object, "password", password);
            serialized_string = json_serialize_to_string_pretty(root_value);

            sockfd = open_connection("34.241.4.235", PORT, AF_INET, SOCK_STREAM, 0);
            message = compute_post_request(host, "/api/v1/tema/auth/register", "application/json", serialized_string, NULL, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

    		if(strstr(response, "is taken"))
    			fprintf(stdout, "Din pacate nu puteti folosi acest username. Este deja folosit :(\n");
    		else
    			fprintf(stdout, "Felicitari! V-ati inregistrat cu succes :)\n");


        } else if (strcmp(command, "login") == 0) {

            fprintf(stdout, "username=");
            fscanf(stdin, "%s", username);

            fprintf(stdout, "password=");
            fscanf(stdin, "%s", password);

            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            char* serialized_string = NULL;
            json_object_set_string(root_object, "username", username);
            json_object_set_string(root_object, "password", password);
            serialized_string = json_serialize_to_string_pretty(root_value);

            sockfd = open_connection("34.241.4.235", PORT, AF_INET, SOCK_STREAM, 0);
            message = compute_post_request(host, "/api/v1/tema/auth/login", "application/json", serialized_string, NULL, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

    		if(strstr(response, "No account with this username!")) {
    			fprintf(stdout, "Ups! Nu exista niciun cont cu acest username :(\n");
    		} else if (strstr(response, "Credentials are not good")){
    			fprintf(stdout, "Ups! Ati introdus parola gresit :(. Incercati din nou.\n");
    		} else {
    			fprintf(stdout, "Felicitari! V-ati logat cu succes :)\n");
                token = strtok(response, "\n");
                while (token) {
                    if (strstr(token, "connect.sid") != NULL) {
                        cookie = strtok(token, " ;");
                        cookie = strtok(NULL, ";");
                        fprintf(stdout, "Cookie-ul este: %s\n", cookie);
                    }
                    token = strtok(NULL, "\n");
                }
    		}
            
        } else if (strcmp(command, "enter_library") == 0) {

            sockfd = open_connection("34.241.4.235", PORT, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request(host, "/api/v1/tema/library/access", NULL, cookie, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            if(strstr(response, "Unauthorized")) {
  				fprintf(stdout, "Ups! Nu aveti acces la biblioteca :(\n");
  			} else {
  				fprintf(stdout, "Felicitari! Aveti acces in biblioteca :)\n");
  				response = strstr(response, "{\"");
  				token = strstr(response,":\"");
  				token = strstr(token,"\"");
    			token = strtok(token, "\"");

                fprintf(stdout, "Token-ul JWT este: %s\n", token);
    		}

        } else if (strcmp(command, "get_books") == 0) {

            if (token == NULL) {
                fprintf(stdout, "Ups! Nu aveti acces la biblioteca :(\n");
            } else {
                sockfd = open_connection("34.241.4.235", PORT, AF_INET, SOCK_STREAM, 0);
                message = compute_get_request(host, "/api/v1/tema/library/books", NULL, cookie, token);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);

                if(strstr(response, "Error when decoding token")){
                    fprintf(stdout, "Ups! Nu aveti acces la biblioteca :(\n");
                } else {
                    response = strstr(response, "[");
                    fprintf(stdout, "%s\n", response);
                }
            }

        } else if (strcmp(command, "get_book") == 0) {

    		if(token == NULL){
    			fprintf(stdout, "Ups! Nu aveti acces la biblioteca :(\n");
    		} else {

                fprintf(stdout, "id=");
                fscanf(stdin, "%d", &id);

                sockfd = open_connection("34.241.4.235", PORT, AF_INET, SOCK_STREAM, 0);
                char url[100];
                sprintf(url, "/api/v1/tema/library/books/%d", id);

                message = compute_get_request(host, url, NULL, cookie, token);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);

                if (strstr(response, "No book")) {
                    fprintf(stdout, "Ups! Aceasta carte nu exista :(\n");
                }else{
                    response = strstr(response, "{\"");
                    response = strstr(response,"\"");
                    response = strtok(response, "]");
                    fprintf(stdout, "{\"id:\"%d\",", id);
                    fprintf(stdout, "%s\n", response);
                }
            }

        } else if (strcmp(command, "add_book") == 0) {

            if(token == NULL){
    			fprintf(stdout, "Ups! Nu aveti acces la biblioteca :(\n");
    		} else {
                fprintf(stdout, "title=");
                getchar();
                scanf("%[^\n]", title);

                fprintf(stdout, "author=");
                getchar();
                scanf("%[^\n]", author);

                fprintf(stdout, "genre=");
                getchar();
                scanf("%[^\n]", genre);

                fprintf(stdout, "publisher=");
                getchar();
                scanf("%[^\n]", publisher);

                fprintf(stdout, "page_count=");
                getchar();
                fscanf(stdin, "%d", &page_count);

                if (page_count > 0) {
                    JSON_Value *root_value = json_value_init_object();
                    JSON_Object *root_object = json_value_get_object(root_value);
                    char *serialized_string = NULL;
                    json_object_set_string(root_object, "title", title);
                    json_object_set_string(root_object, "author", author);
                    json_object_set_string(root_object, "genre", genre);
                    json_object_set_number(root_object, "page_count", page_count);
                    json_object_set_string(root_object, "publisher", publisher);
                    serialized_string = json_serialize_to_string_pretty(root_value);

                    sockfd = open_connection("34.241.4.235", PORT, AF_INET, SOCK_STREAM, 0);
                    message = compute_post_request(host, "/api/v1/tema/library/books", "application/json", serialized_string, cookie, token);
                    send_to_server(sockfd, message);
                    response = receive_from_server(sockfd);

                    fprintf(stdout, "Yeey! S-a adaugat cartea. :)\n");
                } else {
                    fprintf(stdout, "Ups! Datele introduse sunt incorecte. Nu se poate adauga cartea. :(\n");
                }
            }

        } else if (strcmp(command, "delete_book") == 0) {
            
            if(token == NULL){
    			fprintf(stdout, "Ups! Nu aveti acces la biblioteca :(\n");
    		} else {

                fprintf(stdout, "id=");
                fscanf(stdin, "%d", &id);

                sockfd = open_connection("34.241.4.235", PORT, AF_INET, SOCK_STREAM, 0);
                char url[100];
                sprintf(url, "/api/v1/tema/library/books/%d", id);

                message = compute_delete_request(host, url, cookie, token);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);

	   			if(strstr(response, "No book")){
	   				fprintf(stdout, "Ups! Aceasta carte nu exista :(\n");
	   			}else{
	  				fprintf(stdout, "Felicitari! Cartea dorita a fost stearsa. :)\n");
	  			}
            }
            
        } else if (strcmp(command, "logout") == 0) {

            if (cookie == NULL) {
                fprintf(stdout, "Ups! Nu sunteti autentificat :(\n");
            } else {
                sockfd = open_connection("34.241.4.235", PORT, AF_INET, SOCK_STREAM, 0);
                message = compute_get_request(host, "/api/v1/tema/auth/logout", NULL, cookie, NULL);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);

                if(strstr(response, "You are not logged in"))
                    fprintf(stdout, "Ups! Din pacate nu sunteti logat. :(\n");
                else
                    fprintf(stdout, "Ati dat logout cu succes. :)\n");

                cookie = NULL;
                token = NULL;
            }

        } else if (strcmp(command, "exit") == 0) {
            fprintf(stdout, "Conexiunea a fost inchisa. \n");
            break;
        } else {
    		fprintf(stdout, "Comanda dumneavoastra nu este corecta. Incercati alta. :)\n");
        }
        fprintf(stdout, "\n");
    }
    
    free(cookie);
    free(token);
    free(host);
    free(command);
    free(username);
    free(password);

    return 0;
}
