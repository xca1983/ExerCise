#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include<Windows.h>
#include<iostream>
#include<WinSock2.h>
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
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);//调用了windows的动态库，需加入库文件 ws2_32.lib  初始化Ws2_32.dll的函数
	//2.创建Socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);//创建Socket 参数1 socket
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
	//5.等待接收客户端连接   
	sockaddr_in clientAddr = {};//远程客户端地址
	int clientAddrLen = sizeof(sockaddr_in);
	SOCKET clientSocket = INVALID_SOCKET;
	clientSocket = accept(sock, (sockaddr*)&clientAddr, &clientAddrLen);//参数1 listen函数用到的socket,参数2 远程客户端地址 参数3 远程客户端长度 返回远程客户端Socket
	if (SOCKET_ERROR == clientSocket) {
		std::cout << "服务端接收客户端链接失败 ,无效的客户端Socket!" << endl;
	}
	else {
		std::cout << "服务端接收客户端链接成功成功!" << endl;
		std::cout << "新客户端加入成功,IP地址为:!" << inet_ntoa(clientAddr.sin_addr) << endl;
		send(clientSocket, "I'm server", 128, 0);
	}

	while (true) {
		//6.接收客户端消息
		DataPackage recvData;
		int recvLen = recv(clientSocket, (char*)&recvData, 128, 0);
		if (recvLen <= 0) {
			cout << "客户端已退出!" << endl;
			break;
		}
		else
		{
			cout << "收到的数据长度:" << recvData.dataLen << endl;
			cout << "收到的消息类型:" << recvData.msType << endl;
			cout << "收到的用户名:" << recvData._name << endl;
			cout << "收到的密码:" << recvData._password << endl;
			switch (recvData.msType)
			{
			case Login:
				if (strcmp(recvData._name, "张三") == 0) {
					if (strcmp(recvData._password, "123456") == 0) {
						//send(clientSocket, "登录成功!", 128, 0);
						send(clientSocket, "登录成功!", 128, 0);
						cout << "发送登录成功消息!" << endl;
					}
					else
					{
						send(clientSocket, "密码错误!", 128, 0);
					}
				}
				else
				{
					send(clientSocket, "用户名错误", 128, 0);
				}
				break;
			default:
				send(clientSocket, "未知错误!", 128, 0);
				break;
			}
		}
	}

	//8.关闭Socket
	closesocket(clientSocket);
	WSACleanup();//Winsock服务的注销 WSAStartup

	cout << "Hello Socket!" << endl;
	system("pause");
	return 0;
}