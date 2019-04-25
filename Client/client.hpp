#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_
#ifdef _WIN32
#include<windows.h>
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
using std::cout;
using std::endl;
class Client
{
public:
	Client()
	{
		m_sock = INVALID_SOCKET;
	}
	~Client()
	{
		CloseScoket();
	}
	//1.初始化Socket
	void InitSocket()
	{
#ifdef _WIN32
		WORD word = MAKEWORD(2, 2);
		WSADATA data;
		WSAStartup(word, &data);
#endif // _WIN32
		if (INVALID_SOCKET != m_sock)//如果已经有Socket 关闭Socket
		{
			cout << "socket 已经存在,关闭Socket" << endl;
			CloseScoket();
		}
		m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_sock == INVALID_SOCKET)
		{
			cout << "Creat socket falied!" << endl;
		}
		else
		{
			cout << "Creat socket success!" << endl;
		}
	}
	//2.连接服务器
	int ConnectServer(const char* ip,unsigned short port)
	{
		if (INVALID_SOCKET == m_sock)//如果socket还未创建
		{
			cout << "socket还未创建!" << endl;
			InitSocket();
		}
		sockaddr_in serveraddr = {};
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(port);
#ifdef _WIN32
		serveraddr.sin_addr.S_un.S_addr = inet_addr(ip);
#else
		serveraddr.sin_addr.S_un.S_addr = inet_addr(ip);
#endif // _WIN32

		int connret = connect(m_sock, (sockaddr*)&serveraddr, sizeof(sockaddr));
		if (SOCKET_ERROR == connret)
		{
			cout << "连接服务器失败!" << endl;
		}
		else
		{
			cout << "connect server success!" << endl;
		}
		return connret;
	}
	//3.关闭Socket
	void CloseScoket()
	{
		if (m_sock != INVALID_SOCKET)
		{
#ifdef _WIN32
			closesocket(m_sock);
			WSACleanup();
#else
			close(m_sock);
#endif // _WIN32
			cout << "关闭socket" << endl;
			m_sock = INVALID_SOCKET;
		}
	}
	//4.发送数据
	int SendData(DataHead *dataHead)
	{
		if (INVALID_SOCKET == m_sock||dataHead==NULL)
		{
			cout << "socket 未创建,或待发消息为空" <<endl;
			return SOCKET_ERROR;
		}
		return	send(m_sock, (char*)dataHead, dataHead->dataLen, 0);
	}
	//5.select检测是否有消息到达
	bool OnRun()
	{
		if (INVALID_SOCKET == m_sock)
		{
			return false;
		}
		fd_set readfds; FD_ZERO(&readfds);
		FD_SET(m_sock, &readfds);
		timeval t = { 1,0 };
		int ret = select(m_sock + 1, &readfds, NULL, NULL, &t);
		if (ret < 0)
		{
			cout << "select失败" << endl;
			return false;
		}
		if (FD_ISSET(m_sock, &readfds))
		{
			cout << "处理服务端消息" << endl;
			RecvData();
		}
		return true;
	}
	//6.接收数据
	void RecvData()
	{
		if (INVALID_SOCKET == m_sock)
		{
			cout << "接收数据失败 Socket null" << endl;
		}
		DataHead dataHead;
		int reclen = recv(m_sock,(char*)&dataHead, dataHead.dataLen, 0);
		if (reclen < 0)
		{
			cout << "服务端响应长度<0,关闭Socket." << endl;
			CloseScoket();
		}
		SwitchRecv(&dataHead);
	}
	//7.处理数据
	int SwitchRecv(DataHead *dataHead)
	{
		if (!dataHead)
		{
			cout << "待处理数据为null" << endl;
			return -1;
		}
		switch (dataHead->msType)
		{
		case Login:
			cout << "服务器返回Login信息:" << dataHead->ms << endl;
			break;
		case Loginout:
			cout << "服务器返回Loginout信息:" << dataHead->ms << endl;
			break;
		case Delete:
			cout << "服务器返回Delete信息:" << dataHead->ms << endl;
			break;
		case Add:
			cout << "服务器返回Add信息:" << dataHead->ms << endl;
			break;
		case Update:
			cout << "服务器返回Update信息:" << dataHead->ms << endl;
			break;
		case Exit:
			cout << "服务器返回Exit信息:" << dataHead->ms << endl;
			break;
		default:
			cout << "服务器返回None信息:" << dataHead->ms << endl;
			break;
		}
	}
	bool IsRun()
	{
		return INVALID_SOCKET != m_sock;
	}
private:
	SOCKET m_sock;
};
#endif // !_CLIENT_HPP_
