#ifndef SRV_H
#define SRV_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "3501" // порт для подключения пользователей
#define BACKLOG 10 // размер очереди ожидающих подключений

#endif
