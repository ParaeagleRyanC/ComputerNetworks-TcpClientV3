#include <stdio.h>

#include "log.h"
#include "tcp_client.h"

int requests_sent = 0;
int responses_received = 0;
int sockfd;

int handle_response(char *response) {
    log_debug("Got to complete message!");
    printf("%s\n", response);
    responses_received++;
    return (responses_received == requests_sent);
}

int main(int argc, char *argv[]) {
    log_set_level(LOG_ERROR);

    Config config;
    char *action;
    char *message;
    int line_length = 0;

    tcp_client_parse_arguments(argc, argv, &config);
    sockfd = tcp_client_connect(config);
    FILE *fd = tcp_client_open_file(config.file);

    while (line_length != -1) {
        line_length = tcp_client_get_line(fd, &action, &message);
        if (line_length == -1) break;
        tcp_client_send_request(sockfd, action, message);
        requests_sent++;
        free(action);
        free(message);
    }    

    tcp_client_close_file(fd);
    if (requests_sent != 0) {
        tcp_client_receive_response(sockfd, &handle_response);  
    }
    tcp_client_close(sockfd);
    return EXIT_SUCCESS;
}
