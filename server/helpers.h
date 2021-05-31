#include "helpers.c"

void rm_lt_spaces(char *str);
int get_client_ip_port(char *str, char *client_ip, int *client_port);
int setup_data_connection(int *fd, char *client_ip, int client_port, int server_port);
int get_filename(char *input, char *fileptr);
int get_cmd_code(char *command);
int _ls(int controlfd, int datafd, char *input);
int _get(int controlfd, int datafd, char *input);