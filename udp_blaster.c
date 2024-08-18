//########################################################
// UDP Blaster v1.3 2024
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
#include <signal.h>
#define MAXPAYLOAD 3000
#define MAXADDRESS 255
#define FALSE 0
#define TRUE 1

unsigned long packet_nr = 0;

void  INThandler(int sig)
{
 signal(sig, SIG_IGN);
 printf("\nEnded; Sent %ld packets\n", packet_nr);
 exit(0);
}

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
 printf ("UDP Blaster ver1.3 by Abby Eeninkwinkel\n");
 printf ("Usage: udp_blaster [dest. address] [port 1..65535] [speed packets/s] [message]\n");
 printf ("options:\n");
 // printf ("   -i         ignore system throtteling break\n" ); New feature yet to come
 printf ("   [SEQUENCE] in your message will generate a hexadecimal sequence number exactly in that placeholder\n");
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
  int    use_sequence = FALSE;
  long   prev_packet_nr = 0;
  char   sequence_str[9];
  int    i, prt_timer = 0;
  char   * sequence_pos;
  struct sockaddr_in destaddr;

  signal(SIGINT, INThandler);
  if ( argc !=5 ) exit_with_instruction(0, 0);
  if (strlen(argv[4]) > MAXPAYLOAD-1) exit_with_instruction(1, 0);
  strcpy (payload,argv[4]);
  if (strlen(payload) > 10)
     {
      sequence_pos = strstr(payload, "[SEQUENCE]");
      if (sequence_pos)
         use_sequence = TRUE;
     }
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

  printf("<~>~<~>~<~>~<~>~<~><~>~<~>~<~>~<~>\n");
  if (use_sequence)
     printf (" Inject sequence: yes\n");
     else
     printf (" Inject sequence: no\n");
  printf(" Destination: %s:%i\n", saddress, sport);
  printf(" With payload: %s.... [%lu] bytes\n", payload_short, strlen(payload));
  printf(" Attempting %li packets per second.\n", packetspersec);
  printf(" Timeframe: %li\xC2\xB5s\n", timeframepersend);
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
  while (1)
    {
     // untill stopped
     packet_nr += 1;
     if (packet_nr > 0xffffffff) packet_nr = 0;
     if (use_sequence)
        {
         sprintf(sequence_str,"%08lX", packet_nr);
         for (i = 0 ; i < 8 ; i++)
             *(sequence_pos + i + 1) = sequence_str[i];
        }
     if (sendto(sockfd, payload, strlen(payload), 0, (const struct sockaddr*)&destaddr, sizeof(destaddr)) < 0)
        {
         fprintf(stderr, "Error in sendto()\n");
         return EXIT_FAILURE;
        }
     clock_gettime(CLOCK_MONOTONIC_RAW, &start);
     if (start.tv_sec - prt_timer > 0) //do something every second
        {
         prt_timer = start.tv_sec;
         // print running stats
         printf("...\n%08lX %05ld pps\n", packet_nr, (packet_nr - prev_packet_nr));
         // tune the timer
         if ((packet_nr - prev_packet_nr) < packetspersec)
            timeframepersend -= 1;
         if ((packet_nr - prev_packet_nr) > packetspersec)
            timeframepersend += 1;
         prev_packet_nr = packet_nr;
        }

     cycle = 0;
     do
       {
        // waste some time untill next packet is to be sent
        cycle ++;
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
       } while (delta_us < timeframepersend);

    if (cycle < 2)
       {
        printf("Sorry. Unable to run faster on this one socket; try reducing packets per second or launch multiple instances.\n");
        //return -1;
       }
  } // end while(1) loop
    return EXIT_SUCCESS;
}
