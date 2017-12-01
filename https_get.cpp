/*************************************************************************
    > File Name: https_get.cpp
    > Author: nian
    > Blog: https://whoisnian.com
    > Mail: zhuchangbao2017@gmail.com
    > Created Time: 2017年12月01日 星期五 00时40分13秒
 ************************************************************************/
#include<cstdio>
#include<cstring>
#include<ctime>

//引入头文件
#include<sys/types.h> 
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

#include<sys/stat.h>
#include<fcntl.h>
#include<netdb.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

const char HOST[100] = "ipgw.tk";
const unsigned long PORT = 443;

int main(void)
{
    //获取主机名对应IP地址
    struct hostent *host;
    char ip[30];
    host = gethostbyname(HOST);
    inet_ntop(host->h_addrtype, *(host->h_addr_list), ip, sizeof(ip));
    printf("Connecting to %s(%s)...\n\n", HOST, ip);

    //创建服务器端socket，地址族为AF_INET(IPv4)，传输方式为TCP
    int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//初始化IP为输入的IP，端口为已设置的port
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr(ip);
		
	//客户端连接服务器
	connect(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

    //初始化SSL_CTX对象
    const SSL_METHOD *method = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);

    //加载SSL有关内容
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    //绑定SSL到socket
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, server_socket);
    SSL_connect(ssl);
        
    //发送GET请求
    char buf[1000];
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "GET / HTTP/1.1\r\nHost: %s\r\n\r\n", HOST);
    SSL_write(ssl, buf, strlen(buf));
    
    //获取服务器响应
    int len;
	do
    {
        len = SSL_read(ssl, buf, sizeof(buf));
        buf[len] = '\0';
        printf("%s", buf);
    }
    while(len);

    //关闭SSL和socket
    SSL_free(ssl);
    SSL_CTX_free(ctx);
	close(server_socket);
    return 0;
}
