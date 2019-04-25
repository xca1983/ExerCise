#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include<Windows.h>
#include<iostream>
#include<WinSock2.h>
using namespace std;
int main00()
{
	WSAData wsa;
	if (WSAStartup(MAKEWORD(1, 1), &wsa) != 0)
	{
		WSACleanup();
		return 0;
	}
	SOCKET listensocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	do
	{
		if (listensocket == INVALID_SOCKET)
			break;
		sockaddr_in service;
		service.sin_family = AF_INET;
		service.sin_port = htons(8828);
		service.sin_addr.s_addr = INADDR_ANY;
		if (bind(listensocket, (sockaddr*)&service, sizeof(service)) == SOCKET_ERROR)
		{
			cout << "bind error,error code " << WSAGetLastError() << endl;
			break;
		}
		if (listen(listensocket, SOMAXCONN) == SOCKET_ERROR)
		{
			cout << "listen error,error code " << WSAGetLastError() << endl;
			break;
		}
		//以下是select相关代码
		//此处把listensocket设置为非阻塞模式
		u_long unblock = 1;    //非0为非阻塞模式
		if (ioctlsocket(listensocket, FIONBIO, &unblock) == SOCKET_ERROR)
		{
			cout << "ioctlsocket(listensocket) error,error code " << WSAGetLastError() << endl;
			break;
		}
		fd_set m_readset; FD_ZERO(&m_readset);
		fd_set m_writeset; FD_ZERO(&m_writeset);
		FD_SET(listensocket, &m_readset);
		SOCKET socks[64];
		socks[0] = listensocket;
		int TotalSockets = 1;
		while (1)
		{
			fd_set readset = m_readset;
			fd_set writeset = m_writeset;
			timeval tv;
			tv.tv_sec = 1;
			tv.tv_usec = 0;
			int res = select(0, &readset, &writeset, NULL, &tv);
			if (res == SOCKET_ERROR)
			{
				cout << "select error,error code " << WSAGetLastError() << endl;
				break;
			}
			if (res == 0)
			{
				continue;//表示当前无状态可控的socket
			}
			int tempTotalSockets = TotalSockets;
			for (int i = 0; i<TotalSockets; i++)
			{
				if (FD_ISSET(socks[i], &readset))
				{
					if (socks[i] == listensocket)
					{
						//listensocket可读，表示连接到达
						SOCKET acp = accept(listensocket, NULL, NULL);
						if (acp == INVALID_SOCKET)
						{
							cout << "accept error,error code " << WSAGetLastError() << endl;
							break;
						}
						//设置新到达socket为非阻塞模式，并加入socks以及fdset
						if (ioctlsocket(acp, FIONBIO, &unblock) == SOCKET_ERROR)
						{
							cout << "ioctlsocket(acp) error,error code " << WSAGetLastError() << endl;
							break;
						}
						FD_SET(acp, &m_readset);
						FD_SET(acp, &m_writeset);
						socks[tempTotalSockets++] = acp;
					}
					else
					{
						char buf[1024];
						int len = recv(socks[i], buf, 1024, 0);
						if (len == 0)
						{
							cout << "connection has been closed " << endl;
							break;
						}
						else if (len == SOCKET_ERROR)
						{
							cout << "recv error,error code " << WSAGetLastError() << endl;
							break;
						}
						else
						{
							char* outbuf = new char[len + 1];
							memcpy(outbuf, buf, len);
							outbuf[len] = 0;
							cout << "recv data," << outbuf << endl;
							delete outbuf;
						}

					}
				}
				else if (FD_ISSET(socks[i], &writeset))
				{
					std::string str = "send data!";
					if (send(socks[i], str.c_str(), str.length(), 0) == SOCKET_ERROR)
					{
						cout << "send error,error code " << WSAGetLastError() << endl;
						break;
					}
				}
			}
			TotalSockets = tempTotalSockets;
		}

	} while (0);
	closesocket(listensocket);
	WSACleanup();
}