#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>


int flag = 1;
struct nodeAndService
{
   char *node, *service, *myhostname;
};

struct nodeAndService nameAndport;


/*
Takes input from the terminal and sends that through a socket to a destination port. Exits when '!' is typed.
*/
void *SendMsg(void *arg)
{
	char* argv2;
	char* argv3;
	struct nodeAndService *my_data;
	my_data = (struct nodeAndService *) arg;
	argv2 = my_data->node;
	argv3 = my_data->service;

	int len;
	char *msg;
	char buffer[1000];
	int sockfd;    
	int status, error;
	int bytesSent;
	struct addrinfo hints;
	struct addrinfo *res;  // will point to the results

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_INET;  
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; 

	
	if ((status = getaddrinfo( argv2, argv3, &hints, &res)) != 0) {
	    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
	    exit(1);
	}

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd == -1){exit(0);}

	error = connect(sockfd, res->ai_addr, res->ai_addrlen);// connect the socket to the destination address
	if(error == -1){exit(0);}

	while(1){

		msg = fgets(buffer, 1000, stdin);// gets input from the terminal

		if(msg[0] == '!'){// close thread if '!' was typed

			printf("You typed ! and therefore terminated your program.\n");
			send(sockfd, msg, len, 0); 
			close(sockfd);
			freeaddrinfo(res);
			flag = 0;
			exit(0); 
		}

		len = strlen(buffer);
		bytesSent = send(sockfd, msg, len, 0);
	
		while(bytesSent < len){// this ensures that the message is sent competely
			msg = (buffer + bytesSent);
			len = len - bytesSent;
			bytesSent = send(sockfd, msg, len, 0 );
		}

	}

}





/*
Continously receives the messages and terminates when the SendMsg thread has terminated.
*/
void *ReceiveMsg(void *arg){

	char* argv1;
	struct nodeAndService *my_data;
	my_data = (struct nodeAndService *) arg;
	argv1 = my_data->myhostname;

	char buffer[1001];
	int sockfd;    
	int status, error;
	int bytesRead;
	struct addrinfo hints;
	struct addrinfo *res;  // will point to the results
	
	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_INET;  
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // fill in my IP for me

	
	if ((status = getaddrinfo(NULL, argv1, &hints, &res)) != 0) {
    	fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    	exit(1);
	}  
	// res now points to a linked list of 1 or more struct addrinfos
	
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd == -1){exit(0);}
	

	// bind the address to the socket
	error = bind(sockfd, res->ai_addr, res->ai_addrlen);
	if(error == -1){exit(0);}
	

	while(flag == 1){//exit while loop when the SendMsg thread sets flag to 0	
		
		bytesRead = recv(sockfd, buffer, 1000, 0); 
		buffer[bytesRead] = '\0'; //this ensures that the previous msg is not reprinted to the screen
		printf("You received: %s\n", buffer);

		if(buffer[0] == '!'){
			printf("***The other user terminated his/her program.***\n");
	
		}
	}
	
	close(sockfd);
	freeaddrinfo(res);
	pthread_exit(NULL); 

}

/*
Starts the program and creates the SendMsg and ReceiveMsg thread.
*/
int main(int argc, char *argv[])
{

nameAndport.myhostname = argv[1];
nameAndport.node = argv[2];
nameAndport.service = argv[3];

pthread_t threads[2];
int rc;
  
  rc = pthread_create(&threads[0], NULL, ReceiveMsg, (void*) &nameAndport);
       if (rc) {
    printf("ERROR; return code from pthread_create() is %d\n", rc);
    exit(-1);
    	}


  rc = pthread_create(&threads[1], NULL, SendMsg, (void *) &nameAndport);
      if (rc) {
    printf("ERROR; return code from pthread_create() is %d\n", rc);
    exit(-1);
    	}

  
  //wait for SendMsg to terminate	
  rc = pthread_join(threads[1], NULL);
      if (rc) {
         printf("ERROR; return code from pthread_join() is %d\n", rc);
         exit(-1);
         }
      printf("-----------PROGRAM TERMINATED: GOODBYE-----------\n");
  
  //wait for ReceiveMsg to terminate
  rc = pthread_join(threads[0], NULL);
      if (rc) {
         printf("ERROR; return code from pthread_join() is %d\n", rc);
         exit(-1);
         }
      printf("-----------PROGRAM TERMINATED: GOODBYE-----------\n");
  
return 0;

}


