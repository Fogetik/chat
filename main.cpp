#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <wininet.h>
#include <thread>
#include <string>
#include <inaddr.h>
#include <unistd.h>


using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::thread;

void chat(string nick, string ip, bool ch);

#pragma comment(lib, "Ws2_32.lib")

int main() {
    string nickname = "f";
//    cout << "Your nickname: ";
//    cin >> nickname;
    //chat(nickname, true);
    string ip_server = "127.0.0.1";
    string ip_client = "127.0.0.1";
    thread A(chat, nickname, ip_server, true);
    sleep(1);
    thread B(chat, nickname, ip_client, false);
    A.join();
    B.join();
    return 0;
}

WSADATA w;

void chat(string nick, string ip, bool ch){

    int error = WSAStartup (0x0202, &w); // Fill in WSA info
    if (error)
    {
        return; //По какой-то причине мы не смогли запустить Winsock
    }

    if (w.wVersion != 0x0202) {// Неправильная версия Winsock?
        WSACleanup ();
        return;
    }

    int port = 27015;
    if (ch){
//        SOCKET socket_server = socket(AF_INET, SOCK_STREAM, 0);
        SOCKET socket_server;
        socket_server = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_server == INVALID_SOCKET){
            cout << "error socket" << endl;
            return;
        }

        SOCKADDR_IN addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        int receive = bind(socket_server, (SOCKADDR*)&addr, sizeof(addr));
        if (receive == -1){
            cout << "bind error" << endl;
            return;
        }

        receive = listen(socket_server, 5);
        cout << "point1" << endl;
        SOCKET socket_rc = accept(socket_server, nullptr, nullptr);
        if (socket_rc == INVALID_SOCKET){
            cout << "error socket" << endl;
            return;
        }

        char *buf = new char[15];
//        char *buf;
//        string line;
        cout << "point2" << endl;
//        getchar();
        cout << "point3" << endl;
        int f = recv(socket_rc, buf, 15, 0);

        if ( f > 0 )
            printf("Bytes received: %d\n", f);
        else if ( f == 0 )
            printf("Connection closed\n");
        else
            printf("recv failed: %d\n", WSAGetLastError());

        cout << "have a message: " << buf << endl;
        shutdown(socket_server, 0);
        closesocket(socket_server);

    }else{
//        SOCKET socket_client = socket(AF_INET, SOCK_STREAM, 0);
        SOCKET socket_client;
        socket_client = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_client == INVALID_SOCKET){
            cout << "error socket" << endl;
            return;
        }

        SOCKADDR_IN addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        int receive = connect(socket_client, (SOCKADDR*)&addr, sizeof(addr));
        if (receive == SOCKET_ERROR){
            cout << "error connect" << endl;
            return;
        }

//        char *buf1 = "a";
//        cout << &buf1 << endl;
        sleep(1);
        receive = send(socket_client, "hello", 15, 0);
        if (receive == SOCKET_ERROR) {
            cout << "error send" << endl;
            closesocket(socket_client);
            WSACleanup();
            return;
        }
        sleep(1);
        cout << "h1" << endl;
//        getchar();
        sleep(3);
        shutdown(socket_client, 0);
        closesocket(socket_client);
    }
}
