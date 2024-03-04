#include <jni.h>

// cpp
#include <iostream>
#include <string>

// c
#include <stdio.h>
#include <stdlib.h>

// net
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFSIZE 16

extern "C" JNIEXPORT jint
JNICALL Java_com_oiwiterio_FlowSheet_1NR_MainActivity_FileSend(
        JNIEnv *env,
        jobject /*none*/,
        jstring jip,
        jstring jport,
        jstring jfile)
{
    using namespace std;

    const char* cip = env->GetStringUTFChars(jip, NULL);
    const char* cport = env->GetStringUTFChars(jport, NULL);
    const char* cfile = env->GetStringUTFChars(jfile, NULL);

    if (cip == NULL || cport == NULL || cfile == NULL) return -1;

    string ip(cip);
    string port(cport);
    string file(cfile);

    env->ReleaseStringUTFChars(jip, cip); // 释放资源
    env->ReleaseStringUTFChars(jport, cport); // 释放资源
    env->ReleaseStringUTFChars(jfile, cfile); // 释放资源

    // string 阵列蓄势待发...

    // 服务端结构体
    struct sockaddr_in serv_addr;
    struct timeval tv;

    int sock = 0;
    char buffer[BUFSIZE] = {0};
    FILE *fp;

    tv.tv_sec = 0,tv.tv_usec = 500000; // 500 ms

    // 创建socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) return -1;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(stoi(port));

    //  IPv4转换为二进制并放入&serv_addr.sin_addr
    if(inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr)<=0) return -1;

    // 设置发送与接受超时
    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tv, sizeof(tv)) < 0) return -1;

    // 连接服务端
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) return -1;

    // 打开文件
    fp = fopen(file.c_str(), "rb");
    if (fp == NULL) return -1;

    // 读取兼发送文件
    size_t bytesRead; // 字节
    while ((bytesRead = fread(buffer, 1, BUFSIZE, fp)) > 0)
    {
        send(sock, buffer, bytesRead, 0);
    }

    fclose(fp);
    close(sock);

    return 0;
}

extern "C" JNIEXPORT jint
JNICALL Java_com_oiwiterio_FlowSheet_1NR_MainActivity_DataSend(
        JNIEnv *env,
        jobject /*none*/,
        jstring jip,
        jstring jport,
        jstring jdata)
{
    using namespace std;

    const char* cip = env->GetStringUTFChars(jip, NULL);
    const char* cport = env->GetStringUTFChars(jport, NULL);
    const char* cdata = env->GetStringUTFChars(jdata, NULL);

    if (cip == NULL || cport == NULL || cdata == NULL) return -1;

    string ip(cip);
    string port(cport);
    string data(cdata);

    env->ReleaseStringUTFChars(jip, cip); // 释放资源
    env->ReleaseStringUTFChars(jport, cport); // 释放资源
    env->ReleaseStringUTFChars(jdata, cdata); // 释放资源

    // string 阵列蓄势待发...

    // 服务端结构体
    struct sockaddr_in serv_addr;
    struct timeval tv;

    int sock = 1;

    tv.tv_sec = 0,tv.tv_usec = 500000; // 500 ms

    // 创建socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) return -1;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(stoi(port));

    //  IPv4转换为二进制并放入&serv_addr.sin_addr
    if(inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr)<=0) return -1;

    // 设置发送与接受超时
    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tv, sizeof(tv)) < 0) return -1;

    // 连接服务端
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) return -1;

    const char *send_data = data.c_str();

    // 发送数据
    for (int i = 0; i < strlen(send_data); ++i) {
        send(sock, &send_data[i], 1, 0);
    }

    close(sock);

    return 0;
}