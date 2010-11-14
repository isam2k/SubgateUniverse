#define MAX_QUEUE 5
#define DEF_PORT_NUM "6249"

void fnShowUsage(char *name, char *msg)
{
	printf("%s:\n", msg);
	printf("USAGE:\n");
	printf("\t%s [-l logfile] [-m maxconn]\n", name);
	printf("OPTIONS:\n");
	printf("\t-l [logfile]\twrite log to logfile\n");
	printf("\t-m [maxconn]\tlimit number of connected clients\n");
} // fnShowUsage

int fnInitSocket(int argc, char *argv[])
{
	int					sockfd, state;
	struct addrinfo		hints, *serv_addr;
	
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	
	if ((state = getaddrinfo(NULL, DEF_PORT_NUM, &hints, &serv_addr)) != 0)
		return -1;
	
	sockfd = socket(serv_addr->ai_family, serv_addr->ai_socktype, serv_addr->ai_protocol);
	
	if (bind(sockfd, serv_addr->ai_addr, serv_addr->ai_addrlen) < 0)
	{
		freeaddrinfo(serv_addr);
		close(sockfd);
		return -1;
	}
	
	freeaddrinfo(serv_addr);
	return sockfd;
} // fnInitSocket
