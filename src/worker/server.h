#ifndef SERVER_H
#define SERVER_H

#define MAX_MULTIPLICITY 100

int server_socket(const char* portnm);
void accept_loop(int soc);

#endif
