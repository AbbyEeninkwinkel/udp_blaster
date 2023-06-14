//########################################################
// UDP Blaster v1.1 2023
// By Abby Eeninkwinkel
// Credits for the basis Inet engine idea:
//      Yves Chevallier https://github.com/nowox/udp-test
//#########################################################

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>
#define MAXPAYLOAD 3000
#define MAXADDRESS 255

int exit_with_instruction(int action, int value)
{
 switch (action)
  {
   case 0: printf ("Error: Please check the required arguments.\n");
           break;
   case 1: printf ("Payload to large! Maximum MAXPAYLOAD bytes\n");
           break;
   case 2: printf ("Speed packets per second is too large! Supply integer 1 - 99000.\n");
           break;
   case 3: printf ("Invalid packetspeed supplied!\n");
           break;
   case 4: printf ("Server address to long! Maximum MAXADDRESS bytes");   
           break;
   case 5: printf ("Invalid destination address supplied.");
           break;
   case 6: printf ("Incorrect port supplied!\n");
           break;
   default:
           break;
  }
 printf ("UDP Blaster ver1.0 by Abby Eeninkwinkel\n");
 printf ("Usage: udp_blaster [dest. address] [port 1..65535] [speed packets/s] [message]\n");
 exit(1);
}

int main(int argc, char **argv)
{
  struct timespec start, end;
  char   payload[MAXPAYLOAD+1];
  char   payload_short[21];
  char   saddress[MAXADDRESS+1];
  long   packetspersec = 0;
  int    sport = 0;
  long   cycle = 0;
  long   delta_us;
  int    sockfd;
  struct sockaddr_in destaddr;

  if ( argc !=5 ) exit_with_instruction(0, 0); 
  if (strlen(argv[4]) > MAXPAYLOAD-1) exit_with_instruction(1, 0);
  strcpy (payload,argv[4]);
  strncpy (payload_short, payload, 20);
  if (strlen(argv[3]) > 10) exit_with_instruction(2, 0);
  if ((packetspersec = atol(argv[3])) <= 0) exit_with_instruction(3, 0);
  float f=(1/(float)packetspersec)*1000000;
  long timeframepersend= (long) f;
  if (strlen(argv[1]) > MAXADDRESS) exit_with_instruction (4, 0); 
  strcpy(saddress, argv[1]);
  if (strlen(saddress) == 0) exit_with_instruction (5, 0); 
  if (strlen(argv[2]) > 6) exit_with_instruction (6, 0); 
  if ((sport = atol(argv[2])) < 1 || sport > 65535)  exit_with_instruction (6, 0); 
   printf("<~>~<~>~<~>~<~>~<~><~>~<~>~<~>~<~>\n\
 Destination: %s:%i\n\
 With payload: %s.... [%lu] bytes\n\
 Attempting %li packets per second.\n\
 Timeframe: %li\xC2\xB5s\n", saddress, sport, payload_short, strlen(payload), packetspersec,timeframepersend);
  printf(" Configure the socket... ");
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
     {
      fprintf(stderr, "Error opening socket");
      return EXIT_FAILURE;
    }
  printf("[ok]\n");
  bzero((char*)&destaddr, sizeof(destaddr));
  destaddr.sin_family = AF_INET;
  destaddr.sin_addr.s_addr = inet_addr(saddress);
  destaddr.sin_port = htons(sport);
  printf(" Blasting UDP data...\n");

  do
    { 
//     int ret = sendto(sockfd, payload, strlen(payload), 0, (const struct sockaddr*)&destaddr, sizeof(destaddr));
     if (sendto(sockfd, payload, strlen(payload), 0, (const struct sockaddr*)&destaddr, sizeof(destaddr)) < 0)
        {
         fprintf(stderr, "Error in sendto()\n");
         return EXIT_FAILURE;
        }
     clock_gettime(CLOCK_MONOTONIC_RAW, &start);
     cycle = 0;
     do 
       {
        cycle ++;
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
       } while (delta_us < timeframepersend);
     if (cycle < 2)
        {
         printf("Sorry. You system is throtteling udp packets with the requested speed. Reduce packers per seconds.\n");
         return -1;
        }
    } 
    while (1);
    return EXIT_SUCCESS;
}
