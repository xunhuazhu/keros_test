#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "keros.h"
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>


extern int h_errno;


static int uni_get_domain_ip(const char *domain, unsigned int *ip)
{
	char 	*ptr, **pptr;
	char 	str[INET_ADDRSTRLEN];
	int 	ipint = 0;

	if(domain == NULL || ip == NULL){
		return -1;
	}
	printf("domain: %s\r\n", domain);
	struct hostent *host = gethostbyname(domain);
	if(host == NULL){
		printf("get host error.\r\n");
		return -1;
	}

	if(host->h_addrtype != AF_INET){
		printf("host type[%s] error.", (host->h_addrtype == AF_INET) ? "AF_INET": "AF_INET6");
		return -1;
	}

#if 0
	pptr = host->h_addr_list;
	for (; *pptr != NULL; pptr++) {
		//printf("\taddress: %s\n", inet_ntop(host->h_addrtype, *pptr, str, sizeof(str)));

		memcpy(&ipint, *pptr, sizeof(unsigned int));
		printf("0x%x 0x%x 0x%x\r\n", ipint, ntohl(ipint), inet_addr("14.215.177.38"));
		//ipint = ntohl(ipint);
		printf("\taddress: %s\n", inet_ntoa(*((struct in_addr*)&ipint)));
	}
#endif
	pptr = host->h_addr_list;
	memcpy(ip, *pptr, sizeof(unsigned int));
	printf("--------->IP = 0x%x.\r\n", *ip);

	return 0;
}

static uni_socket_t uni_tcp_connect(unsigned int ip, unsigned short port, int recv_timeout, int send_timeout)
{
	int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("Create Socket Fail.\n");
        return -1;
    }

    struct in_addr addr;
    memcpy(&addr, &ip, 4);
    
	printf("[%s %d]ip:%s(0x%x) port:%d\r\n", __func__, __LINE__, inet_ntoa(addr), ip, port);
	
	struct sockaddr_in server_addr;
	server_addr.sin_addr.s_addr = ip;
	server_addr.sin_port = htons(port);
	server_addr.sin_family = AF_INET;

    struct timeval tv;
    memset(&tv, 0, sizeof( struct timeval ));
    tv.tv_sec = recv_timeout/1000;
    tv.tv_usec = recv_timeout%1000*1000;
    int ret = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	if(ret){
		printf( "set socket recv timeout failed %d\n",ret);
	}
    tv.tv_sec = send_timeout/1000;
    tv.tv_usec = send_timeout%1000*1000;
    ret = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	if(ret){
		printf("set socket snd timeout failed %d\n",ret);
	}

    int rc = connect(sock,
            (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in));
    if (rc != 0) {
        printf("[%s %d]connect server error:%d\r\n", __func__, __LINE__, rc);
        close(sock);
		return -1;
    }
    printf("[%s] connect ok.\r\n", __func__);
	return sock;
}

static int uni_tcp_destroy(uni_socket_t socket)
{
	return close(socket);
}

static int uni_tcp_send(uni_socket_t socket, char *buf, int len, int timeout_ms)
{
	return send(socket,buf,len,0);
}

static int uni_tcp_recv(uni_socket_t socket, char *buf, int len, int timeout_ms)
{
	struct timeval timeout;
	fd_set read_fds;
	
	timeout.tv_sec = timeout_ms/1000;
	timeout.tv_usec = timeout_ms%1000*1000;
	FD_ZERO(&read_fds);
	FD_SET(socket, &read_fds);
	int ret = select(socket + 1, &read_fds, NULL, NULL, &timeout);
	if(ret > 0){
		if(FD_ISSET(socket, &read_fds)){
			ret = recv(socket,buf,len,0);
			return ret;
		}
		return 0;
	}else {
		return -1;
	}
}

unsigned short uni_htons(unsigned short n)
{
	return htons(n);
}

unsigned int uni_htonl(unsigned int n)
{
	return htonl(n);
}

unsigned int uni_ntohl(unsigned int n)
{
	return ntohl(n);
}

int uni_socket_register(void)
{
	struct uni_socket_opt opt;
	opt.uni_get_domain_ip = uni_get_domain_ip;
	opt.uni_tcp_connect = uni_tcp_connect;
	opt.uni_tcp_destroy = uni_tcp_destroy;
	opt.uni_tcp_send = uni_tcp_send;
	opt.uni_tcp_recv = uni_tcp_recv;
	opt.uni_ntohl = uni_ntohl;
	opt.uni_htonl = uni_htonl;
	opt.uni_htons = uni_htons;

	uni_tcp_opt_register(&opt);

	return 0;
}
