/* CS 447 Network Communications (Spring 2024)                               */
/* clien_2023.cpp: the sample client process (for Project #1)                */

// Updated for Visual Studio 2019
// Include link-library: wsock32.lib or ws2_32.lib (from Project Properties)

// Last Updated: 12:14 p.m., January 19, 2024

/*----- Include files -------------------------------------------------------*/
#include <stdio.h>          /* Needed for printf()                           */
#include <string.h>         /* Needed for memcpy() and strcpy()              */
#include <time.h>           /* Needed for clock() and CLK_TCK                */
#include <fcntl.h>          /* for O_WRONLY, O_CREAT                         */

#include <windows.h>        /* Needed for all Winsock stuff                  */
#include <sys\timeb.h>      /* Needed for ftime() and timeb structure        */

/*----- Defines -------------------------------------------------------------*/
#define  MAX_BUFFER     100    /* Maximum Buffer Size                        */
#define  NUM_LOOPS      15     /* Number of client repeats                   */

/*------ Prototypes ---------------------------------------------------------*/
void sleep(clock_t wait);      /* wait for specific time (in milisecond)     */

/*===== The Main ============================================================*/
void main(int argc, char* argv[])
{
	int                  i, j;           /* Loop counters                     */

	unsigned int         my_s;           /* My (client) socket descriptor     */

	struct sockaddr_in server_addr;      /* Server-side IP address and port # */
	struct sockaddr_in   my_addr;        /* client-side IP address and port # */

	char   in_buf[MAX_BUFFER];           /* The in-coming buffer              */
	char   out_buf[MAX_BUFFER];          /* The out-going buffer              */

	int    buffer_size = MAX_BUFFER;     /* set up the max. buffer size       */

	int    NumBytes;                     /* recieved bytes                    */
	int    status;                       /* socket API result status          */
	int    addrlen;                      /* size of a sock_addr structure     */

	char* stopstring;                    /* for "strtod" function             */
	double dTempPort;                    /* for "strtod" function             */

        /* load the Winsock library --------------------------------------------- */
	WORD wVersionRequested = MAKEWORD(1, 1);/* Version 1.1                    */
	WSADATA wsaData;                        /* for the loaded winsock library */

        /* import the command-line argument ------------------------------------- */
	if (argc < 4)
	{
		printf("Number of arguments is not correct.\n");
		printf("Format: client <client-ID> <IP address of the server> <port #>\n");
	}

	/* for debuging purpose -------------------------------------------------- */
	else
	{
		printf("\nClient ID: %s\n", argv[1]);
		printf("IP Address: %s\n", argv[2]);
		dTempPort = strtod(argv[3], &stopstring);
		printf("Port#: %d\n\n", int(dTempPort));

		/* THE MAIN LOOP ------------------------------------------------------ */
		for (i = 0; i < NUM_LOOPS; i++)
		{
			/* activate the winsock libabry ----------------------------------- */
			WSAStartup(wVersionRequested, &wsaData);

			/* create a new TCP/IP socket ------------------------------------- */
			/* AF_INET = "IP" and SOCK_STREAM = "TCP"                              */
			my_s = socket(AF_INET, SOCK_STREAM, 0);

			/* Set this client's IP address and TCP port neumber ----------------- */
			server_addr.sin_family = AF_INET;
			server_addr.sin_addr.s_addr = inet_addr(argv[2]);        /* IP address */
			server_addr.sin_port = htons(int(dTempPort));    /* Port number to use */
			addrlen = sizeof(server_addr);       /* the size of the server address */

                        /* establish a TCP/IP connect to the remote server ------------------- */
			status = connect(my_s, (LPSOCKADDR)& server_addr, sizeof(server_addr));

			/* error check for "connect" API ------------------------------------- */
			if (status < 0)
			{
				printf("Connect Error ... \n");
				i = NUM_LOOPS;
			}

			/* If a TCP/IP connection is successfully established ---------------- */
			else
			{
				/* Clean up the buffers ------------------------------------------- */
				for (j = 0; j < MAX_BUFFER; j++)
				{
					out_buf[j] = '\0';
					in_buf[j] = '\0';
				}

				/* construct the first payload message to the server ------------- */
				out_buf[0] = argv[1][0];  // One character code from command arguments
				out_buf[1] = '\n';        // Null-terminate it

                                /* send the message to the remote server ------------------------- */
				status = send(my_s, out_buf, strlen(out_buf), 0);

				/* error check for "send" API ------------------------------------ */
				if (status < 0)
				{
					printf("Initial Request Error ... \n");
				}

				/* receive a message from the server ----------------------------- */
				NumBytes = recv(my_s, in_buf, MAX_BUFFER, 0);

				/* error check for recv API -------------------------------------- */
				if (NumBytes < 0)
				{
					printf("Receiving Error ... \n");
				}

				/* if this client successfully receives a message from the server  */
				else
				{
					/* display the returned message from the server ------------------ */
					printf("Reply Received: %d bytes received.\n", NumBytes);
					printf("Reply Message: %s.\n\n", in_buf);
				}

				/* close the TCP/IP socket connection ---------------------------- */
				status = closesocket(my_s);

				/* error check for closesockt API -------------------------------- */
				if (status < 0)
				{
					printf("CLOSE Error ... \n");
				}
			}

			/* unload the WinSock library ------------------------------------ */
			WSACleanup();

			/* insert 1 second delay before repeating -------------------------*/
			sleep((clock_t)1 * CLOCKS_PER_SEC);

		} // Loop X15 (as defined by "NUM_LOOPS")
	} // if the number of the command-arguments is corect
}
// END OF MODULE MAIN -------------------------------------------------------- */

/* Wait for specific time (in milisecond) ---------------------------------- */
void sleep(clock_t wait)
{
	clock_t goal;
	goal = wait + clock();
	while (goal > clock())
		;
}

// === THE END OF LINES ==================================================== */
