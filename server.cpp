/* CS 447 Network Communications (Spring 2024) */
/* the sample server process (for Project #1)  */

// Updated for Visual Studio 2019
// include link-library: wsock32.lib or ws2_32.lib (from Project Properties)

// Last Updated: 12:19 p.m., January 19, 2024

#define _CRT_SECURE_NO_WARNINGS  // avoid C4996 warnings (needed for Visual Studio 2019)

/*----- Include files -------------------------------------------------------*/
#include <iostream>
#include <string>           /* Needed for memcpy() and strcpy()              */
#include <time.h>           /* Needed for clock() and CLK_TCK                */
#include <fcntl.h>          /* for O_WRONLY, O_CREAT                         */
#include <windows.h>        /* Needed for all Winsock stuff                  */
#include <sys\timeb.h>      /* Needed for ftime() and timeb structure        */

using namespace std;
/*----- Defines -------------------------------------------------------------*/
#define  SERV_PORT_NUM     8050   /* Server Side Port number                 */
#define  MAX_BUFFER         100   /* Maximum Buffer Size in bytes            */
#define  MAX_CONNECTIONS      3   /* up to 3 connections                     */

// MODULE MAIN ===============================================================
void main(void)
{
	int status;                   // status-code from socket APIs

	int server_socket;            // socket descriptor for primary connection
	int child_socket;             // socket descriptor for duplicated connection

	int size;                     // size of a sock_addr structure

    /* load the Winsock library ---------------------------------------------- */
	WORD wVersionRequested = MAKEWORD(1, 1); /* Version 1.1                    */
	WSADATA wsaData;                         /* for the loaded winsock library */

    /* the sockaddr structures ----------------------------------------------- */
	struct sockaddr_in   server_addr;     /* for server (IP address and port #)*/
	struct sockaddr_in   client_addr;     /* for client (IP address and port #)*/

	char   in_buf[MAX_BUFFER];            /* The in-coming buffer              */
	char   out_buf[MAX_BUFFER];           /* The out-going buffer              */

    /* activate the winsock libabry ------------------------------------------ */
	WSAStartup(wVersionRequested, &wsaData);

	/* create a new TCP/IP socket -------------------------------------------- */
	/* AF_INET = "IP" and SOCK_STREAM = "TCP"                                  */
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	/* error check for "socket API" ------------------------------------------ */
	if (server_socket == INVALID_SOCKET)
	{  printf("Error at socket(): %ld\n", WSAGetLastError());  }

	/* set up "sockaddr structure" for this server --------------------------- */
	server_addr.sin_family = AF_INET;                         /* "IP" protocol */
	server_addr.sin_port = htons(int(SERV_PORT_NUM));         /* TCP port #    */
	server_addr.sin_addr.s_addr = htons(INADDR_ANY); /* Server-side IP address */

    /* connect this server process to a designated TCP port ------------------ */
	status = bind(server_socket, (struct sockaddr*) & server_addr, sizeof server_addr);

	/* error check for bind API ---------------------------------------------- */
	if (status < 0)
	{  cout << "BIND Error " << WSAGetLastError() << " " << WSAGetLastError() << endl;  }

	/* start listening to the TCP port --------------------------------------- */
	status = listen(server_socket, MAX_CONNECTIONS);

	/* error check for listen API -------------------------------------------- */
	if (status < 0)
	{  cout << "LISTEN Error " << WSAGetLastError() << endl;  }

	/* the server-side main (infinite) loop ---------------------------------- */
	while (true)
	{
		/* wait for a new client on the primary TCP port ----------- */
		size = sizeof(client_addr);
		child_socket = accept(server_socket, (struct sockaddr*) & client_addr, &size);

		/* if a TCP connection is successfully established with this client -- */
		if (child_socket != INVALID_SOCKET)
		{
			cout << "\nConnection from " << inet_ntoa(client_addr.sin_addr)
				<< " arrived (Client-Side TCP Port No = " << htons(client_addr.sin_port) << ")\n";

			/* Clean up the outgoing buffers ----------------------- */
			for (int j = 0; j < MAX_BUFFER; j++)
			{
				out_buf[j] = '\0';
				in_buf[j] = '\0';
			}

			/* receive a client request ---------------------------- */
			recv(child_socket, in_buf, MAX_BUFFER, 0);

			/* put Client ID and current time in out_buf and send to Client */
			out_buf[0] = in_buf[0];
			out_buf[1] = ' ';
			char time[MAX_BUFFER];

			/* get and append the timestamp to the return message -- */
			_strtime(time);          // grab the current timestamp
			strcat(out_buf, time);   // apend it to the return message

            /* for testing-purpose only --- */
			cout << "Client ID = " << out_buf[0] << " at " << time << endl << endl;

			/* send it back to this client ------------------------- */
			send(child_socket, out_buf, MAX_BUFFER, 0);

			/* drop the connection through the duplicated TCP port - */
			closesocket(child_socket);
		}
	} // END OF THE WHILE-LOOP ////////////////////
}
// END OF THE SERVER PROCESS //////////////////////////////////////////////