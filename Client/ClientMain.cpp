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
#include<stdio.h>
using namespace std;
enum MessageType
{
	Login,
	Loginout
};
struct DataPackage
{
	short dataLen;
	MessageType msType;
	char _name[32];
	char _password[32];
};
void cmdThread(SOCKET s)
{
	while (true)
	{
		DataPackage recvdata;
		cout << "please enter username:" << endl;
		cin >> recvdata._name;
		cout << "please enter password:" << endl;
		cin >> recvdata._password;
		recvdata.dataLen = sizeof(DataPackage);
		recvdata.msType = Login;
		//4.向服务器发送消息
		if (SOCKET_ERROR == send(s, (const char*)&recvdata, sizeof(DataPackage), 0))
		{
			cout << "send message to server failed" << endl;
		}//根据输入项服务器发送消息
		else
		{
			cout << "send message to server success" << endl;
		}
	}
}
int main()
{
#ifdef _WIN32
	//1.Winsock服务的初始化 WSAStartup
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);
#endif // _WIN32


	//2.创建Socket
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == clientSocket)
	{
		cout << "Creat socket failed!" << endl;
	}
	else
	{
		cout << "Creat socket success!:" << clientSocket << endl;
	}
	//3.连接服务器服务器
	sockaddr_in serverAddr = {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(4567);
#ifdef _WIN32
	//serverAddr.sin_addr.S_un.S_addr = inet_addr("129.204.40.157");
	serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
#else
	serverAddr.sin_addr.s_addr = inet_addr("129.204.40.157");
#endif // _WIN32
	if (SOCKET_ERROR == connect(clientSocket, (sockaddr*)& serverAddr, sizeof(sockaddr)))
	{
		cout << "connect server failed" << endl;
	}
	else
	{
		cout << "connect server success!" << endl;

	}

	//char msBuff[128] = {};
	char msbuff2[128];
	//int recvlen = recv(clientSocket, msBuff, 128, 0);
	//cout << "服务器消息:" << msBuff << endl;
	thread t1(cmdThread, clientSocket);//启动一个线程
	while (true)
	{

		fd_set readfds; FD_ZERO(&readfds);
		timeval times = { 1,0 };
		FD_SET(clientSocket, &readfds);
		int ret = select(clientSocket + 1, &readfds, 0, 0, &times);
		if (ret < 0)
		{
			cout << "dis connect server" << endl;
			break;
		}
		if (FD_ISSET(clientSocket, &readfds))
		{
			//5.接收服务器信息		
			int recvlen = recv(clientSocket, msbuff2, 128, 0);
			cout << "return server len:" << recvlen << endl;
			if (recvlen > 0)
			{
				cout << "recv server message:" << msbuff2 << endl;
			}
			else
			{
				cout << "server no return" << endl;
			}
		}
		//cout << "主线程空闲时处理其他业务" << endl;
	}
#ifdef _WIN32
	//6.关闭Socket
	closesocket(clientSocket);
	//7.Winsock服务的注销 WSACleanup()
	WSACleanup();
#endif // _WIN32
	cout << "Hello Client!" << endl;
	system("pause");
	return EXIT_SUCCESS;
}