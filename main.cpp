// Maciej Zimnoch - 248104
// Sieci Komputerowe - Pracownia 1


#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netinet/ip_icmp.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include "sockwrap.h"
#include "icmp.h"
#include "Pakiet.h"
using namespace std;

int maxhops = 30;
unsigned char 	buffer[IP_MAXPACKET+1];
unsigned char* 	buffer_ptr;
Pakiet* packets;
int pid;

void shift_bytes (int count);
void sendIcmpPacket(sockaddr_in &remote_address, int sockfd, int ttl);
void processPacket(int seq,string ip_address,timeval* timers,int i);

int main (int argc, char** argv)
{
	if (argc != 2) { printf ("Usage: ./sieci <ip>\n"); exit(1); }

	int sockfd = Socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

	sockaddr_in remote_address;
	bzero (&remote_address, sizeof(remote_address));
	remote_address.sin_family	= AF_INET;
	inet_pton(AF_INET, argv[1], &remote_address.sin_addr);
	
	packets = new Pakiet[maxhops + 1]();
	for(int i = 0 ; i < maxhops; ++i)
	{
		packets[i] = Pakiet(i);
	}
	pid = getpid();
	
	int ttl = 1;
	while(ttl <= maxhops)
	{
		timeval timers[3];
		for(int i = 0 ; i < 3 ; ++i)
		{
			timeval tim;
			sendIcmpPacket(remote_address,sockfd,ttl);
			gettimeofday(&tim,NULL);
			timers[i] = tim;
		}
		
		int i = 0;
		
		while(1)
		{
			sockaddr_in client_address;
			socklen_t len = sizeof(client_address);

			fd_set descriptors;
			
			FD_ZERO(&descriptors);
			FD_SET(sockfd,&descriptors);	
			timeval timeout; timeout.tv_sec = 1; timeout.tv_usec = 0;
			int ready = Select(sockfd+1, &descriptors, NULL, NULL, &timeout);
			if(ready)
			{
				int n = Recvfrom (sockfd, buffer, IP_MAXPACKET+1, 0, &client_address, &len);
				if(n == 0) break;
				buffer_ptr = buffer;
				char ip_address[20];
				
				
				ip* ip_packet = (ip*) buffer_ptr;
				shift_bytes(ip_packet->ip_hl * 4);

				icmp* icmp_packet = (icmp*) buffer_ptr;
				shift_bytes(ICMP_HEADER_LEN);
				
				if(icmp_packet->icmp_type == ICMP_TIME_EXCEEDED &&
				   icmp_packet->icmp_code == ICMP_EXC_TTL)
				{
					ip* packet_orig = (ip*) buffer_ptr;
					shift_bytes(packet_orig->ip_hl *4);

					if(packet_orig->ip_p == IPPROTO_ICMP)
					{
						icmp_packet = (icmp*) buffer_ptr;

						char ip_address[20];
						inet_ntop (AF_INET, &client_address.sin_addr, ip_address, sizeof(ip_address));

						if(icmp_packet->icmp_id == pid)
						{
							if(ttl == icmp_packet->icmp_seq)
							{
								
								processPacket(icmp_packet->icmp_seq,ip_address,timers,i);
								Pakiet pac = packets[icmp_packet->icmp_seq];
								if(pac.isFull())
								{	
									pac.print();
									break;
								}
							}

							i++;
							
						}
						else
						{
							continue;
						}
						
					}
				}

				else if(icmp_packet->icmp_type == ICMP_ECHOREPLY && icmp_packet->icmp_id == pid)
				{
				  
					inet_ntop (AF_INET, &client_address.sin_addr, ip_address, sizeof(ip_address));
					ip* packet_orig = (ip*) buffer_ptr;
					shift_bytes(packet_orig->ip_hl *4);
					
					processPacket(icmp_packet->icmp_seq,ip_address,timers,i);
					Pakiet pac = packets[icmp_packet->icmp_seq];
					if(pac.isFull())
					{	
						pac.print();
						exit(0);	
					}
					i++;
					
				}
				
			}
			else
			{ 
			  if(packets[ttl].isFilled())
			  {
			    packets[ttl].print();
			    break;
			  }
			  printf("%d. *\n",ttl);
				  break;
				
			}
		}
		ttl++;

	}
	return 0;
}

void sendIcmpPacket(sockaddr_in &remote_address, int sockfd, int ttl)
{
	icmp icmp_packet;
	icmp_packet.icmp_type = ICMP_ECHO;
	icmp_packet.icmp_code = 0;
	icmp_packet.icmp_id = pid;
	icmp_packet.icmp_seq = ttl;
	icmp_packet.icmp_cksum = 0;
	icmp_packet.icmp_cksum = in_cksum((short int*)&icmp_packet, 8, 0);
	Setsockopt (sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
	Sendto(sockfd, &icmp_packet, ICMP_HEADER_LEN, 0, &remote_address, sizeof(remote_address));
}
void processPacket(int seq,string ip_address,timeval* timers,int i)
{
	Pakiet pac = packets[seq];
	pac.setIP(ip_address);
	timeval tim;
	gettimeofday(&tim,NULL);
	double elapsedTime = (tim.tv_sec - timers[i].tv_sec) * 1000.0;
	elapsedTime += (tim.tv_usec - timers[i].tv_usec) / 1000.0;
	pac.signNew(elapsedTime);
	packets[seq] = pac;
}
void shift_bytes(int count)
{
	buffer_ptr += count;
}