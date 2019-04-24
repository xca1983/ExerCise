#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
#include<thread>
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
		cout << "�������û���:" << endl;
		cin >> recvdata._name;
		cout << "����������:" << endl;
		cin >> recvdata._password;
		recvdata.dataLen = sizeof(DataPackage);
		recvdata.msType = Login;
		//4.�������������Ϣ
		if (SOCKET_ERROR == send(s, (const char*)&recvdata, sizeof(DataPackage), 0))
		{
			cout << "�������������Ϣʧ��" << endl;
		}//���������������������Ϣ
		else
		{
			cout << "����������ͳɹ���Ϣ" << endl;
		}
	}
}
int main() {
	//1.Winsock����ĳ�ʼ�� WSAStartup
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);
	//2.����Socket
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == clientSocket) {
		cout << "�ͻ��˴���Socketʧ��!" << endl;
	}
	else
	{
		cout << "�ͻ��˴���Socket�ɹ�! �����Socket:" << clientSocket<< endl;
	}
	//3.���ӷ�����������
	sockaddr_in serverAddr = {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(4567);
	serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == connect(clientSocket, (sockaddr*)& serverAddr, sizeof(sockaddr))) {
		cout << "���ӷ�����ʧ��!" << endl;
	}
	else {
		cout << "���ӷ������ɹ�!" << endl;

	}

	//char msBuff[128] = {};
	char msbuff2[128];
	//int recvlen = recv(clientSocket, msBuff, 128, 0);
	//cout << "��������Ϣ:" << msBuff << endl;
	thread t1(cmdThread, clientSocket);//����һ���߳�
	while (true) {

		fd_set readfds; FD_ZERO(&readfds);
		timeval times = { 1,0 };
		FD_SET(clientSocket, &readfds);
		int ret=select(clientSocket + 1, &readfds, 0, 0, &times);
		if (ret < 0)
		{
			cout << "��������Ͽ�����" << endl;
			break;
		}
		if (FD_ISSET(clientSocket, &readfds))
		{
			//5.���շ�������Ϣ		
			int recvlen = recv(clientSocket, msbuff2, 128, 0);
			cout << "���������س���:" << recvlen << endl;
			if (recvlen > 0)
			{
				cout << "�յ����Է���������Ϣ:" << msbuff2 << endl;
			}
			else
			{
				cout << "��������Ӧʧ��" << endl;
			}
		}
		//cout << "���߳̿���ʱ��������ҵ��" << endl;
	}
	//6.�ر�Socket
	closesocket(clientSocket);
	//7.Winsock�����ע�� WSACleanup()
	WSACleanup();
	cout << "Hello Client!" << endl;
	system("pause");
	return EXIT_SUCCESS;
}