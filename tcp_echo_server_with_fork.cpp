/*************************************************************************
    > File Name: tcp_echo_server_with_fork.cpp
    > Author: nian
    > Blog: https://whoisnian.com
    > Mail: zhuchangbao2017@gmail.com
    > Created Time: 2017年08月28日 星期一 20时45分28秒
 ************************************************************************/
#include<cstdio>
#include<cstdlib>
#include<cstring>

//引入头文件
#include<sys/types.h> 
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

int main(int argc, char *argv[])
{
	int res;

	//检查是否提供了端口参数
	if(argc != 2)
	{
		printf("Invalid parameter\n");
		exit(1);
	}

	//检查端口是否合法
	const unsigned long PORT = atoi(argv[1]);
	if(PORT < 1||PORT > 65535)
	{
		printf("Invalid port\n");
		exit(1);
	}

	//创建服务器端socket，地址族为AF_INET(IPv4)，传输方式为TCP
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(server_socket < 0)
	{
		perror("Error when create server_socket ");
		exit(1);
	}
	
	//设置监听本地全部IP，端口为已设置的PORT
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");

	//绑定socket与IP和端口
	res = bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(res < 0)
	{
		perror("Error when bind address to socket ");
		exit(1);
	}

	//使socket进入监听模式
	listen(server_socket, 10);

	//接收到新连接时主进程创建子进程来处理连接，
	//然后主进程关闭自己接收到的客户端socket，
	//主进程继续等待新的连接，同时子进程接收内容，
	//接收完毕后子进程关闭，防止从子进程fork出新的进程
	while(1)
	{

		//等待接收客户端的请求
		int client_socket;
		struct sockaddr_in client_addr;
		socklen_t client_addr_len;
		client_addr_len = sizeof(client_addr);
		client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
		if(client_socket < 0)
		{
			perror("Error when accept client request ");
			exit(1);
		}

		//创建新的进程
		pid_t pid;
		pid = fork();
		if(pid < 0)
		{
			perror("Error when fork ");
			exit(1);
		}

		//获取客户端发送的内容，直到接收到“end”时停止
		while(pid == 0)
		{
			char buf[1000];
			memset(buf, 0, sizeof(buf));
			res = read(client_socket, buf, sizeof(buf));
			if(res < 0)
			{
				perror("Error when read from client ");
				exit(1);
			}

			//如果内容是“end”就停止，否则返回给客户端输入的内容
			if(buf[0] == 'e'&&buf[1] == 'n'&&buf[2] == 'd'&&buf[3] == '\n')
			{
				//子进程处理完自己接收到的连接后关闭
				close(client_socket);
				close(server_socket);
				exit(0);
			}
			else
			{
				res = write(client_socket, buf, sizeof(buf));
				if(res < 0)
				{
					perror("Error when write to client ");
					exit(1);
				}
			}
		}

		//关闭主进程中打开的客户端socket
		if(pid > 0) close(client_socket);
	}
	
	close(server_socket);
	return 0;
}
