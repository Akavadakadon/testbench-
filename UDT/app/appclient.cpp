#ifndef WIN32
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wspiapi.h>
#endif
#include <iostream>
#include <udt.h>
#include "cc.h"
#include <chrono>
#include <fstream>
#include <cstdlib>
#include "test_util.h"

using namespace std;

#ifndef WIN32
void* monitor(void*);
#else
DWORD WINAPI monitor(LPVOID);
#endif

char* build_filename(char* size) {
    char* name;
    char* name1;
    char* name2;
    char* n1 = new char[20];
    name = strcpy(n1, (char*)"resp_");
    name1 = strcat(name, size);
    name2 = strcat(name1, (char*)"_b.txt\0");
    return name2;
}

std::string build_resfilename(char* delay, char* error, char* size) {
    char* name;
    char* name1;
    char* name2;
    char* n1 = new char[40];
    std::string res("");
    res = std::string() + "result/res_with_" + size + "_" + error + "_" + delay + "_b.txt\0";
    return res;
}

void printArgs(int argc, char* argv[])
{
    printf("Args:\n");
    for (int i = 0; i < argc; i++)
    {
        printf(argv[i]);
        printf("\n");
    }
    printf("\n");
}



char* file(int size)
{
    char* data;
    int bytes = (size);
    data = (char*)malloc(bytes);
    for (int i = 0; i < bytes; i++) {
        data[i] = (char)rand();
    }
    return data;
}

std::ofstream fout;

int main(int argc, char* argv[])
{
    if ((argc < 5) || (0 == atoi(argv[2])))
    {
        cout << "usage: [1]recvfile [2]server_ip [3]server_port [4]P2P_DELAY [5]ERROR_RATE [6]file_size [7]num_iters" << endl;
        return -1;
    }
    //printArgs(argc, argv);
    char* fileName = build_filename(argv[5]);
    printf(fileName);
    printf("\n");

    int num_iters = atoi(argv[6]);
    int size = atoi(argv[5]);

    std::string file_result_name = build_resfilename(argv[3], argv[4], argv[5]);
    cout << file_result_name << endl;
    fout.open(file_result_name);
    fout << "BYTES_COUNT\tP2P_DELAY\tERROR_RATE\tTIME\tTRetrans\tRetrans\tSndRate" << endl;//BYTES_COUNT	P2P_DELAY	ERROR_RATE	TIME

    // Automatically start up and clean up UDT module.
    UDTUpDown _udt_;

    struct addrinfo hints, * local, * peer;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    //hints.ai_socktype = SOCK_DGRAM;

    if (0 != getaddrinfo(NULL, "9000", &hints, &local))
    {
        cout << "incorrect network address.\n" << endl;
        return 0;
    }
    char* sisi = file(size);
    UDTSOCKET client = UDT::socket(local->ai_family, local->ai_socktype, local->ai_protocol);
    for (int i = 0; i < num_iters; i++)
    {

        // UDT Options
        //UDT::setsockopt(client, 0, UDT_CC, new CCCFactory<CUDPBlast>, sizeof(CCCFactory<CUDPBlast>));
        //UDT::setsockopt(client, 0, UDT_MSS, new int(9000), sizeof(int));
        //UDT::setsockopt(client, 0, UDT_SNDBUF, new int(10000000), sizeof(int));
        //UDT::setsockopt(client, 0, UDP_SNDBUF, new int(10000000), sizeof(int));
        //UDT::setsockopt(client, 0, UDT_MAXBW, new int64_t(12500000), sizeof(int));

        // Windows UDP issue
        // For better performance, modify HKLM\System\CurrentControlSet\Services\Afd\Parameters\FastSendDatagramThreshold
        client = UDT::socket(local->ai_family, local->ai_socktype, local->ai_protocol);//hints.ai_flags, hints.ai_socktype, hints.ai_protocol
#ifdef WIN32
        UDT::setsockopt(client, 0, UDT_MSS, new int(1052), sizeof(int));
#endif

        // for rendezvous connection, enable the code below
        /*
        UDT::setsockopt(client, 0, UDT_RENDEZVOUS, new bool(true), sizeof(bool));
        if (UDT::ERROR == UDT::bind(client, local->ai_addr, local->ai_addrlen))
        {
           cout << "bind: " << UDT::getlasterror().getErrorMessage() << endl;
           return 0;
        }
        */

        //freeaddrinfo(local);

        if (0 != getaddrinfo(argv[1], argv[2], &hints, &peer))
        {
            cout << "incorrect server/peer address. " << argv[1] << ":" << argv[2] << endl;
            return 0;
        }

        chrono::system_clock::time_point start = chrono::system_clock::now();
        // connect to the server, implict bind
        if (UDT::ERROR == UDT::connect(client, peer->ai_addr, peer->ai_addrlen))
        {
            cout << "connect: " << UDT::getlasterror().getErrorMessage() << endl;
            return 0;
        }

        //freeaddrinfo(peer);

        // using CC method
        //CUDPBlast* cchandle = NULL;
        //int temp;
        //UDT::getsockopt(client, 0, UDT_CC, &cchandle, &temp);
        //if (NULL != cchandle)
        //   cchandle->setRate(500);


        //chrono::system_clock::time_point intermed = chrono::system_clock::now();
#ifndef WIN32
        //pthread_create(new pthread_t, NULL, monitor, &client);
#else
        //CreateThread(NULL, 0, monitor, &client, 0, NULL);
#endif

        int snd;
        if (UDT::ERROR ==(snd =  UDT::send(client, sisi, size, 0)))
        {
            cout << "send:" << UDT::getlasterror().getErrorMessage() << endl;
            i--;
            break;
        }
        chrono::system_clock::time_point end = chrono::system_clock::now();
        if (!file_result_name.empty())
            fout << size << "\t" << argv[3] << "\t" << argv[4] << "\t" \
            << chrono::duration_cast<chrono::microseconds>(end - start).count();
        //cout << "send " << snd << endl;
        monitor(&client);
        UDT::close(client);
        //if (i == num_iters - 1)
            //monitor(&client);
    }
    fout.close();

    return 0;
}

#ifndef WIN32
void* monitor(void* s)
#else
DWORD WINAPI monitor(LPVOID s)
#endif
{
    UDTSOCKET u = *(UDTSOCKET*)s;

    UDT::TRACEINFO perf;

    //cout << "SendRate(Mb/s)\tRTT(ms)\tCWnd\tPktSndPeriod(us)\tRecvACK\tRecvNAK" << endl;
    int i = 0;
    while (i++!=1)
    {
#ifndef WIN32
        //sleep(1);
#else
        //Sleep(1000);
#endif

        if (UDT::ERROR == UDT::perfmon(u, &perf))
        {
            cout << "perfmon: " << UDT::getlasterror().getErrorMessage() << endl;
            break;
        }

        fout << perf.pktRetransTotal << "\t"
            << perf.pktRetrans << "\t"
            << perf.mbpsBandwidth /*<< "\t"
            << perf.usPktSndPeriod << "\t"
            << perf.pktRetrans << "\t"
            << perf.pktSent << "\t"
            << perf.pktRecvNAK */<< endl;
    }

#ifndef WIN32
    return NULL;
#else
    return 0;
#endif
}