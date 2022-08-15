#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "parson.h"
#include "helpers.h"
#include "requests.h"

int main(int argc, char *argv[]) {
    char *message;
    char *response;
    int sockfd;
    
    char **cookies = (char **)calloc(2, sizeof(char *));
    if (cookies == NULL) error("Memory failure.");
    
    char *access_route = (char *)calloc(LINELEN, sizeof(char));
    if (access_route == NULL) error("Memory failure.");

    char command[20];
    while (1) {
        scanf("%s", command);
        memset(access_route, 0, LINELEN);

        // REGISTER
        if (!strcmp(command, "register")) {
            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            char *serialized_string = NULL;
            char input[50];
            
            printf("%s", "username=");
            scanf("%s", input);
            json_object_set_string(root_object, "username", input);
            
            printf("%s", "password=");
            scanf("%s", input);
            json_object_set_string(root_object, "password", input);

            serialized_string = json_serialize_to_string_pretty(root_value);

            sprintf(access_route, "/api/v1/tema/auth/register");
            sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_post_request("3.8.116.10", access_route, "application/json", &serialized_string, 1, NULL, 0);

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printf("\nresponse: %s\n", response);
            
            close_connection(sockfd);

            // Free memory
            json_free_serialized_string(serialized_string);
            json_value_free(root_value);
            free(response);
            free(message);
        }

        // LOGIN
        if (!strcmp(command, "login")) {
            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            char *serialized_string = NULL;
            char input[50];
            
            printf("%s", "username=");
            scanf("%s", input);
            json_object_set_string(root_object, "username", input);
            
            printf("%s", "password=");
            scanf("%s", input);
            json_object_set_string(root_object, "password", input);

            serialized_string = json_serialize_to_string_pretty(root_value);

            sprintf(access_route, "/api/v1/tema/auth/login");
            sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_post_request("3.8.116.10", access_route, "application/json", &serialized_string, 1, NULL, 0);

            send_to_server(sockfd, message);   
            response = receive_from_server(sockfd);
            printf("\nresponse: %s\n", response);

            // Get cookie 
            if (response[9] == '2') {
                char *tmp;
                tmp = strstr(response, "Set-Cookie:");
                tmp = strtok(tmp, " ");
                tmp = strtok(NULL, ";");
                free(cookies[0]);
                cookies[0] = (char *)calloc((strlen(tmp) + 1), sizeof(char));
                if (cookies[0] == NULL) error("Memory failure.");
                strcpy(cookies[0], tmp);
            }
            
            close_connection(sockfd);

            // Free memory
            json_free_serialized_string(serialized_string);
            json_value_free(root_value);
            free(response);
            free(message);
        }

        // ENTER LIBRARY
        if (!strcmp(command, "enter_library")) {
            sprintf(access_route, "/api/v1/tema/library/access");
            sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request("3.8.116.10", access_route, NULL, cookies, 1);

            send_to_server(sockfd, message);   
            response = receive_from_server(sockfd);
            printf("\nresponse: %s\n", response);

            // Get token
            char *tmp = NULL;
            if (response[9] == '2') {
                tmp = strstr(response, "{\"token\"");
                tmp = strtok(tmp, ": {\"}");
                tmp = strtok(NULL, ": {\"}");
                free(cookies[1]);
                cookies[1] = (char *)calloc((strlen(tmp) + 1), sizeof(char));
                if (cookies[1] == NULL) error("Memory failure.");
                strcpy(cookies[1], tmp);
            }
            
            close_connection(sockfd);
            free(response);
            free(message);
        }

        // GET BOOKS
        if (!strcmp(command, "get_books")) {
            sprintf(access_route, "/api/v1/tema/library/books");
            sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request("3.8.116.10", access_route, NULL, cookies, 2);

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printf("\nresponse: %s\n", response);

            close_connection(sockfd);

            // Free memory
            free(response);
            free(message);
        }

        // GET BOOK
        if (!strcmp(command, "get_book")) {
            int id;
            printf("id=");
            scanf("%d", &id);

            sprintf(access_route, "/api/v1/tema/library/books/%d", id);
            sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request("3.8.116.10", access_route, NULL, cookies, 2);    

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printf("\nresponse: %s\n", response);

            close_connection(sockfd);  

            // Free memory
            free(response);
            free(message);
        }

        // ADD BOOK
        if (!strcmp(command, "add_book")) {
            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            char *serialized_string = NULL;

            char input[200]; int page_count;

            printf("title=");
            scanf("%s", input);
            json_object_set_string(root_object, "title", input);
            printf("author=");
            scanf("%s", input);
            json_object_set_string(root_object, "author", input);
            printf("genre=");
            scanf("%s", input);
            json_object_set_string(root_object, "genre", input);
            printf("publisher=");
            scanf("%s", input);
            json_object_set_string(root_object, "publisher", input);
            printf("page_count=");
            scanf("%d", &page_count);
            json_object_set_number(root_object, "page_count", page_count);

            serialized_string = json_serialize_to_string_pretty(root_value);
            sprintf(access_route, "/api/v1/tema/library/books");

            sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_post_request("3.8.116.10", access_route, "application/json", &serialized_string, 1, cookies, 2);

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printf("\nresponse: %s\n", response);

            close_connection(sockfd);

            // Free memory
            json_free_serialized_string(serialized_string);
            json_value_free(root_value);
            free(response);
            free(message);
        }

        // DELETE BOOK
        if (!strcmp(command, "delete_book")) {
            int id;
            printf("id=");
            scanf("%d", &id);

            sprintf(access_route, "/api/v1/tema/library/books/%d", id);
            sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_delete_request("3.8.116.10", access_route, cookies, 2);    

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printf("\nresponse: %s\n", response);
            close_connection(sockfd); 

            // Free memory 
            free(response);
            free(message);
        }

        // LOGOUT
        if (!strcmp(command, "logout")) {
            sprintf(access_route, "/api/v1/tema/auth/logout");
            
            sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request("3.8.116.10", access_route, NULL, cookies, 1);    

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printf("\nresponse: %s\n", response);

            free(cookies[0]);
            free(cookies[1]);
            cookies[0] = NULL;
            cookies[1] = NULL;
            close_connection(sockfd); 

            // Free memory 
            free(response);
            free(message);
        }

        // EXIT
        if (!strcmp(command, "exit")) {
            // Free memory
            free(cookies[0]);
            free(cookies[1]);
            free(cookies);
            free(access_route);
            return 0;
        }
    }
    return 0;
}