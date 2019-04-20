#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
using namespace std;
int main() {
	//1.Winsock服务的初始化 WSAStartup
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);
	//2.创建Socket
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == clientSocket) {
		cout << "客户端创建Socket失败!" << endl;
	}
	else
	{
		cout << "客户端创建Socket成功!" << endl;
	}
	//3.连接服务器服务器
	sockaddr_in serverAddr = {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(4567);
	serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == connect(clientSocket, (sockaddr*)& serverAddr, sizeof(sockaddr))) {
		cout << "连接服务器失败!" << endl;
	}
	else {
		cout << "连接服务器成功!" << endl;
	}

	char msBuff[256] = {};
	while (true) {
		char sendBuff[256] = {};
		cin >> sendBuff;
		//4.向服务器发送消息
		send(clientSocket, sendBuff, 256, 0);//根据输入项服务器发送消息
		//5.接收服务器信息
		int recvlen = recv(clientSocket, msBuff, 256, 0);
		cout << "服务器返回长度:" << recvlen << endl;
		if (recvlen > 0) {
			cout << "收到来自服务器的消息:" << msBuff << endl;
		}
		else
		{
			cout << "服务器响应失败" << endl;
		}

	}
	//6.关闭Socket
	closesocket(clientSocket);
	//7.Winsock服务的注销 WSACleanup()
	WSACleanup();
	cout << "Hello Client!" << endl;
	system("pause");
	return EXIT_SUCCESS;
}