#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include"client.hpp"
#include"Message.hpp"
using namespace std;

void cmdThread(Client *client)
{
	while (true)
	{
		DataHead recvdata;
		cout << "please enter username:" << endl;
		cin >> recvdata.ms;
		recvdata.dataLen = sizeof(DataHead);
		if (0 == strcmp(recvdata.ms, "Login"))
		{
			recvdata.msType = Login;
		}
		else if (0 == strcmp(recvdata.ms, "Loginout"))
		{
			recvdata.msType = Loginout;
		}
		else if (0 == strcmp(recvdata.ms, "Delete"))
		{
			recvdata.msType = Delete;
		}
		else if (0 == strcmp(recvdata.ms, "Add"))
		{
			recvdata.msType = Add;
		}
		else if (0 == strcmp(recvdata.ms, "Update"))
		{
			recvdata.msType = Update;
		}
		else if (0 == strcmp(recvdata.ms, "Exit"))
		{
			recvdata.msType = Exit;
			client->SendData(&recvdata);
			client->CloseScoket();
		}
		else
		{
			recvdata.msType = None;
		}
		//4.向服务器发送消息
		client->SendData(&recvdata);
	}
}
int main()
{
	Client *client = new Client();
	client->InitSocket();//初始化Socket
	client->ConnectServer("127.0.0.1", 4567);
	thread t1(cmdThread, client);//启动一个线程
	t1.detach();
	while (client->IsRun())
	{
		client->OnRun();
	}
	client->CloseScoket();
	cout << "==================================" << endl;
	system("pause");
	return EXIT_SUCCESS;
}