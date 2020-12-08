
// RobotZer0  2020

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")


#pragma pack(push)
#pragma pack(1)

// ASN.1 / BER encoding
struct packet {
	    //snmp message
/*40*/	unsigned char type1=0x30; //sequence
/*39*/	unsigned char len1=38;
	    //snmp version
/*38*/	unsigned char type2 = 0x02; //integer
/*37*/	unsigned char len2 = 1;
/*36*/	unsigned char val2 = 1;
	    //snmp community string
/*35*/	unsigned char type3 = 0x04; //octet string
/*34*/	unsigned char len3=6;
/*33*/	unsigned char val3[6]= { 'p','u','b','l','i','c' };
	    //snmp pdu type 
/*27*/	unsigned char type4 = 0xA0; //GetRequest
/*26*/	unsigned char len4=25;
	    //request id
/*25*/	unsigned char type5 = 0x02; //integer
/*24*/	unsigned char len5 = 1;
/*23*/	unsigned char val5 = 1;
	    //error
/*22*/	unsigned char type6 = 0x02; //integer
/*21*/	unsigned char len6 = 1;
/*20*/	unsigned char val6 = 0;
	    //error index
/*19*/	unsigned char type7 = 0x02; //integer
/*18*/	unsigned char len7 = 1;
/*17*/	unsigned char val7 = 0;
	    //varbind list
/*16*/	unsigned char type8 = 0x30; //sequence
/*15*/	unsigned char len8=14;
	    //varbind
/*14*/	unsigned char type9 = 0x30; //sequence
/*13*/	unsigned char len9=12;
	    //oid
/*12*/	unsigned char type10 = 0x06; //oid
/*11*/	unsigned char len10=8;
/*10*/	unsigned char val10[8] = {0x2B,6,1,2,1,1,5,0}; //sysName //symbolic name
	    //oid value
/*02*/	unsigned char type11 = 0x05; //null
/*01*/	unsigned char len11 = 0;		
} request;

#pragma pack (pop)

const int r2size = 124;
unsigned char r2[r2size] = { //captured with wireshark between oidview and ruter
//udp and other headers deleted here
0x30,0x7a,
0x02,0x01,0x01,
0x04 ,0x06,
0x70,0x75,0x62,0x6c,0x69,0x63, //public
0xa0,0x6d,
0x02,0x01,0x03,
0x02,0x01,0x00,
0x02,0x01,0x00,
0x30,0x62,
0x30,0x0c,0x06,0x08,0x2b,0x06,0x01,0x02,0x01,0x01,0x01,0x00,
0x05,0x00,
0x30,0x0c,0x06,0x08,0x2b,0x06,0x01,0x02,0x01,0x01,0x02,0x00,
0x05,0x00,
0x30,0x0c,0x06,0x08,0x2b,0x06,0x01,0x02,0x01,0x01,0x03,0x00,
0x05,0x00,
0x30,0x0c,0x06,0x08,0x2b,0x06,0x01,0x02,0x01,0x01,0x04,0x00,
0x05,0x00,
0x30,0x0c,
0x06,0x08,
0x2b,0x06,0x01,0x02,0x01,0x01,0x05,0x00, //oid
0x05,0x00,
0x30,0x0c,0x06,0x08,0x2b,0x06,0x01,0x02,0x01,0x01,0x06,0x00,
0x05,0x00,
0x30,0x0c,0x06,0x08,0x2b,0x06,0x01,0x02,0x01,0x01,0x07,0x00,
0x05,0x00
};


int main(int argc, wchar_t* argv[]) {


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
	servaddr.sin_port = htons(161); //htons converts to propper endianness for the network
	servaddr.sin_addr.s_addr = inet_addr("192.168.1.1");// INADDR_ANY;
	//servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");


	///////////////////

	if (1) {
		sendto(clientSock, 
			(const char *)&request, sizeof(request),
			0, 
			(const struct sockaddr *) &servaddr,sizeof(servaddr));
	}
	else {
		sendto(clientSock, 
			(const char *)r2, r2size,
			0, 
			(const struct sockaddr *) &servaddr,sizeof(servaddr));
	}

	printf("Request sent.\n");

	auto n = recvfrom(clientSock,   //blocking operation
		(char *)RecvBuf, 1024,
		0, (struct sockaddr *) 
		NULL,NULL);

	//////////////////////////

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