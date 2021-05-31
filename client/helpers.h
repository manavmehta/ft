#include "helpers.c"

void rm_lt_spaces(char *str);
long int findSize(const char *file_name);
int get_prompt(char *buffer);
int get_port_str(char *str, char *ip, int n5, int n6);
int check_command(char *command);
int get_cmd_code(char *command);
int convert(uint16_t port, int *n5, int *n6);
int get_ip_port(int fd, char *ip, int *port);
int get_filename(char *input, char *fileptr);
int _ls(int sockfd, int datafd, char *input);
int _history(int sockfd, int datafd, char *input);
int _get(int sockfd, int datafd, char *input);