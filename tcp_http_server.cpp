/*************************************************************************
    > File Name: tcp_http_server.cpp
    > Author: nian
    > Blog: https://whoisnian.com
    > Mail: zhuchangbao2017@gmail.com
    > Created Time: 2017年08月24日 星期四 23时36分28秒
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

//设置服务器监听端口号
const unsigned long port = 80;

int main(void)
{
	//创建服务器端socket，地址族为AF_INET(IPv4)，传输方式为TCP
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//初始化监听IP为本地所有IP，端口为已设置的port
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
	
	//绑定socket与IP和端口
	bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

	//使socket进入监听模式
	listen(server_socket, 10);
	
	while(1)
	{
		//接收客户端请求
		int client_socket;
		struct sockaddr_in client_addr;
		socklen_t client_addr_len;
		client_addr_len = sizeof(client_addr);
		client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);

		//获取浏览器发送的请求
		char request[1000];
		read(client_socket, request, sizeof(request));
		printf("%s\n", inet_ntoa(client_addr.sin_addr));

		//获取当前时间
		time_t Time;
		char time_of_now[50];
		time(&Time);
		strftime(time_of_now, 50, "%a, %d %b %Y %H:%I:%S %Z", gmtime(&Time));
		
		char address[500];
		sscanf(request, "%*s%s", address);
		//如果不是请求”/“或”/index.html“，返回404
		if(strcmp(address, "/")&&strcmp(address, "/index.html"))
		{
			//初始化响应
			char response[100];
			memset(response, 0, sizeof(response));

			//设置响应头
			sprintf(response, "HTTP/1.1 404 Not Found\nDate: %s\nServer: C++ Server\n\n", time_of_now);

			//返回服务器响应
			write(client_socket, response, strlen(response));
		}
		//否则返回当前目录下index.html的内容
		else
		{
			//初始化响应
			char response[100000];
			char html[90000];
			memset(response, 0, sizeof(response));
			memset(html, 0, sizeof(html));

			//设置响应头
			sprintf(response, "HTTP/1.1 200 OK\nDate: %s\nServer: C++ Server\n\n", time_of_now);
		
			//设置返回的内容
			int fp = open("./index.html", O_RDONLY);
			read(fp, html, sizeof(html));
			close(fp);
			strcat(response, html);
		
			//返回服务器响应
			write(client_socket, response, strlen(response));
		}
		close(client_socket);
	}
	close(server_socket);
	return 0;
}
