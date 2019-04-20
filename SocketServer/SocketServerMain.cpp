#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include<Windows.h>
#include<iostream>
#include<WinSock2.h>
using namespace std;
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
	char msBuff[] = "Hello I'm SocketServer";
	char mbuff[] = "δ֪����,����������!";
	char clientms[256] = {};
	clientSocket = accept(sock, (sockaddr*)&clientAddr, &clientAddrLen);//����1 listen�����õ���socket,����2 Զ�̿ͻ��˵�ַ ����3 Զ�̿ͻ��˳��� ����Զ�̿ͻ���Socket
	if (SOCKET_ERROR == clientSocket) {
		std::cout << "����˽��տͻ�������ʧ�� ,��Ч�Ŀͻ���Socket!" << endl;
	}
	else {
		std::cout << "����˽��տͻ������ӳɹ��ɹ�!" << endl;
		std::cout << "�¿ͻ��˼���ɹ�,IP��ַΪ:!" << inet_ntoa(clientAddr.sin_addr) << endl;
	}

	while (true) {
		//6.���տͻ�����Ϣ
		int recvLen = recv(clientSocket, clientms, 256, 0);
		if (recvLen <= 0) {
			cout << "�ͻ������˳�!" << endl;
			break;
		}
		else
		{
			cout << "�յ��ͻ�����Ϣ:" << clientms << endl;
		}
		if (strcmp(clientms, "Hello")) {
			send(clientSocket, mbuff, strlen(msBuff) + 1, 0);
		}
		else
		{
			//7.����˷�������
			send(clientSocket, msBuff, strlen(msBuff) + 1, 0);
		}
	}

	//8.�ر�Socket
	closesocket(clientSocket);
	WSACleanup();//Winsock�����ע�� WSAStartup

	cout << "Hello Socket!" << endl;
	system("pause");
	return 0;
}