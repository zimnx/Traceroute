// Maciej Zimnoch - 248104
// Sieci Komputerowe - Pracownia 1
#include "Pakiet.h"

bool Pakiet::isFull() 
{ 
    return came == 3; 
}

Pakiet::Pakiet()
{
    Pakiet(0);
}
Pakiet::Pakiet(int t) :ip(""), time(0), ttl(t), came(0) {}

void Pakiet::setIP(string ipaddress)
{
    if(ip.empty())
    {
        ip = ipaddress;
    }
    else if( ip != ipaddress)
    {
        ip += " " + ipaddress;
    }
}
void Pakiet::signNew(double usec)
{
    came++;
    time += usec;
}
void Pakiet::print()
{
    printf("%d. %s ",ttl,ip.c_str());
    if(isFull())
        printf("%4.0lfms",(time/3));
    else
        printf("???");
    printf("\n");
}
bool Pakiet::isFilled()
{
  return came > 0;
}