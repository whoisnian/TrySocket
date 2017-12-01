# TrySocket
Try Socket.

### Environment
* C++ language in Linux (I use gcc 7.2.0).

### tcp_echo_server
* `$ g++ tcp_echo_server.cpp -o tcp_echo_server`
* `$ ./tcp_echo_server 7637`
  (1 <= port number <= 65535, sudo when port number < 1024)

* You can use `netcat` to test.
  `$ nc locahost 7637`
* Server and client process will finish when input "end".

### tcp_echo_server_with_fork
* `$ g++ tcp_echo_server_with_fork.cpp -o tcp_echo_server_with_fork`
* `$ ./tcp_echo_server_with_fork 7637`
  (1 <= port number <= 65535, sudo when port number < 1024)

* You can use `netcat` to test. 
  `$ nc locahost 7637`
* Client process will finish when input "end".
* Server process can be finished with Ctrl+C.
* Multiple connections are supported by create multiple processes.

### tcp_http_server
* `$ g++ tcp_http_server.cpp -o tcp_http_server`  
* `$ vim index.html`  
  ```
  <html>
  <head>
	<title>
	  Permission denied
	</title>
  </head>

  <body>
	<br/><br/>
	<h1 style="font-family:verdana;font-size:10em;text-align:center;color:#303F9F">
	  Permission denied
	</h1>
  </body>
  </html>
  ```
* `$ sudo ./tcp_http_server`
* Visit [http://localhost](http://localhost) with browser to get the content of index.html.  

### http_get
* Set a hostname.  
* `$ g++ http_get.cpp -o http_get`  
* `$ ./http_get`  

### https_get
* Set a hostname.  
* `$ g++ https_get.cpp -o https_get`  
* `$ ./https_get` 
