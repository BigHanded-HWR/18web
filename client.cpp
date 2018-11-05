// GBN_client.cpp : �������̨Ӧ�ó������ڵ㡣
//#include "stdafx.h"
#include <stdlib.h>
#include<stdio.h>
#include <WinSock2.h>
#include <time.h>
#include <WS2tcpip.h>

#pragma comment(lib,"ws2_32.lib")
#define SERVER_PORT  12340 //�������ݵĶ˿ں�
#define SERVER_IP  "127.0.0.1" // �������� IP ��ַ
const int BUFFER_LENGTH = 1026;
const int SEQ_SIZE = 20;//���ն����кŸ�����Ϊ 1~20
/****************************************************************/
/* -time �ӷ������˻�ȡ��ǰʱ��
-quit �˳��ͻ���
-testgbn [X] ���� GBN Э��ʵ�ֿɿ����ݴ���
[X] [0,1] ģ�����ݰ���ʧ�ĸ���
[Y] [0,1] ģ�� ACK ��ʧ�ĸ���
*/
/****************************************************************/
void printTips() {
	printf("*****************************************\n");
	printf("| -time to get current time |\n");
	printf("| -quit to exit client |\n");
	printf("| -testgbn [X] [Y] to test the gbn |\n");
	printf("*****************************************\n");
}
//************************************
// Method: lossInLossRatio
// FullName: lossInLossRatio
// Access: public
// Returns: BOOL
// Qualifier: ���ݶ�ʧ���������һ�����֣��ж��Ƿ�ʧ,��ʧ�򷵻�TRUE�����򷵻� FALSE
// Parameter: float lossRatio [0,1]
//************************************
BOOL lossInLossRatio(float lossRatio) {
	int lossBound = (int)(lossRatio * 100);
	int r = rand() % 101;
	if (r <= lossBound) {
		return TRUE;
	}
	return FALSE;
}
int main(int argc, char* argv[])
{
	//�����׽��ֿ⣨���룩
	WORD wVersionRequested;
	WSADATA wsaData;
	//�׽��ּ���ʱ������ʾ
	int err;
	//�汾 2.2
	wVersionRequested = MAKEWORD(2, 2);
	//���� dll �ļ� Scoket ��
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		//�Ҳ��� winsock.dll
		printf("WSAStartup failed with error: %d\n", err);
		return 1;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		printf("Could not find a usable version of Winsock.dll\n");
		WSACleanup();
	}
	else {
		printf("The Winsock 2.2 dll was found okay\n");
	}
	SOCKET socketClient = socket(AF_INET, SOCK_DGRAM, 0);
	SOCKADDR_IN addrServer;
	addrServer.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
	//addrServer.sin_addr.S_un.S_addr = inet_pton(AF_INET, SERVER_IP, 0);
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(SERVER_PORT);
	//���ջ�����
	char buffer[BUFFER_LENGTH];
	ZeroMemory(buffer, sizeof(buffer));
	int len = sizeof(SOCKADDR);
	//Ϊ�˲���������������ӣ�����ʹ�� -time ����ӷ������˻�õ�ǰʱ��
	//ʹ�� -testgbn [X] [Y] ���� GBN ����[X]��ʾ���ݰ���ʧ����
	//  [Y]��ʾ ACK ��������
	printTips();
	int ret;
	int interval = 1;//�յ����ݰ�֮�󷵻� ack �ļ����Ĭ��Ϊ 1 ��ʾÿ�������� ack��0 ���߸�������ʾ���еĶ������� ack
	char cmd[128];
	float packetLossRatio = 0.2; //Ĭ�ϰ���ʧ�� 0.2
	float ackLossRatio = 0.2;  //Ĭ�� ACK ��ʧ�� 0.2
							   //��ʱ����Ϊ������ӣ�����ѭ����������
	srand((unsigned)time(NULL));
	while (true) {
		gets_s(buffer);
		//scanf_s  ��������
		//ʹ��%s��ʽ�����ݽ���ʱ�����峤�ȱ�������ַ������ȣ����������  ���ӣ�%s,str,str.length
		//%d %f ����ĸ�ʽ��ƥ��
		ret = sscanf(buffer, "%s%f%f", &cmd, &packetLossRatio, &ackLossRatio);
		//��ʼ GBN ���ԣ�ʹ�� GBN Э��ʵ�� UDP �ɿ��ļ�����
		//printf("%s\n",cmd);
		if (!strcmp(cmd, "-time")) {
			int recvSend = sendto(socketClient,"-time", strlen("-time") + 1, 0, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
			if (recvSend == -1)
			{
				printf("����ʧ��\n");
				//10013 - WSAEACCES   
				//Ȩ�ޱ��ܡ����Զ��׽��ֽ��в�����������ֹ������ͼ��sendto��WSASendTo��ʹ��
				//һ���㲥��ַ��������δ��setsockopt��SO_BROADCAST������ѡ�����ù㲥Ȩ�ޣ�
				//�������������
				printf("%d\n", WSAGetLastError());
			}
			else {
				recvSend = recvfrom(socketClient, buffer, BUFFER_LENGTH, 0, (SOCKADDR*)&addrServer, &len);
				if (recvSend == SOCKET_ERROR)
				{
					printf("����ʧ��\n");
				}
				else printf("%s\n", &buffer[0]);
			}
		}
		if (!strcmp(cmd, "-quit")) {
			int recvSend = sendto(socketClient, "-quit", strlen("-quit") + 1, 0, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
			if (recvSend == -1)
			{
				printf("����ʧ��\n");
				//10013 - WSAEACCES   
				//Ȩ�ޱ��ܡ����Զ��׽��ֽ��в�����������ֹ������ͼ��sendto��WSASendTo��ʹ��
				//һ���㲥��ַ��������δ��setsockopt��SO_BROADCAST������ѡ�����ù㲥Ȩ�ޣ�
				//�������������
				printf("%d\n", WSAGetLastError());
			}
			else {
				recvSend = recvfrom(socketClient, buffer, BUFFER_LENGTH, 0, (SOCKADDR*)&addrServer, &len);
				if (recvSend == SOCKET_ERROR)
				{
					printf("����ʧ��\n");
				}
				else printf("%s\n", &buffer[0]);
			}
		}
		if (!strcmp(cmd, "-testgbn")) {
			//printf("%s\n", "Begin to test GBN protocol, please don't abort the	process");
			printf("The loss ratio of packet is %.2f,the loss ratio of ack	is %.2f\n", packetLossRatio, ackLossRatio);
			int waitCount = 0;
			int stage = 0;
			BOOL b;
			unsigned char u_code;//״̬��
			unsigned short seq;//�������к�
			unsigned short recvSeq;//���մ��ڴ�СΪ 1����ȷ�ϵ����к�
			unsigned short waitSeq;//�ȴ������к�
				//int sendto(int s, const void * msg, int len, unsigned int flags, const struct sockaddr * to, int tolen);
				//����˵����sendto() ������������ָ����socket �����Է�����.
				//����s Ϊ�ѽ������ߵ�socket, �������UDPЭ�����辭�����߲���.
				//����msg ָ�������ߵ���������, ����flags һ����0, ��ϸ������ο�send().
				//����to ����ָ�������͵������ַ, �ṹsockaddr ��ο�bind(). 
				//����tolen Ϊsockaddr �Ľ������.
			    //
			int recvSend;
			recvSend = sendto(socketClient, "-testgbn", strlen("-testgbn") + 1, 0,(SOCKADDR*)&addrServer, sizeof(SOCKADDR));
			if (recvSend == -1)
			{
				printf("����ʧ��\n");
				//10013 - WSAEACCES   
				//Ȩ�ޱ��ܡ����Զ��׽��ֽ��в�����������ֹ������ͼ��sendto��WSASendTo��ʹ��
				//һ���㲥��ַ��������δ��setsockopt��SO_BROADCAST������ѡ�����ù㲥Ȩ�ޣ�
				//�������������
				printf("%d\n",WSAGetLastError());
			}
			while (true)
			{
				//�ȴ� server �ظ����� UDP Ϊ����ģʽ
				recvSend =  recvfrom(socketClient, buffer, BUFFER_LENGTH, 0, (SOCKADDR*)&addrServer, &len);
				if (recvSend == SOCKET_ERROR)
				{
					printf("����ʧ��\n");
				}
				switch (stage) {
				case 0://�ȴ����ֽ׶�
					u_code = (unsigned char)buffer[0];
					if ((unsigned char)buffer[0] == 205)
					{
						printf("Ready for file transmission\n");
						buffer[0] = 200;
						buffer[1] = '\0';
						sendto(socketClient, buffer, 2, 0,(SOCKADDR*)&addrServer, sizeof(SOCKADDR));
						stage = 1;
						recvSeq = 0;
						waitSeq = 1;
					}
					break;
				case 1://�ȴ��������ݽ׶�
					seq = (unsigned short)buffer[0];
					//�����ģ����Ƿ�ʧ
					b = lossInLossRatio(packetLossRatio);
					if (b) {
						printf("The packet with a seq of %d loss\n", seq);
						continue;
					}
					printf("recv a packet with a seq of %d\n", seq);
					//������ڴ��İ�����ȷ���գ�����ȷ�ϼ���
					if (!(waitSeq - seq)) {
						++waitSeq;
						if (waitSeq == 21) {
							waitSeq = 1;
						}
						//�������
						printf("%s\n",&buffer[1]);
						buffer[0] = seq;
						recvSeq = seq;
						buffer[1] = '\0';
					}
					else {
						//�����ǰһ������û���յ�����ȴ� Seq Ϊ 1 �����ݰ��������򲻷��� ACK����Ϊ��û����һ����ȷ�� ACK��
						if (!recvSeq) {
							continue;
						}
						buffer[0] = recvSeq;
						buffer[1] = '\0';
					}
					b = lossInLossRatio(ackLossRatio);
					if (b) {
						printf("The  ack  of  %d  loss\n", (unsigned char)buffer[0]);
						continue;
					}
					sendto(socketClient, buffer, 2, 0,(SOCKADDR*)&addrServer, sizeof(SOCKADDR));
					printf("send a ack of %d\n", (unsigned char)buffer[0]);
					break;
				}
				Sleep(500);
			}
		}
		sendto(socketClient, buffer, strlen(buffer) + 1, 0,(SOCKADDR*)&addrServer, sizeof(SOCKADDR));
		ret = recvfrom(socketClient, buffer, BUFFER_LENGTH, 0, (SOCKADDR*)&addrServer,&len);
		//printf("%s\n", buffer);
		if (!strcmp(cmd, "-quit")) {
			
			break;
		}
		printTips();
	}
	//�ر��׽���
	closesocket(socketClient);
	WSACleanup();
	system("pause");
	return 0;
}