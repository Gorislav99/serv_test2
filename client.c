#include "srv.h"

void	*get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char **argv)
{
	int 		sock_fd, numbytes;
	int		rv;
	char		buf[MAXDATASIZE];
	char		s[INET6_ADDRSTRLEN];
	struct addrinfo	hints, *servinfo, *p;
	
	if (argc != 2)
	{
		fprintf(stderr, "usage: client hostname\n");
		exit(1);
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// цикл по всем результатам и связывание с первым возможным 
	
	for (p = servinfo;p != NULL;p = p->ai_next)
	{
		if ((sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			perror("client: socket");
			continue;
		}
		if (connect(sock_fd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sock_fd);
			perror("client: connect");
			continue;
		}
		break;
	}

	if (p == NULL)
	{
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr*)p->ai_addr), s, sizeof(s));
	
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo);

	if ((numbytes = recv(sock_fd, buf, MAXDATASIZE - 1, 0)) == -1)
	{
		perror("recv");
		exit(1);
	}

	buf[numbytes] = '\0';
	printf("clinet: received %s\n", buf);
	close(sock_fd);
	return 0;
}
