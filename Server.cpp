#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>

// <WinSock2.h> ������ ����ϱ� ���� ���̺귯�� �߰�
#pragma comment(lib, "ws2_32")

using namespace std;

int main()
{
	/////////////////////////////////// [ 1. ���� ���� �� ���� �ܰ� ]
	// ���� ���ڸ� ���ؼ� ����ϴ� srand �Լ� ����
	srand((unsigned int)time(nullptr));

	// ������ �������� ����ϴ� ������(.dll) �ε��ϴ� �ڵ�, ������ ���� �ڵ带 �����ؾ��մϴ�.
	// �ʱ�ȭ - 2.2 ���� ��� �� ��������
	WSAData wsaData;
	int Result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	// MS ���Ĺ����� ������ �ε� ������ 0 �� �ƴϸ� �����̵� ���� �ε� �����Դϴ�.
	// exit(-1); : ���� �߻��� �����Ű�� ���� �Լ�
	if (Result != 0)
	{
		cout << "Winsock dll error" << endl;
		exit(-1);
	}

	// ���� ����(�����⸦ ��ٸ�) ����� ������ �����մϴ�.
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
	* ���� �۾��� ��ǻ�Ϳ� ������ ��θ� �����ϴ� ������ �� ���Դϴ�.
	* ���� �ܺο��� �� ��θ� ���� �����ؾ� �ϴµ�, �ܺο��� �ҷ����� �ּҸ� IP ��� �մϴ�.
	* ����ü�� ���ؼ� �� IP �� �ְ� �޴µ� �� ����ü�� SOCKADDR �Դϴ�.
	* ������ ���� �ʿ��� Port(���� : �ױ�) - 21, 22, 80, 443 ������ �� ��Ʈ���� �ַ� ����մϴ�.
	* ��ȣȭ �۾��� �ʿ��ѵ� ���� ��ǥ���� RSA �Դϴ�.
	*/ 


	/////////////////////////////////// [ 2. ���� �ּ� �Ҵ� �ܰ� ]
	// �� ������ �츮�� ����� IP �Դϴ�.
	SOCKADDR_IN ListenSockAddr;

	memset(&ListenSockAddr, 0, sizeof(ListenSockAddr));		// IP �ʱⰪ�� ��� 0 ���� �ʱ�ȭ
	//ZeroMemory(&ListenSockAddr, sizeof(ListenSockAddr));	// �̴� �����쿡���� ����ϴ� �ʱ�ȭ �Լ� �Դϴ�.

	ListenSockAddr.sin_family = AF_INET;			// AF_INET	   : �ּ� ü�踦 IPv4 �� ����
	ListenSockAddr.sin_addr.s_addr = INADDR_ANY;	// ������ ��Ʈ�� ������ ��� IP �� �޴´ٴ� �ǹ�(�����δ� �̷��� �ϸ� �ȵ˴ϴ�)
	ListenSockAddr.sin_port = htons(30211);			// ��Ʈ��ȣ ���� : Ȯ�ο����� ���� ��ſ��� ������� �ʴ� ��Ʈ ��ȣ�Դϴ�.

	// ������ �վ����� ���� ��ī��� ������ ���ִ� �Լ��Դϴ�.
	// �� ���� ���ϰ��� �޴µ� ���� ���� �⺻���� 0 �̰� �������� ��� �����Դϴ�.
	// bind �� ���Ͽ� �ּҸ� �Ҵ��ϴ� ������ �Լ��Դϴ�.
	//	- ListenSocket				 : �ּҸ� �Ҵ��� ������ �ڵ�
	//	- (SOCKADDR*)&ListenSockAddr : ���Ͽ� �Ҵ�� �ּ� ������ ����ִ� SOCKADDR ����ü ������ �Դϴ�.
	//	- sizeof(ListenSockAddr)	 : �Ҵ�� �ּ� ������ ũ���Դϴ�.
	Result = bind(ListenSocket, (SOCKADDR*)&ListenSockAddr, sizeof(ListenSockAddr));
	if (Result == SOCKET_ERROR)
	{
		cout << "can't bind : " << GetLastError() << endl;
		exit(-1);
	}


	/*
	* �� ���� �ϸ� ������ ����	q������ ���� ����, �ش� ������ IPv4 ���·� ���� ���̰�
	* IPv4 �� ��ī�� ip�� ����ϸ� ��Ʈ��ȣ�� �츮�� �����ؼ� ����մϴ�. �׸��� bind �� ���� �����մϴ�.
	*/


	/////////////////////////////////// [ 3. ���� ��� �ܰ� ]
	// listen �� �̿��ؼ� ������ ���� ��� ���·� ��ȯ���ݴϴ�.
	// ���� ���� 5 �� ���ÿ� ó���� �� �ִ� ���� ��û�� �ִ� ������ ������ ���Դϴ�.
	Result = listen(ListenSocket, 5);
	if (Result == SOCKET_ERROR)
	{
		cout << "can't listen : " << GetLastError() << endl;
		exit(-1);
	}

	/////////////////////////////////// [ 4. ���� �ܰ� ]
	while (true)
	{
		// SOCKADDR_IN : ������ �ǹ� �״�� ������ �ּ� ������ �����ϴ� ����ü �Դϴ�.
		// memset	   : �ռ� �ߴ� �͵�� ���������� �ʱ� ���� 0 ���� �ʱ�ȭ �� �� ����մϴ�.
		SOCKADDR_IN ClientSockAddr;
		memset(&ClientSockAddr, 0, sizeof(ClientSockAddr));
		int ClientSockAddrLength = sizeof(ClientSockAddr);

		// (SOCKADDR*)&ClientSockAddr : ���� IP ����
		// &ClientSockAddrLength	  : ���� IP �� ���̰�
		SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSockAddr, &ClientSockAddrLength);
		if (ClientSocket == INVALID_SOCKET)
		{
			cout << "accept fail : " << GetLastError() << endl;
			exit(-1);
		}


		/////////////////////////////////// [ ���� ���� ���� ]
		int FirstNumber = rand() % 10000;				// 0~9999
		int SecondNumber = rand() % 9999 + 1;			// 1~9999
		int OpreatorIndex = rand() % 5;					// 0~4
		char Opreator[5] = { '+', '-', '*', '/', '%' };	// ���꿡 ����� ������ �迭�Դϴ�.

		char Buffer[1024] = { 0, };
		sprintf(Buffer, "%d%c%d", FirstNumber, Opreator[OpreatorIndex], SecondNumber);
		//sprintf_s(Buffer, "%d%c%d", FirstNumber, Opreator[OpreatorIndex], SecondNumber);


		/////////////////////////////////// [ 5. ������ �ܰ� ]
		// ������ �����͸� ���� �� �ʿ��� �������Դϴ�.
		// ClientSocket : �����͸� ������ ������ Ŭ���̾�Ʈ�� ����� �����Դϴ�.
		// Buffer		: �����ϰ��� �ϴ� �����Ͱ� ����� ���� �Դϴ�.
		// (int)(strlen(Buffer) + 1) : ������ �������� ũ���Դϴ�.
		//	- char �迭�� �����̱� ������ ���� ������ ���� NULL ���� ���� '\0' �� ������ ���� �Դϴ�.
		// 0			: flag, ���� �ɼ��� ��Ÿ���µ� 0 �� ��� �Ϲ������� �ƹ��� �ɼ� ���� �����͸� �����մϴ�.
		int SentByte = send(ClientSocket, Buffer, (int)strlen(Buffer) + 1, 0);
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


		/////////////////////////////////// [ 6. �޴� �ܰ� ]
		char RecvBuffer[1024] = { 0, };
		int RecvByte = recv(ClientSocket, RecvBuffer, sizeof(RecvBuffer), 0);
		if (RecvByte < 0)
		{
			cout << "Error : " << GetLastError() << endl;
			//exit(-1);
			continue;		// �׽�Ʈ �����̹Ƿ� ������ ������� �ʰ� ����մϴ�.
		}
		else if (RecvByte == 0)
		{
			cout << "Disconnected : " << GetLastError() << endl;
			//exit(-1);
			continue;
		}
		else
		{
			cout << "Recv byte : " << RecvByte << ", " << RecvBuffer << endl;
		}
		closesocket(ClientSocket);
	}


	/////////////////////////////////// [ 7. ���� �ܰ� ]
	closesocket(ListenSocket);

	// Winsocket ��� ���� �Լ��Դϴ�.
	WSACleanup();

	return 0;
}