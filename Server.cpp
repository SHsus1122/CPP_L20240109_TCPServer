#include <iostream>
#include <WinSock2.h>

// <WinSock2.h> ������ ����ϱ� ���� ���̺귯�� �߰�
#pragma comment(lib, "ws2_32")

using namespace std;

int main()
{
	// ������ �������� ����ϴ� ������(.dll) �ε��ϴ� �ڵ�, ������ ���� �ڵ带 �����ؾ��մϴ�.
	WSAData wsaData;
	// �ʱ�ȭ - 2.2 ���� ��� �� ��������
	int Result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	// MS ���Ĺ����� ������ �ε� ������ 0 �� �ƴϸ� �����̵� ���� �ε� �����Դϴ�.
	if (Result != 0)
	{
		cout << "Winsock dll error" << endl;
		exit(-1);	// ���� �߻��� �����Ű�� ���� �Լ�
	}

	// ���� ����(�����⸦ ��ٸ�) ����� ����
	//  - PF_INET	  : TCP, UDP ���������� ����ϰڴٴ� �ǹ� (�߰��� IPv6 �� AF_INET6)
	//	- SOCK_STREAM : ���� ������ ������ ����(��� ����)
	//	- IPPROTO_TCP : TCP ���ؿ��� ���� ������ ���� �ɼ� ����(��� �ɼ� ����) 
	//					����� ���� TCP �� ����ϱ� ������ 0 ���� �ص� �����մϴ�.
	SOCKET ListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == INVALID_SOCKET)
	{
		// GetLastError() : ���� ������ ���� ���� ��ȣ�� �˷��ݴϴ�.(���� ������ ���� �˻�)
		cout << "INVALID_SOCKET : " << GetLastError() << endl;
		exit(-1);
	}

	/*
	���� �۾��� ��ǻ�Ϳ� ������ ��θ� �����ϴ� ������ �� ���Դϴ�.
	���� �ܺο��� �� ��θ� ���� �����ؾ� �ϴµ�, �ܺο��� �ҷ����� �ּҸ� IP ��� �մϴ�.
	����ü�� ���ؼ� �� IP �� �ְ� �޴µ� �� ����ü�� SOCKADDR �Դϴ�.
	������ ���� �ʿ��� Port(���� : �ױ�) - 21, 22, 80, 443 ������ �� ��Ʈ���� �ַ� ����մϴ�.
	��ȣȭ �۾��� �ʿ��ѵ� ���� ��ǥ���� RSA �Դϴ�.
	*/ 

	// �� ������ �츮�� ����� IP �Դϴ�.
	SOCKADDR_IN ListenSockAddr;

	memset(&ListenSockAddr, 0, sizeof(ListenSockAddr));		// IP �ʱⰪ�� ��� 0 ���� �ʱ�ȭ
	//ZeroMemory(&ListenSockAddr, sizeof(ListenSockAddr));	// �̴� �����쿡���� ����ϴ� �ʱ�ȭ �Լ� �Դϴ�.

	ListenSockAddr.sin_family = AF_INET;			// AF_INET	   : �ּ� ü�踦 IPv4 �� ����
	ListenSockAddr.sin_addr.s_addr = INADDR_ANY;	// ������ ��Ʈ�� ������ ��� IP �� �޴´ٴ� �ǹ�(�����δ� �̷��� �ϸ� �ȵ˴ϴ�)
	ListenSockAddr.sin_port = htons(30211);			// ��Ʈ��ȣ ���� : Ȯ�ο����� ���� ��ſ��� ������� �ʴ� ��Ʈ ��ȣ�Դϴ�.

	// ������ �վ����� ���� ��ī��� ������ ���ִ� �Լ��Դϴ�.
	// �� ���� ���ϰ��� �޴µ� ���� ���� �⺻���� 0 �̰� �������� ��� �����Դϴ�.
	Result = bind(ListenSocket, (SOCKADDR*)&ListenSockAddr, sizeof(ListenSockAddr));
	if (Result == SOCKET_ERROR)
	{
		cout << "can't bind : " << GetLastError() << endl;
		exit(-1);
	}
	// �� ���� �ϸ� ������ ���� ������ ���� ����, �ش� ������ IPv4 ���·� ���� ���̰�
	// IPv4 �� ��ī�� ip�� ����ϸ� ��Ʈ��ȣ�� �츮�� �����ؼ� ����մϴ�. �׸��� bind �� ���� �����մϴ�.


	// ���� ���� 5 �� ��⿭�� ���ÿ� ��û�� ���� �� �ִµ� �� ���� ��⿭ �ִ� �ο��� ���մϴ�.
	Result = listen(ListenSocket, 5);
	if (Result == SOCKET_ERROR)
	{
		cout << "can't listen : " << GetLastError() << endl;
		exit(-1);
	}


	SOCKADDR_IN ClientSockAddr;
	memset(&ClientSockAddr, 0, sizeof(ClientSockAddr));
	int ClientSockAddrLength = sizeof(ClientSockAddr);


	/////////////////////////////////// [ ���� �ܰ� ]
	// (SOCKADDR*)&ClientSockAddr : ���� IP ����
	// &ClientSockAddrLength	  : ���� IP �� ���̰�
	SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSockAddr, &ClientSockAddrLength);
	if (ClientSocket == INVALID_SOCKET)
	{
		cout << "accept fail : " << GetLastError() << endl;
		exit(-1);
	}


	/////////////////////////////////// [ ������ �ܰ� ]
	const char Buffer[] = { "Hello World" };
	int SentByte = send(ClientSocket, Buffer, strlen(Buffer), 0);
	if (SentByte < 0)
	{
		// 0 ���� ������ �� ���� ���̸� �̴� ��Ʈ��ũ�� ����ٴ� �ǹ��Դϴ�.
		cout << "Error : " << GetLastError() << endl;
		exit(-1);
	}
	else if (SentByte == 0)
	{
		cout << "Disconnected : " << GetLastError() << endl;
		exit(-1);
	}
	else
	{
		cout << "Sent byte : " << SentByte << ", " << Buffer << endl;
	}


	/////////////////////////////////// [ �޴� �ܰ� ]
	char RecvBuffer[1024] = { 0, };
	int RecvByte = recv(ClientSocket, RecvBuffer, sizeof(RecvBuffer), 0);
	if (RecvByte < 0)
	{
		// 0 ���� ������ �� ���� ���̸� �̴� ��Ʈ��ũ�� ����ٴ� �ǹ��Դϴ�.
		cout << "Error : " << GetLastError() << endl;
		exit(-1);
	}
	else if (RecvByte == 0)
	{
		cout << "Disconnected : " << GetLastError() << endl;
		exit(-1);
	}
	else
	{
		cout << "Recv byte : " << RecvByte << ", " << RecvBuffer << endl;
	}


	/////////////////////////////////// [ ���� �ܰ� ]
	closesocket(ClientSocket);
	closesocket(ListenSocket);

	// Winsocket ��� ���� �Լ��Դϴ�.
	WSACleanup();

	return 0;
}