/*************************************************************************
    > File Name: http_get.cpp
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

const char HOST[50] = "baidu.whoisnian.com";
const unsigned long PORT = 80;

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

	//初始化IP为得到的IP，端口为已设置的port
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr(ip);
		
	//连接服务器
	connect(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

	//发送GET请求
	char buf[1000];
	memset(buf, 0, sizeof(buf));
    sprintf(buf, "GET / HTTP/1.1\r\nHost: %s\r\n\r\n", HOST);
    write(server_socket, buf, sizeof(buf));

    //获取服务器响应
    int len;
	do
    {
        len = read(server_socket, buf, sizeof(buf));
        buf[len] = '\0';
        printf("%s", buf);
    }
    while(len);

    //关闭socket
	close(server_socket);
	return 0;
}
