# TrySocket
Try Socket.

### Environment
C++ language in Linux (use gcc 7.1.1).

### tcp_echo_server
`$ gcc tcp_echo_server.cpp -o tcp_echo_server`
`$ ./tcp_echo_server 7637`
(1 <= port number <= 65535, sudo when port number < 1024)

You can use `netcat` to test.
`$ nc locahost 7637`
Server process will finish when input "end".

