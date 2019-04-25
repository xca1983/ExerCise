
#define _CRT_SECURE_NO_WARNINGS

#ifndef _SERVER_HPP_
#define _SERVER_HPP_
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
#include"Message.hpp"
//using namespace std;
using std::cout;
using std::endl;

class Server
{
public:
	Server()
	{
		m_serverSocket = INVALID_SOCKET;
	}
	~Server()
	{
		CloseServerSocket();
	}
	//初始化server Socket
	void InitServerSocket()
	{
#ifdef _WIN32
		WORD word = MAKEWORD(2, 2);
		WSADATA data;
		WSAStartup(word, &data);
#endif // _WIN32
		if (IsRun())
		{
			CloseServerSocket();
		}
		m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (IsRun())
		{
			cout << "创建server Socket成功!" << endl;
		}
		else
		{
			cout << "创建Server Socket失败!" << endl;
		}

	}
	//绑定端口
	void BindServerPort(const char * ip,short port)
	{
		if (!IsRun())
		{
			cout << "server socket 未创建!" << endl;
			CloseServerSocket();
			return;
		}
		sockaddr_in serveradd = {};
		serveradd.sin_family = AF_INET;
		serveradd.sin_port = htons(port);
#ifdef _WIN32
		serveradd.sin_addr.S_un.S_addr = inet_addr(ip);
#else
		serveradd.sin_addr.S_un.S_addr = inet_addr(ip);
#endif // _WIN32
		int ret = bind(m_serverSocket, (sockaddr*)&serveradd, sizeof(sockaddr));
		if (SOCKET_ERROR == ret)
		{
			cout << "server 绑定端口失败!" << endl;
		}
		else
		{
			cout << "Server 绑定端口成功!" << endl;
		}
	}
	//监听端口
	void ListenPort()
	{
		if (!IsRun()) 
		{
			cout << "server Socket未创建!" << endl;
		}
		int ret = listen(m_serverSocket, 5);
		if (SOCKET_ERROR == ret)
		{
			cout << "监听端口失败!" << endl;
		}
		else
		{
			cout << "监听端口成功!" << endl;
		}
	}
	//select模型OnRun
	void OnRun()
	{
		fd_set readfds; FD_ZERO(&readfds);
		fd_set writefds; FD_ZERO(&writefds);
		fd_set expfds; FD_ZERO(&expfds);
		FD_SET(m_serverSocket, &readfds);
		FD_SET(m_serverSocket, &writefds);
		FD_SET(m_serverSocket, &expfds);
		int maxfd = m_serverSocket;
		for (int i = 0; i < m_clientSockets.size(); ++i)
		{
			FD_SET(m_clientSockets[i], &readfds);
			FD_SET(m_clientSockets[i], &writefds);
			FD_SET(m_clientSockets[i], &expfds);
			if (m_clientSockets[i] > maxfd)
			{
				maxfd = m_clientSockets[i];
			}
		}
		timeval t = { 1,0 };
		int ret = select(maxfd + 1, &readfds, &writefds, &expfds, &t);
		if (ret < 0)
		{
			cout << "select 失败!" << endl;
		}
		if (FD_ISSET(m_serverSocket, &readfds))//如果可读集合中有serverSocket 有客户端连接
		{
			AcceptClient();
		}
		for (int i = 0; i < m_clientSockets.size(); i++)
		{
			if (FD_ISSET(m_clientSockets[i], &readfds))//如果客户端有可读
			{
				RecvClientMessage(m_clientSockets[i]);
			}
		}

	}
	//接受服务器连接
	void AcceptClient()
	{
		sockaddr newClientAddr;
		int addrlen = sizeof(sockaddr);
		SOCKET newClient = INVALID_SOCKET;
#ifdef _WIN32
		newClient=	accept(m_serverSocket, &newClientAddr, &addrlen);
#else
		newClient = accept(m_serverSocket, &newClientAddr, (socklen_t*)& addrlen);//参数1 listen函数用到的socket,参数2 远程客户端地址 参数3 远程客户端长度 返回远程客户端Socket
#endif // _WIN32
		if (newClient == SOCKET_ERROR||newClient==INVALID_SOCKET)
		{
			std::cout << "服务端接收客户端链接失败 ,无效的客户端Socket!" << endl;
		}
		else
		{
			cout << "新客户端连接,socket:" << newClient << endl;
			for (int i = 0; i < m_clientSockets.size(); ++i)
			{
				DataHead newClientms = {};
				newClientms.msType = Login;
				strcpy_s(newClientms.ms, "新客户端加入");
				newClientms.dataLen = sizeof(DataHead);

				SendMessageToClient(m_clientSockets[i], &newClientms);
			}
			m_clientSockets.push_back(newClient);
		}
	}
	//接受客户端消息
	void RecvClientMessage(SOCKET s)
	{
		if (INVALID_SOCKET != s) {
			DataHead dataHead;
			int ret = recv(s, (char*)& dataHead, dataHead.dataLen, 0);
			if (ret < 0) 
			{
				for (int i = 0; i < m_clientSockets.size(); i++)
				{
					if (m_clientSockets[i] == s)
					{
						m_clientSockets.erase(m_clientSockets.begin() + i);
					}
				}
#ifdef _WIN32
				closesocket(s);
#else
				close(s);
#endif // _WIN32
				cout << "收到客户端数据长度<0!关闭Socket:" << s << endl;
			}
			else{
				SwitchClientMessage(s,&dataHead);
			}
		}


	}
	//向客户端发送消息
	void SendMessageToClient(SOCKET s,DataHead *dataHead)
	{
		int ret = send(s, (char *)dataHead, dataHead->dataLen, 0);
		if (SOCKET_ERROR == ret)
		{
			cout << "向客户端发送" << dataHead->ms <<"错误"<< endl;
		}
		else
		{
			cout << "向客户端发送" << dataHead->ms << endl;
		}
	}
	//处理客户端消息
	void SwitchClientMessage(SOCKET s,DataHead *dataHead) {
		if (dataHead == NULL)return;
		switch (dataHead->msType)
		{
		case Login:
			strcpy_s(dataHead->ms,"Login成功");
			SendMessageToClient(s,dataHead);
			break;
		case Loginout:
			strcpy_s(dataHead->ms, "Loginout成功");
			SendMessageToClient(s, dataHead);
			break;
		case Delete:
			strcpy_s(dataHead->ms, "Delete成功");
			SendMessageToClient(s, dataHead);
			break;
		case Add:
			strcpy_s(dataHead->ms, "Add成功");
			SendMessageToClient(s, dataHead);
			break;
		case Exit:
			for (int i = 0; i < m_clientSockets.size(); i++)
			{
				if (m_clientSockets[i] == s)
				{
					m_clientSockets.erase(m_clientSockets.begin() + i);
				}
			}
#ifdef _WIN32
			closesocket(s);
#else
			close(s);
#endif // _WIN32

			cout << "客户端socket:" << s << "断开连接" << endl;
			break;
		case Update:
			strcpy_s(dataHead->ms, "Update成功");
			SendMessageToClient(s, dataHead);
			break;
		default:
			strcpy_s(dataHead->ms, "未知命令!");
			SendMessageToClient(s, dataHead);
			break;
		}
	}
	//关闭Socket
	void CloseServerSocket()
	{
		if (IsRun())
		{
#ifdef _WIN32
			closesocket(m_serverSocket);
			WSACleanup();
#else
			close(m_serverSocket);
#endif // _WIN32
			cout << "server socket关闭!" << endl;
			m_serverSocket = INVALID_SOCKET;
		}

	}
	//服务端Socket是否运行
	bool IsRun()
	{
		return INVALID_SOCKET != m_serverSocket;
	}

private:
	SOCKET m_serverSocket;
	std::vector<SOCKET> m_clientSockets;
};

#endif // !_SERVER_HPP_
