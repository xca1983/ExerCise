#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
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
		cout << "客户端创建Socket成功! 服务端Socket:" << clientSocket<< endl;
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

	//char msBuff[128] = {};
	char msbuff2[128];
	//int recvlen = recv(clientSocket, msBuff, 128, 0);
	//cout << "服务器消息:" << msBuff << endl;
	DataPackage recvdata = {sizeof(DataPackage),Login,"张三","123456"};
	while (true) {

		fd_set readfds; FD_ZERO(&readfds);
		timeval times = { 1,0 };
		FD_SET(clientSocket, &readfds);
		int ret=select(clientSocket + 1, &readfds, 0, 0, &times);
		if (ret < 0)
		{
			cout << "与服务器断开连接" << endl;
			break;
		}
		if (FD_ISSET(clientSocket, &readfds))
		{
			//5.接收服务器信息		
			int recvlen = recv(clientSocket, msbuff2, 128, 0);
			cout << "服务器返回长度:" << recvlen << endl;
			if (recvlen > 0)
			{
				cout << "收到来自服务器的消息:" << msbuff2 << endl;
			}
			else
			{
				cout << "服务器响应失败" << endl;
			}
		}
		cout << "请输入用户名:" << endl;
		//cin >> recvdata._name;
		//cout << "请输入密码:" << endl;
		//cin >> recvdata._password;

		//4.向服务器发送消息
		//if (SOCKET_ERROR == send(clientSocket, (const char*)&recvdata, sizeof(DataPackage), 0)) {
		//	cout << "向服务器发送消息失败" << endl;
		//}//根据输入项服务器发送消息
		//else
		//{
		//	cout << "向服务器发送成功消息" << endl;
		//}

		
	}
	//6.关闭Socket
	closesocket(clientSocket);
	//7.Winsock服务的注销 WSACleanup()
	WSACleanup();
	cout << "Hello Client!" << endl;
	system("pause");
	return EXIT_SUCCESS;
}