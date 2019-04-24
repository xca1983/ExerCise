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
//using namespace std;
using std::cout;
using std::endl;
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
		cout << "�ͻ������˳�!" << endl;
		return -1;
	}
	else
	{
		cout << "�յ��ͻ���" << sock << "����������" << endl;
		cout << "�յ��ͻ��˷������ݳ���:" << recvData.dataLen << endl;
		cout << "�յ�����Ϣ����:" << recvData.msType << endl;
		cout << "�յ����û���:" << recvData._name << endl;
		cout << "�յ�������:" << recvData._password << endl;
		switch (recvData.msType)
		{
		case Login:
			if (strcmp(recvData._name, "����") == 0)
			{
				if (strcmp(recvData._password, "123456") == 0)
				{
					//send(clientSocket, "��¼�ɹ�!", 128, 0);
					send(sock, "��¼�ɹ�!", 128, 0);
					cout << "��ͻ���" << sock << "���͵�¼�ɹ���Ϣ!" << endl;
				}
				else
				{
					send(sock, "�������!", 128, 0);
					cout << "��ͻ���" << sock << "�������������Ϣ!" << endl;
				}
			}
			else
			{
				send(sock, "�û�������", 128, 0);

				cout << "��ͻ���" << sock << "�����û���������Ϣ!" << endl;
			}

			break;
		default:
			send(sock, "δ֪����!", 128, 0);

			cout << "��ͻ���" << sock << "����δ֪����!��Ϣ!" << endl;
			break;
		}
	}
}

int main()
{
#ifdef _WIN32
	//1.Winsock����ĳ�ʼ�� WSAStartup
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);
#endif // _WIN32

	//2.����Socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);//����Socket ����1 socket
	if (SOCKET_ERROR == sock)
	{
		cout << "Creat Socket failed!" << endl;
	}
	else
	{
		cout << "Creat Socket success!:" << sock << endl;
	}
	//3.�����ڽ��տͻ������ӵ�����˿�
	sockaddr_in _sin = {};//ָ����ַ
	_sin.sin_family = AF_INET;//��ַ���õ�Э��
	_sin.sin_port = htons(4567);//��ַ�˿�
#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//��ַ
#else
	_sin.sin_addr.s_addr = INADDR_ANY; //inet_addr("129.204.40.157");
#endif // _WIN32
	if (SOCKET_ERROR == bind(sock, (sockaddr*)&_sin, sizeof(sockaddr_in)))
	{//�󶨶˿�
		cout << "bind failed!" << endl;
	}
	else
	{
		cout << "bind success!" << endl;
	}
	//4.��������˿�
	if (SOCKET_ERROR == listen(sock, 5))
	{
		std::cout << "listen failed!" << endl;
	}
	else
	{
		std::cout << "listen success!" << endl;
	}


	timeval times = { 1,0 };
	SOCKET socks[4];
	socks[0] = sock;		//������Socket��������(�����Socket)
	int totalSock = 1;


	while (true)
	{
		//cout << "Socket����:" << totalSock << endl;
		fd_set readfds; FD_ZERO(&readfds);//�ɶ�����
		fd_set writefds; FD_ZERO(&writefds);//��д����
		fd_set exceptfds; FD_ZERO(&exceptfds);//���⼯��

		for (int i = 0; i < totalSock; ++i)
		{
			FD_SET(socks[i], &readfds);//������Socket����ɶ����� (�����Socket)
			FD_SET(socks[i], &writefds);//������Socket�����д����(�����Socket)
			FD_SET(socks[i], &exceptfds);//������Socket�����쳣����(�����Socket)
		}
		int ret = select(sock + 1, &readfds, &writefds, &exceptfds, &times);
		if (ret < 0)
		{
			cout << "select failed " << endl;

			break;
		}

		int m_totalSock = totalSock;
		for (int i = 0; i < m_totalSock; ++i)//����socks���鼯�� 
		{
			if (FD_ISSET(socks[i], &readfds))// �����sock�ڿɶ���������          
			{
				if (socks[i] == sock)//��������sock�ڶ��������� ���¿ͻ������ӵ���
				{
					sockaddr_in clientAddr = {};//Զ�̿ͻ��˵�ַ
					int clientAddrLen = sizeof(sockaddr_in);
					SOCKET clientSocket = INVALID_SOCKET;
#ifdef _WIN32
					clientSocket = accept(sock, (sockaddr*)&clientAddr, &clientAddrLen);//����1 listen�����õ���socket,����2 Զ�̿ͻ��˵�ַ ����3 Զ�̿ͻ��˳��� ����Զ�̿ͻ���Socket
#else
					clientSocket = accept(sock, (sockaddr*)&clientAddr, (socklen_t*)& clientAddrLen);//����1 listen�����õ���socket,����2 Զ�̿ͻ��˵�ַ ����3 Զ�̿ͻ��˳��� ����Զ�̿ͻ���Socket
#endif // _WIN32

					if (SOCKET_ERROR == clientSocket)
					{
						std::cout << "����˽��տͻ�������ʧ�� ,��Ч�Ŀͻ���Socket!" << endl;
						break;
					}
					else
					{
						std::cout << "����˽��տͻ������ӳɹ��ɹ�!Socket" << clientSocket << endl;
						std::cout << "�¿ͻ��˼���ɹ�,IP��ַΪ:!" << inet_ntoa(clientAddr.sin_addr) << endl;
						/*	char msbuff[128];
						recv(clientSocket, msbuff, 128, 0);
						cout << "�յ��ͻ�����Ϣ" << msbuff << endl;*/
						//send(clientSocket, "I'm server!", 128, 0);
						for (int i = 1; i < totalSock; i++)
						{
							send(socks[i], "���û�����", 128, 0);
						}
						socks[totalSock++] = clientSocket;
						cout << "��Socket��:" << totalSock << endl;
					}
				}
				else
				{	//6.����ĳ���ͻ�����Ϣ

					if (RecvMessage(socks[i]) == -1)
					{
						/*for (int i = 0; i < totalSock; ++i)
						{

						}*/
						break;
					}
				}
			}
		}
	}

	//8.�ر�Socket
	//closesocket(clientSocket);
#ifdef _WIN32
	//closesocket(clientSocket);
	WSACleanup();
#else
	close(sock);
#endif // _WIN32


	cout << "Hello Socket!" << endl;
	system("pause");
	return 0;
}


