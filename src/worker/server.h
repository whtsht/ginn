#ifndef SERVER_H
#define SERVER_H

int server_socket(const char* portnm);
void accept_loop(int soc);

#endif
