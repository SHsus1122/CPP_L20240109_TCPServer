#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>

// <WinSock2.h> 소켓을 사용하기 위한 라이브러리 추가
#pragma comment(lib, "ws2_32")

using namespace std;

int main()
{
	/////////////////////////////////// [ 1. 소켓 생성 및 설정 단계 ]
	// 랜덤 숫자를 위해서 사용하는 srand 함수 선언
	srand((unsigned int)time(nullptr));

	// 윈도우 전용으로 사용하는 소켓을(.dll) 로딩하는 코드, 리눅스 사용시 코드를 변경해야합니다.
	// 초기화 - 2.2 버전 사용 및 가져오기
	WSAData wsaData;
	int Result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	// MS 공식문서에 들어가보면 로드 성공시 0 이 아니면 무엇이든 간에 로드 실패입니다.
	// exit(-1); : 에러 발생시 종료시키기 위한 함수
	if (Result != 0)
	{
		cout << "Winsock dll error" << endl;
		exit(-1);
	}

	// 리슨 서버(들어오기를 기다림) 방식의 소켓을 생성합니다.
	//  - PF_INET	  : TCP, UDP 프로토콜을 사용하겠다는 의미 (추가로 IPv6 는 AF_INET6)
	//	- SOCK_STREAM : 연결 지향형 데이터 전송(통로 개설)
	//	- IPPROTO_TCP : TCP 수준에서 적용 가능한 소켓 옵션 선택(통로 옵션 설정) 
	//					현재는 거의 TCP 만 사용하기 때문에 0 으로 해도 무방합니다.
	SOCKET ListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == INVALID_SOCKET)
	{
		// GetLastError() : 에러 종류에 대한 것을 번호로 알려줍니다.(에러 종류는 구글 검색)
		cout << "INVALID_SOCKET : " << GetLastError() << endl;
		exit(-1);
	}


	/*
	* 위의 작업은 컴퓨터에 가상의 통로를 개설하는 행위를 한 것입니다.
	* 이제 외부에서 이 통로를 통해 연결해야 하는데, 외부에서 불려지는 주소를 IP 라고 합니다.
	* 구조체를 통해서 이 IP 를 주고 받는데 그 구조체가 SOCKADDR 입니다.
	* 연결을 위해 필요한 Port(번역 : 항구) - 21, 22, 80, 443 지금은 이 포트들을 주로 사용합니다.
	* 암호화 작업도 필요한데 가장 대표격이 RSA 입니다.
	*/ 


	/////////////////////////////////// [ 2. 소켓 주소 할당 단계 ]
	// 이 변수가 우리가 사용할 IP 입니다.
	SOCKADDR_IN ListenSockAddr;

	memset(&ListenSockAddr, 0, sizeof(ListenSockAddr));		// IP 초기값을 모두 0 으로 초기화
	//ZeroMemory(&ListenSockAddr, sizeof(ListenSockAddr));	// 이는 윈도우에서만 사용하는 초기화 함수 입니다.

	ListenSockAddr.sin_family = AF_INET;			// AF_INET	   : 주소 체계를 IPv4 로 설정
	ListenSockAddr.sin_addr.s_addr = INADDR_ANY;	// 지정한 포트로 들어오는 모든 IP 를 받는다는 의미(실제로는 이렇게 하면 안됩니다)
	ListenSockAddr.sin_port = htons(30211);			// 포트번호 설정 : 확인용으로 실제 통신에선 사용하지 않는 포트 번호입니다.

	// 소켓을 뚫었으면 이제 랜카드와 연결을 해주는 함수입니다.
	// 이 또한 리턴값을 받는데 위와 같이 기본값이 0 이고 나머지는 모두 실패입니다.
	// bind 는 소켓에 주소를 할당하는 역할의 함수입니다.
	//	- ListenSocket				 : 주소를 할당할 소켓의 핸들
	//	- (SOCKADDR*)&ListenSockAddr : 소켓에 할당될 주소 정보를 담고있는 SOCKADDR 구조체 포인터 입니다.
	//	- sizeof(ListenSockAddr)	 : 할당된 주소 정보의 크기입니다.
	Result = bind(ListenSocket, (SOCKADDR*)&ListenSockAddr, sizeof(ListenSockAddr));
	if (Result == SOCKET_ERROR)
	{
		cout << "can't bind : " << GetLastError() << endl;
		exit(-1);
	}


	/*
	* 위 까지 하면 랜선을 꼽을	q가상의 소켓 생성, 해당 소켓은 IPv4 형태로 만든 것이고
	* IPv4 는 랜카드 ip를 사용하며 포트번호는 우리가 선택해서 사용합니다. 그리고 bind 를 통해 연결합니다.
	*/


	/////////////////////////////////// [ 3. 수신 대기 단계 ]
	// listen 을 이용해서 소켓을 수신 대기 상태로 전환해줍니다.
	// 뒤의 숫자 5 는 동시에 처리할 수 있는 연결 요청의 최대 갯수를 설정한 것입니다.
	Result = listen(ListenSocket, 5);
	if (Result == SOCKET_ERROR)
	{
		cout << "can't listen : " << GetLastError() << endl;
		exit(-1);
	}

	/////////////////////////////////// [ 4. 수락 단계 ]
	while (true)
	{
		// SOCKADDR_IN : 쓰여진 의미 그대로 소켓의 주소 정보를 저장하는 구조체 입니다.
		// memset	   : 앞서 했던 것들과 마찬가지로 초기 값을 0 으로 초기화 할 때 사용합니다.
		SOCKADDR_IN ClientSockAddr;
		memset(&ClientSockAddr, 0, sizeof(ClientSockAddr));
		int ClientSockAddrLength = sizeof(ClientSockAddr);

		// (SOCKADDR*)&ClientSockAddr : 들어온 IP 정보
		// &ClientSockAddrLength	  : 들어온 IP 의 길이값
		SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSockAddr, &ClientSockAddrLength);
		if (ClientSocket == INVALID_SOCKET)
		{
			cout << "accept fail : " << GetLastError() << endl;
			exit(-1);
		}


		/////////////////////////////////// [ 랜덤 계산식 생성 ]
		int FirstNumber = rand() % 10000;				// 0~9999
		int SecondNumber = rand() % 9999 + 1;			// 1~9999
		int OpreatorIndex = rand() % 5;					// 0~4
		char Opreator[5] = { '+', '-', '*', '/', '%' };	// 연산에 사용할 연산자 배열입니다.

		char Buffer[1024] = { 0, };
		sprintf(Buffer, "%d%c%d", FirstNumber, Opreator[OpreatorIndex], SecondNumber);
		//sprintf_s(Buffer, "%d%c%d", FirstNumber, Opreator[OpreatorIndex], SecondNumber);


		/////////////////////////////////// [ 5. 보내는 단계 ]
		// 서버로 데이터를 보낼 때 필요한 정보들입니다.
		// ClientSocket : 데이터를 전송할 서버가 클라이언트에 연결된 소켓입니다.
		// Buffer		: 전송하고자 하는 데이터가 저장된 버퍼 입니다.
		// (int)(strlen(Buffer) + 1) : 전송할 데이터의 크기입니다.
		//	- char 배열의 형태이기 때문에 에러 방지를 위해 NULL 종료 문자 '\0' 을 포함한 길이 입니다.
		// 0			: flag, 여러 옵션을 나타내는데 0 인 경우 일반적으로 아무런 옵션 없이 데이터를 전송합니다.
		int SentByte = send(ClientSocket, Buffer, (int)strlen(Buffer) + 1, 0);
		if (SentByte < 0)
		{
			// 0 보다 작으면 못 보낸 것이며 이는 네트워크가 끊겼다는 의미입니다.
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


		/////////////////////////////////// [ 6. 받는 단계 ]
		char RecvBuffer[1024] = { 0, };
		int RecvByte = recv(ClientSocket, RecvBuffer, sizeof(RecvBuffer), 0);
		if (RecvByte < 0)
		{
			cout << "Error : " << GetLastError() << endl;
			//exit(-1);
			continue;		// 테스트 목적이므로 서버가 종료되지 않게 사용합니다.
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


	/////////////////////////////////// [ 7. 종료 단계 ]
	closesocket(ListenSocket);

	// Winsocket 사용 종료 함수입니다.
	WSACleanup();

	return 0;
}