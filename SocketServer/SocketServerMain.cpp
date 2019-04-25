#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#ifdef _WIN32
#include<Windows.h>
#include<WinSock2.h>
#else
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#endif // _WIN32
#include<iostream>
#include<thread>
#include<vector>
#include"Server.hpp"
//using namespace std;
using std::cout;
using std::endl;

int main()
{
	Server* s = new Server;
	s->InitServerSocket();
	s->BindServerPort("127.0.0.1", 4567);
	s->ListenPort();
	s->AcceptClient();
	while (s->IsRun())
	{
		s->OnRun();
	}
	s->CloseServerSocket();

	cout << "Hello Socket!" << endl;
	system("pause");
	return 0;
}


