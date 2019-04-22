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
	//1.Winsock����ĳ�ʼ�� WSAStartup
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);//������windows�Ķ�̬�⣬�������ļ� ws2_32.lib  ��ʼ��Ws2_32.dll�ĺ���
	//2.����Socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);//����Socket ����1 socket
	//3.�����ڽ��տͻ������ӵ�����˿�
	sockaddr_in _sin = {};//ָ����ַ
	_sin.sin_family = AF_INET;//��ַ���õ�Э��
	_sin.sin_port = htons(4567);//��ַ�˿�
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//��ַ

	if (SOCKET_ERROR == bind(sock, (sockaddr*)&_sin, sizeof(sockaddr_in))) {//�󶨶˿�
		std::cout << "����˰󶨶˿�ʧ��!" << endl;
	}
	else {
		std::cout << "����˰󶨶˿ڳɹ�!" << endl;
	}
	//4.��������˿�
	if (SOCKET_ERROR == listen(sock, 5)) {
		std::cout << "����˼����˿�ʧ��!" << endl;
	}
	else {
		std::cout << "����˼����˿ڳɹ�!" << endl;
	}
	//5.�ȴ����տͻ�������   
	sockaddr_in clientAddr = {};//Զ�̿ͻ��˵�ַ
	int clientAddrLen = sizeof(sockaddr_in);
	SOCKET clientSocket = INVALID_SOCKET;
	clientSocket = accept(sock, (sockaddr*)&clientAddr, &clientAddrLen);//����1 listen�����õ���socket,����2 Զ�̿ͻ��˵�ַ ����3 Զ�̿ͻ��˳��� ����Զ�̿ͻ���Socket
	if (SOCKET_ERROR == clientSocket) {
		std::cout << "����˽��տͻ�������ʧ�� ,��Ч�Ŀͻ���Socket!" << endl;
	}
	else {
		std::cout << "����˽��տͻ������ӳɹ��ɹ�!" << endl;
		std::cout << "�¿ͻ��˼���ɹ�,IP��ַΪ:!" << inet_ntoa(clientAddr.sin_addr) << endl;
		send(clientSocket, "I'm server", 128, 0);
	}

	while (true) {
		//6.���տͻ�����Ϣ
		DataPackage recvData;
		int recvLen = recv(clientSocket, (char*)&recvData, 128, 0);
		if (recvLen <= 0) {
			cout << "�ͻ������˳�!" << endl;
			break;
		}
		else
		{
			cout << "�յ������ݳ���:" << recvData.dataLen << endl;
			cout << "�յ�����Ϣ����:" << recvData.msType << endl;
			cout << "�յ����û���:" << recvData._name << endl;
			cout << "�յ�������:" << recvData._password << endl;
			switch (recvData.msType)
			{
			case Login:
				if (strcmp(recvData._name, "����") == 0) {
					if (strcmp(recvData._password, "123456") == 0) {
						//send(clientSocket, "��¼�ɹ�!", 128, 0);
						send(clientSocket, "��¼�ɹ�!", 128, 0);
						cout << "���͵�¼�ɹ���Ϣ!" << endl;
					}
					else
					{
						send(clientSocket, "�������!", 128, 0);
					}
				}
				else
				{
					send(clientSocket, "�û�������", 128, 0);
				}
				break;
			default:
				send(clientSocket, "δ֪����!", 128, 0);
				break;
			}
		}
	}

	//8.�ر�Socket
	closesocket(clientSocket);
	WSACleanup();//Winsock�����ע�� WSAStartup

	cout << "Hello Socket!" << endl;
	system("pause");
	return 0;
}