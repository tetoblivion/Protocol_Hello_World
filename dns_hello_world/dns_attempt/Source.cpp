
// RobotZer0  2020

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")


#pragma pack(push)
#pragma pack(1)



struct sflags_b1 {
	unsigned char
		qr : 1,      //query or response
		opcode : 4,  //opcode
		aa : 1,      //authorativeanswer 
		tc : 1,      //truncated  
		rd : 1;      //recursion desired 
};

struct sflags_b2 {
	unsigned char
		ra : 1,      //recursion available
		zeros : 3,   //zeros
		rcode : 4;   //rcode
};

struct sheader {
	unsigned short id;
	sflags_b2 flagsb2;
	sflags_b1 flagsb1;
	unsigned short nq;    //number of question records
	unsigned short nans;  //number of answer records
	unsigned short nauth; //number of authoritative records
	unsigned short nadd;  //number of additional records
};

struct question_section {
	unsigned char l1 = 3;
	char www[3] = { 'w','w','w' };
	unsigned char l2 = 10;
	char duck[10] = { 'd','u','c','k','d','u','c','k','g','o' };
	unsigned char l3 = 3;
	char com[3] = { 'c','o','m' };
	unsigned char l4 = 0;
	//
	unsigned short qtype = htons(1);  //Requests the A record for the domain name     
	unsigned short qclass = htons(1); //internet
};


struct squery {
	sheader header;
	question_section qs;

	squery() {
		header.id = htons(17); //random number
		//
		header.flagsb1.qr = 0; //0-query 1-answer
		header.flagsb1.opcode=0; //QUERY. standard query.
		header.flagsb1.aa = 1;
		header.flagsb1.tc = 0;
		header.flagsb1.rd = 1;
		header.flagsb2.ra = 0;
		header.flagsb2.zeros = 0;
		header.flagsb2.rcode = 0;
		//
		header.nq = htons(1);
		header.nans = 0;
		header.nauth = 0;
		header.nadd = 0;
	}
}query;


#pragma pack(pop)



int main() {

	int iResult;
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	char RecvBuf[1024];
	RecvBuf[1023] = '\0';

	int clientSock;

	clientSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(53); //htons converts to propper endianness for the network
	servaddr.sin_addr.s_addr = inet_addr("192.168.1.1");// INADDR_ANY;

	////////////

	sendto(clientSock,
		(const char *)&query, sizeof(query),
		0,
		(const struct sockaddr *) &servaddr, sizeof(servaddr));

	printf("Request sent.\n");

	auto n = recvfrom(clientSock,  //blocking operation
		(char *)RecvBuf, 1024,
		0, (struct sockaddr *)
		NULL, NULL);

	////////////

	if (n < 0) {
		auto er = WSAGetLastError();
		printf("Error code:%d", er);
		while (1);
		return 0;
	}

	printf("Response (hex):\n");

	for (int i = 0; i < n; i++)
	{
		printf(" %X", (unsigned char)RecvBuf[i]);
	}

	printf("\nResponse(char):\n");

	for (int i = 0; i < n; i++)
	{
		printf(" %c", (char)RecvBuf[i]);
	}

	while (1);

	return 0;
}

