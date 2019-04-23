#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include<Windows.h>
#include<iostream>
#include<WinSock2.h>
#include "SocketServerMain.h"
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
int RecvMessage(SOCKET sock)
{
	DataPackage recvData;
	int recvLen = recv(sock, (char*)&recvData, sizeof(DataPackage), 0);
	if (recvLen <= 0)
	{
		cout << "客户端已退出!" << endl;
		return -1;
	}
	else
	{
		cout << "收到客户端" << sock << "发来的数据" << endl;
		cout << "收到客户端发的数据长度:" << recvData.dataLen << endl;
		cout << "收到的消息类型:" << recvData.msType << endl;
		cout << "收到的用户名:" << recvData._name << endl;
		cout << "收到的密码:" << recvData._password << endl;
		switch (recvData.msType)
		{
		case Login:
			if (strcmp(recvData._name, "张三") == 0)
			{
				if (strcmp(recvData._password, "123456") == 0)
				{
					//send(clientSocket, "登录成功!", 128, 0);
					send(sock, "登录成功!", 128, 0);
					cout <<"向客户端"<<sock<< "发送登录成功消息!" << endl;
				}
				else
				{
					send(sock, "密码错误!", 128, 0);
					cout << "向客户端" << sock << "发送密码错误消息!" << endl;
				}
			}
			else
			{
				send(sock, "用户名错误", 128, 0);

				cout << "向客户端" << sock << "发送用户名错误消息!" << endl;
			}

			break;
		default:
			send(sock, "未知错误!", 128, 0);

			cout << "向客户端" << sock << "发送未知错误!消息!" << endl;
			break;
		}
	}
}

int main() {
	//1.Winsock服务的初始化 WSAStartup
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);//调用了windows的动态库，需加入库文件 ws2_32.lib  初始化Ws2_32.dll的函数
	//2.创建Socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);//创建Socket 参数1 socket
	if (SOCKET_ERROR == sock)
	{
		cout << "服务端创建Socket失败!" << endl;
	}
	else
	{
		cout << "服务端创建Socket成功!Socket:" << sock << endl;
	}
	//3.绑定用于接收客户端连接的网络端口
	sockaddr_in _sin = {};//指定地址
	_sin.sin_family = AF_INET;//地址采用的协议
	_sin.sin_port = htons(4567);//地址端口
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//地址

	if (SOCKET_ERROR == bind(sock, (sockaddr*)&_sin, sizeof(sockaddr_in))) {//绑定端口
		std::cout << "服务端绑定端口失败!" << endl;
	}
	else {
		std::cout << "服务端绑定端口成功!" << endl;
	}
	//4.监听网络端口
	if (SOCKET_ERROR == listen(sock, 5)) {
		std::cout << "服务端监听端口失败!" << endl;
	}
	else {
		std::cout << "服务端监听端口成功!" << endl;
	}
	

	timeval times = { 1,0 };
	SOCKET socks[4];
	socks[0] = sock;		//将监听Socket存入数组(服务端Socket)
	int totalSock = 1;


	while (true) {
		//cout << "Socket总数:" << totalSock << endl;
		fd_set readfds; FD_ZERO(&readfds);//可读集合
		fd_set writefds; FD_ZERO(&writefds);//可写集合
		fd_set exceptfds; FD_ZERO(&exceptfds);//例外集合

		for (int i = 0; i < totalSock; ++i)
		{
			FD_SET(socks[i], &readfds);//将监听Socket加入可读集合 (服务端Socket)
			FD_SET(socks[i], &writefds);//将监听Socket加入可写集合(服务端Socket)
			FD_SET(socks[i], &exceptfds);//将监听Socket加入异常集合(服务端Socket)
		}
		int ret = select(sock + 1, &readfds, &writefds, &exceptfds,&times);
		if (ret < 0)
		{
			cout << "select 返回值小于0 " << endl;

			break;
		}

		int m_totalSock = totalSock;
		for (int i = 0; i < m_totalSock; ++i)//遍历socks数组集合 
		{
			if (FD_ISSET(socks[i], &readfds))// 如果该sock在可读集合里面          
			{
				if (socks[i] == sock)//如果服务端sock在读集合里面 有新客户端连接到达
				{
					sockaddr_in clientAddr = {};//远程客户端地址
					int clientAddrLen = sizeof(sockaddr_in);
					SOCKET clientSocket = INVALID_SOCKET;
					clientSocket = accept(sock, (sockaddr*)&clientAddr, &clientAddrLen);//参数1 listen函数用到的socket,参数2 远程客户端地址 参数3 远程客户端长度 返回远程客户端Socket
					if (SOCKET_ERROR == clientSocket)
					{
						std::cout << "服务端接收客户端链接失败 ,无效的客户端Socket!" << endl;
						break;
					}
					else
					{
						std::cout << "服务端接收客户端链接成功成功!Socket" <<clientSocket<< endl;
						std::cout << "新客户端加入成功,IP地址为:!" << inet_ntoa(clientAddr.sin_addr) << endl;
					/*	char msbuff[128];
						recv(clientSocket, msbuff, 128, 0);
						cout << "收到客户端消息" << msbuff << endl;*/
						//send(clientSocket, "I'm server!", 128, 0);
						for (int i = 1; i < totalSock; i++)
						{
							send(socks[i], "新用户加入", 128, 0);
						}
						socks[totalSock++] = clientSocket;
						cout <<"总Socket数:"<< totalSock << endl;
					}
				}
				else{	//6.接收某个客户端消息
					
					//if (RecvMessage(socks[i]) == -1)
					//{
					//	/*for (int i = 0; i < totalSock; ++i)
					//	{

					//	}*/
					//	break;
					//}
				}
			}
		}	
	}

	//8.关闭Socket
	//closesocket(clientSocket);
	WSACleanup();//Winsock服务的注销 WSAStartup

	cout << "Hello Socket!" << endl;
	system("pause");
	return 0;
}


