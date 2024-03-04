
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <codecvt> 
#include <iostream>
#include <vector>
#include <fstream>
#include <thread>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <string>
#include <locale>
#include <opencv2/opencv.hpp>

#define FILEPORT "48484"
#define DATAPORT "58484"
#define BUFSIZE 16

std::string t()
{
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);

    std::tm now_tm;
    localtime_s(&now_tm, &now_c);

    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");
    std::string formattedDate = oss.str();  // 将格式化的时间转换为std::string
    return formattedDate;
}

void p(const char* str)
{
    using namespace std;
    cout << str << endl;
}

bool file_serverRun = true;
bool text_serverRun = true;

int file_server()
{
    while (file_serverRun)
    {
        /*cout << "start now?" << endl;
        int startcode;
        cin >> startcode;
        cout << "start" << endl;*/
        WSADATA wsaData;
        int iResult;

        SOCKET ListenSocket = INVALID_SOCKET;
        SOCKET ClientSocket = INVALID_SOCKET;

        struct addrinfo* result = NULL;
        struct addrinfo hints;

        int recvbuflen = BUFSIZE;
        char recvbuf[BUFSIZE];

        // 初始化Winsock
        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            printf("WSAStartup failed: %d\n", iResult);
            return 1;
        }

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET; // 使用IPv4地址
        hints.ai_socktype = SOCK_STREAM; // 指定流式套接字
        hints.ai_protocol = IPPROTO_TCP; // 指定TCP协议
        hints.ai_flags = AI_PASSIVE; // 调用者将在bind()函数中使用返回的地址

        // 解析本地地址和端口以供监听
        iResult = getaddrinfo(NULL, FILEPORT, &hints, &result);
        if (iResult != 0) {
            printf("getaddrinfo failed: %d\n", iResult);
            WSACleanup();
            return 1;
        }

        // 创建监听socket
        ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (ListenSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            return 1;
        }

        Sleep(10);
        // 绑定监听socket
        iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            printf("bind failed with error: %d\n", WSAGetLastError());
            freeaddrinfo(result);
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }


        freeaddrinfo(result);

        // 开始监听连接
        iResult = listen(ListenSocket, SOMAXCONN);
        if (iResult == SOCKET_ERROR) {
            printf("listen failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }

        p("文件端等待数据");

        // 接受客户端连接
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }

        

        // 接收数据直到客户端关闭连接
        using namespace std;
        using namespace cv;
        ofstream outtxt("lastget.jpg", ios::out | ios::binary);
        do {
            iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
            if (iResult > 0) {
                for (size_t i = 0; i < sizeof(recvbuf); i++) {
                    char c = recvbuf[i];
                    outtxt.write(&c, sizeof(c));
                }
            }
            else if (iResult == 0)
            {
                outtxt.close();
                Mat fileimage = imread("lastget.jpg");
                if (!fileimage.empty())
                {
                    UMat showimage;
                    resize(fileimage, showimage, Size(fileimage.cols / 4, fileimage.rows / 4), 0, 0, INTER_LINEAR);
                    imshow("lastimage", showimage);
                    waitKey(0);
                    destroyAllWindows();
                }
                printf("\n接收完毕\n");
            }
            else printf("\n接收失败\n");

        } while (iResult > 0);
        
        

        // 关闭
        /*int closecode = 0;
        do
        {
            closecode = closesocket(ClientSocket);
        } while (closecode != 0);
        closecode = 0;
        do
        {
            closecode = WSACleanup();
        } while (closecode != 0);*/
        closesocket(ClientSocket);
        closesocket(ListenSocket);
        WSACleanup();
    }
    return 0;
}

int text_server()
{
    while (text_serverRun)
    {
        WSADATA wsaData;
        int iResult;

        SOCKET ListenSocket = INVALID_SOCKET;
        SOCKET ClientSocket = INVALID_SOCKET;

        struct addrinfo* result = NULL;
        struct addrinfo hints;

        int recvbuflen = 1;
        char recvbuf[1];

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET; // 使用IPv4地址
        hints.ai_socktype = SOCK_STREAM; // 指定流式套接字
        hints.ai_protocol = IPPROTO_TCP; // 指定TCP协议
        hints.ai_flags = AI_PASSIVE; // 调用者将在bind()函数中使用返回的地址

        // 初始化Winsock
        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            printf("WSAStartup failed: %d\n", iResult);
            return 1;
        }

        // 解析本地地址和端口以供监听
        iResult = getaddrinfo(NULL, DATAPORT, &hints, &result);
        if (iResult != 0) {
            printf("getaddrinfo failed: %d\n", iResult);
            WSACleanup();
            return 1;
        }

        // 创建监听socket
        ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (ListenSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            closesocket(ClientSocket);
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }


        // Pay attention here, The original plan was to continue bind while notepad was closed, but it backfired.
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        bool notepad = false;
        const char* notepadTitle = "lastget.txt - Notepad";
        do
        {
            HWND hWnd = FindWindowA(NULL, notepadTitle);
            if (hWnd != NULL) notepad = true;
            else notepad = false;
            Sleep(10);
        } while (notepad);
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;




        // 绑定监听socket
        iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            printf("bind failed with error: %d\n", WSAGetLastError());
            freeaddrinfo(result);
            closesocket(ClientSocket);
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }

        freeaddrinfo(result);

        // 开始监听连接
        iResult = listen(ListenSocket, SOMAXCONN);
        if (iResult == SOCKET_ERROR) {
            printf("listen failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }
        p("数据端等待数据");

        // 接受客户端连接
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }


        // 接收数据直到客户端关闭连接
        using namespace std;
        ofstream ofile("lastget.txt", ios::binary);
        do {
            iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
            if (iResult > 0) {
                char c = recvbuf[0];
                ofile.write(&c, 1);
            }
            else if (iResult == 0)
            {
                ofile.close();
                // Pay attention here, The original plan was to continue bind while notepad was closed, but it backfired.
                ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
                system("start \"\" \"C:\\Windows\\System32\\notepad.exe\" \"lastget.txt\" ");
                ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
                printf("\n接收完毕\n");
            }
            else printf("\n接收失败\n");

        } while (iResult > 0);
        

        closesocket(ClientSocket);
        closesocket(ListenSocket);
        WSACleanup();
        
    }
    return 0;
}

int main()
{
    using namespace std;
    cout << "数据端 与 文件端 已独立执行" << endl;
    Sleep(10);
    thread fs(file_server);
    Sleep(10);
    thread ds(text_server);
    fs.detach();
    ds.detach();
    char lastget = getchar();
    file_serverRun = false;
    text_serverRun = false;

    return 0;
    
}
