#include <string>
#include <cstdio>
using namespace std;

class Pakiet
{
    string ip;
    double time;
public:
    int ttl;
    int came;
    bool isFull(); 
    Pakiet();
    Pakiet(int t);
    void setIP(string ipaddress);
    void signNew(double usec);
    void print();
    bool isFilled();
};