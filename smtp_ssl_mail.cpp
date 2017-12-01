/*************************************************************************
    > File Name: smtp_ssl_mail.cpp
    > Author: nian
    > Blog: https://whoisnian.com
    > Mail: zhuchangbao2017@gmail.com
    > Created Time: 2017年12月01日 星期五 12时22分13秒
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

const char LOCALHOST[100]   = "localhost";          //本地主机名
const char HOST[100]        = "smtp.example.com";   //SMTP服务器地址
const unsigned long PORT    = 465;                  //SMTP服务器SSL端口
const char USERNAME[100]    = "me@example.com";     //用户名
const char PASSWORD[100]    = "password";           //密码
const char FROM[100]        = "me@exmple.com";      //发件人邮箱
const char MAILTO[100]      = "other@other.com";    //收件人邮箱
const char CHARSET[50]      = "UTF-8";              //邮件中文编码：UTF-8 GB2312
const char SUBJECT[100]     = "SMTP 测试";          //邮件主题
const char TYPE[50]         = "plain";              //邮件格式：html plain
const char CONTENT[1000]    = "你好！Just a test."; //邮件内容

char buf[1000];     //缓存区大小

//Base64 编码
void base64_encode(const char *ori, char *res)
{
    int len, k, i;
    k = len = strlen(ori);
    
    if(len % 3 != 0)
        k = (len / 3 + 1) * 3;

    for(i = 0;i * 3 < k;i++)
    {
        res[i*4] = (ori[i*3] & 0xfc) >> 2;
        res[i*4+1] = ((ori[i*3] & 0x03) << 4) + ((ori[i*3+1] & 0xf0) >> 4);
        res[i*4+2] = ((ori[i*3+1] & 0x0f) << 2) + ((ori[i*3+2] & 0xc0) >> 6);
        res[i*4+3] = ori[i*3+2] & 0x3f;
    }

    for(i = 0;i < (len + 2) / 3 * 4;i++)
    {
        if(i < (len + 2) / 3 + len)
        {
            if(res[i] >= 0&&res[i] <= 25)
                res[i] += 65;
            else if(res[i] >= 26&& res[i] <= 51)
                res[i] += 71;
            else if(res[i] >= 52&&res[i] <= 61)
                res[i] -= 4;
            else if(res[i] == 62)
                res[i] = 43;
            else if(res[i] == 63)
                res[i] = 47;
        }
        else
            res[i] = '=';
    }
    res[i] = '\0';
}

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
     
    int len;
    memset(buf, 0, sizeof(buf));

    //获取服务器状态
    len = SSL_read(ssl, buf, sizeof(buf));
    buf[len] = '\0';
    printf("%s", buf);  // 220

    //发送本地主机身份
    sprintf(buf, "EHLO %s\r\n", LOCALHOST);
    SSL_write(ssl, buf, strlen(buf));

    for(int i = 0;i < 3;i++)
    {
        len = SSL_read(ssl, buf, sizeof(buf));
        buf[len] = '\0';
        printf("%s", buf);  // 250
    }

    //发送登录请求
    strcpy(buf, "AUTH LOGIN\r\n");
    SSL_write(ssl, buf, strlen(buf));

    len = SSL_read(ssl, buf, sizeof(buf));
    buf[len] = '\0';
    printf("%s", buf);  // 334

    //发送Base64编码的用户名
    base64_encode(USERNAME, buf);
    strcat(buf, "\r\n");
    SSL_write(ssl, buf, strlen(buf));

    len = SSL_read(ssl, buf, sizeof(buf));
    buf[len] = '\0';
    printf("%s", buf);  // 334

    //发送Base64编码的密码
    base64_encode(PASSWORD, buf);
    strcat(buf, "\r\n");
    SSL_write(ssl, buf, strlen(buf));

    len = SSL_read(ssl, buf, sizeof(buf));
    buf[len] = '\0';
    printf("%s", buf);  // 235

    //发送发件人邮箱
    sprintf(buf, "MAIL FROM:<%s>\r\n", FROM);
    SSL_write(ssl, buf, strlen(buf));

    len = SSL_read(ssl, buf, sizeof(buf));
    buf[len] = '\0';
    printf("%s", buf);  // 250

    //发送收件人邮箱
    sprintf(buf, "RCPT TO:<%s>\r\n", MAILTO);
    SSL_write(ssl, buf, strlen(buf));

    len = SSL_read(ssl, buf, sizeof(buf));
    buf[len] = '\0';
    printf("%s", buf);  // 250

    //准备发送数据
    strcpy(buf, "DATA\r\n");
    SSL_write(ssl, buf, strlen(buf));

    len = SSL_read(ssl, buf, sizeof(buf));
    buf[len] = '\0';
    printf("%s", buf);  // 354

    //获取当前时间
    time_t Time;
    char time_of_now[50];
    time(&Time);
    strftime(time_of_now, 50, "%a, %d %b %Y %H:%I:%S %z", localtime(&Time));

    //对邮件主题进行编码
    char subject[200];
    base64_encode(SUBJECT, subject);
    
    //设置Content-Type
    char type[50];
    if(!strcmp(TYPE, "html"))
        strcpy(type, "text/html");
    else
        strcpy(type, "text/plain");

    //发送数据
    sprintf(buf, "Date: %s\r\nTo: %s\r\nFrom: %s\r\nSubject: =?%s?B?%s?=\r\nContent-Type: %s; charset=%s\r\n\r\n%s\r\n\r\n.\r\n", time_of_now, MAILTO, FROM, CHARSET, subject, type, CHARSET, CONTENT);
    SSL_write(ssl, buf, strlen(buf));

    len = SSL_read(ssl, buf, sizeof(buf));
    buf[len] = '\0';
    printf("%s", buf);  // 250

    //结束会话
    strcpy(buf, "QUIT\r\n");
    SSL_write(ssl, buf, strlen(buf));

    len = SSL_read(ssl, buf, sizeof(buf));
    buf[len] = '\0';
    printf("%s", buf);  // DONE

    //关闭SSL和socket
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(server_socket);
    return 0;
}
