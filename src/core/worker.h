#ifndef SERVER_H
#define SERVER_H

int server_socket(const char* portnm);
int send_recv(int acc);
void accept_loop(int soc);
void shutdown_worker();

#endif
