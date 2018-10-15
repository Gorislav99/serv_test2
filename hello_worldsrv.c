#include "srv.h"

void sigchld_handler(int s)
{
	while (waitpid(-1, NULL, WNOHANG) > 0);
}

void	*get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int	main(void)
{
	int			sock_fd, new_fd; // слушать на sock_fd, новое подключение на new_fd
	struct addrinfo		hints, *servinfo, *p; 
	struct sockaddr_storage	their_addr; // адресная информация подключившегося 
	struct sigaction	sa;
	socklen_t		sin_size; // размер struct sockaddr_storage
	char 			s[INET6_ADDRSTRLEN];
	char 			yes = '1';
	int			rv;

	memset(&hints, 0, sizeof(hints)); // заполняет нулями sizeof(hints) байт
	hints.ai_family = AF_UNSPEC; // Ipv4 or Ipv6
	hints.ai_socktype = SOCK_STREAM; // потоковый сокет 
	hints.ai_flags = AI_PASSIVE; // испольщовать мой IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	for (p = servinfo;p != NULL;p = p->ai_next)
	{
		if ((sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			perror("server: socket");
			continue;
		}
		
		if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
			perror("setsockop");
			exit(1);
		}
		
		if (bind(sock_fd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close (sock_fd);
			perror("server: bind");
			continue;
		}

		break;
	}
	
	if (p == NULL)
	{
		fprintf(stderr, "server: failed to binf\n");
		return 2;
	}
	
	freeaddrinfo(servinfo);
	
	if (listen(sock_fd, BACKLOG) == -1)
	{
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // убираем мертвые процессы
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connection..\n");
	
	while(1)
	{
		sin_size = sizeof(their_addr);
		new_fd = accept(sock_fd, (struct sockaddr*)&their_addr, &sin_size);
		
		if (new_fd == -1)
		{
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr*)&their_addr), s, sizeof(s));

		printf("server: got connection from %s\n", s);
		
		if (!fork)
		{
			close(sock_fd);
			
			if (send(new_fd, "Hello World!", 13, 0) == -1)
			{
				perror("send");
				close(new_fd);
					exit(0);
			}
		}
		close(new_fd);
	}
	return 0;
}
