#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <inaddr.h>
#include <unistd.h>

using std::cin;
using std::cout;
using std::endl;
using std::string;

void send_messages(int port, const string& ip, string nickname);

void accept_messages(int port, const string& ip);


#pragma comment(lib, "Ws2_32.lib")

WSADATA w;

int main(int argc, char *argv[]) {
    int error = WSAStartup (0x0202, &w); // Fill in WSA info
    if (error)
    {
        return 1; //По какой-то причине мы не смогли запустить Winsock
    }

    if (w.wVersion != 0x0202) {// Неправильная версия Winsock?
        WSACleanup ();
        return 1;
    }
//    string ip_server = "192.168.0.28";
//    string ip_client = "192.168.0.204";
    string ip_server = "127.0.0.1";
    string ip_client = "127.0.0.1";
    string nickname;
    int port;

    if(argc == 3){
        ip_server = string(argv[1]);
        port = atoi(argv[2]);
        accept_messages(port, ip_server);
        return 0;

    }
    else if( argc == 4 )
    {
        ip_client = string(argv[1]);
        port = atoi(argv[2]);
        nickname = string(argv[3]);
        send_messages(port, ip_client, nickname);
        return 0;
    }


    cout << "Your nickname:";
    cin >> nickname;
    cout << "Enter number of port:";
    cin >> port;
    cout << "-------------------------------------------" << endl << endl;
    cout << "-------------------------------------------" << endl;


    STARTUPINFO si1, si2;
    PROCESS_INFORMATION pi1, pi2;

    ZeroMemory(&si1, sizeof(si1));
    si1.cb = sizeof(si1);
    ZeroMemory(&pi1, sizeof(pi1));

    ZeroMemory(&si2, sizeof(si2));
    si2.cb = sizeof(si2);
    ZeroMemory(&pi2, sizeof(pi2));


    string line1 = argv[0];
    line1 = line1 + " " + ip_client + " " + std::to_string(port) + " " + nickname;
    string line2 = argv[0];
    line2 = line2 + " " + ip_server + " " + std::to_string(port);
    //create process for sending messages
    if( !CreateProcess( nullptr,   // No module name (use command line1)
                        const_cast<char*>( line1.c_str()),        // Command line1
                        nullptr,           // Process handle not inheritable
                        nullptr,           // Thread handle not inheritable
                        FALSE,          // Set handle inheritance to FALSE
                        0,              // No creation flags
                        nullptr,           // Use parent's environment block
                        nullptr,           // Use parent's starting directory
                        &si1,            // Pointer to STARTUPINFO structure
                        &pi1 )           // Pointer to PROCESS_INFORMATION structure
            )
    {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return 1;
    }

    //create process for accepting messages
    if( !CreateProcess( nullptr,   // No module name (use command line1)
                        const_cast<char*>( line2.c_str()),        // Command line1
                        nullptr,           // Process handle not inheritable
                        nullptr,           // Thread handle not inheritable
                        FALSE,          // Set handle inheritance to FALSE
                        0,              // No creation flags
                        nullptr,           // Use parent's environment block
                        nullptr,           // Use parent's starting directory
                        &si2,            // Pointer to STARTUPINFO structure
                        &pi2 )           // Pointer to PROCESS_INFORMATION structure
            )
    {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return 1;
    }

    HANDLE process_array[2];
    process_array[0] = pi1.hProcess;
    process_array[1] = pi2.hProcess;
    WaitForMultipleObjects(2, process_array, FALSE, INFINITE);

    TerminateProcess(pi1.hProcess, 0);
    TerminateProcess(pi2.hProcess, 0);
    return 0;
}

void send_messages(int port, const string& ip, string nickname){
    SOCKET socket_client;
    socket_client = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_client == INVALID_SOCKET){
        cout << "error socket1" << endl;
        return;
    }

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    int receive = connect(socket_client, (SOCKADDR*)&addr, sizeof(addr));
    if (receive == SOCKET_ERROR){
        cout << "error connect" << endl;
        return;
    }

    string message;
    bool end_chat = false;
    nickname += ":";
    cout << "------------ start of the chat ------------" << endl;
    while(true){
        message = "";
        std::getline(cin, message);
        if (message == "!exit")
            end_chat = true;

        message.insert(0, nickname);
        receive = send(socket_client, message.c_str(), (int)(message.length()+1), 0);
        if (receive == SOCKET_ERROR) {
            cout << "error send" << endl;
            closesocket(socket_client);
            WSACleanup();
            return;
        }

        if (end_chat)
            break;

        sleep(1);

    }
    cout << "------------ end of the chat ------------" << endl;

    shutdown(socket_client, 0);
    closesocket(socket_client);
}

void accept_messages(int port, const string& ip){
    SOCKET socket_server;
    socket_server = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_server == INVALID_SOCKET){
        cout << "error socket" << endl;
        return;
    }

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    int receive = bind(socket_server, (SOCKADDR*)&addr, sizeof(addr));
    if (receive == -1){
        cout << "bind error" << endl;
        return;
    }

    receive = listen(socket_server, 5);

    SOCKET socket_rc = accept(socket_server, nullptr, nullptr);
    if (socket_rc == INVALID_SOCKET){
        cout << "error socket" << endl;
        return;
    }

    char *buf;
    int f;
    bool end_chat = false;
    string line;
    while(1) {
        buf = new char[1024];
        f = recv(socket_rc, buf, 1024, 0);
        if (f == 0)
            break;
        else if (f < 0)
            printf("recv failed: %d\n", WSAGetLastError());

        for (int i = 0; buf[i] != '\0'; i++){
            if (buf[i] == ':'){
                if (buf[i+1] == '!' && buf[i+2] == 'e' && buf[i+3] == 'x' && buf[i+4] == 'i'
                    && buf[i+5] == 't') {
                    end_chat = true;
                }

                break;
            }
        }

        if (end_chat)
            break;

        cout << buf << endl;
    }
    cout << "------------ end of the chat ------------" << endl;
    shutdown(socket_server, 0);
    closesocket(socket_server);

}
