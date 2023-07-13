#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>    //Sleep()
#include <math.h>       //trunc()
#include <time.h>

#include "../include/Bitmap.h"
#include "../include/pruebas_client.h"
#include "../include/f_client.h"
#include "../include/Get_Frame.h"
#include "../include/VideoStreaming.h"
#include <VimbaC/Include/VimbaC.h>

#pragma comment(lib, "ws2_32.lib")

typedef int bool;
#define false 0
#define true 1

#define CLIENT_PORT                 28000
#define SERVER_PORT                 8080
#define PROCESSOR_PORT              26000

#define MAX_BUFFER_SIZE_OPT         1024
#define MAX_BUFFER_SIZE_FRAME       4004572

#define UDP_PACKET_SIZE 49152

#define NUM_FRAMES      15
#define NUM_FRAMES_HDR  3072

#define MAX_NAME_LEN	1024

typedef struct
{
    char Name[MAX_NAME_LEN];
    char Description[MAX_NAME_LEN];
    char Value[MAX_NAME_LEN];
} Feature;

int main(int argc, char *argv[])
{

    int                 elapsed_time        = 0;
    VmbError_t          error               = VmbErrorSuccess;

    //Features parameters
    VmbUint32_t         nFeatures           = 0;                            // The amount of features
    Feature*            FeatureList         = NULL;                         // A list of features

    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    int total_bytes_recv                    = 0;
    int bytes_recv;
    int bytes_sent;
    char sent_buffer[MAX_BUFFER_SIZE_OPT]   = {0};
    int socket_buffer_size                  = UDP_PACKET_SIZE;
    int server_address_len                  = sizeof(server_address);
    int client_address_len                  = sizeof(client_address);

    int  opcion;
    char confirmation[MAX_BUFFER_SIZE_OPT]  = { 0 };

    char s_ExposureTimeAbs[1024]            = { 0 };
    char s_Gain[1024]                       = { 0 };
    char s_TH1[1024]                        = { 0 };
    char s_EXP1[1024]                       = { 0 };

    UINT32 bufferSize                       = 0;
    UINT32 imageSize                        = 0;
    UINT32 width                            = 0;
    UINT32 height                           = 0;
    double ExposureTimeAbs                  = 0;
    double Gain                             = 0;

    int num_packets;

    char message[MAX_BUFFER_SIZE_OPT];

    int num_recv_packets                    = 0;

    int i;

    const char* pFileName                   = "../../Images/image.bmp";             // The path and name of the file wanted to create
    AVTBitmap bitmap;                                                               // The bitmap to create

    char* recv_buffer = (char*)malloc(MAX_BUFFER_SIZE_FRAME * sizeof(char));
    if (recv_buffer == NULL) {
        perror("Failed to allocate memory");
        return 1;
    }

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup failed\n");
        free(recv_buffer);
        return 1;
    }

    // Create socket
    if ((client_socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket failed");
        free(recv_buffer);
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(client_socket, SOL_SOCKET, SO_RCVBUF, (char *) &socket_buffer_size, sizeof(socket_buffer_size)))
    {
        perror("setsockopt");
        free(recv_buffer);
        exit(EXIT_FAILURE);
    }

    // Set server address
    //192.168.1.20
    server_address.sin_addr.s_addr          = inet_addr("192.168.10.13"); // Server IP (Raspberry Pi)
    server_address.sin_family               = PF_INET;
    server_address.sin_port                 = htons(SERVER_PORT);

    client_address.sin_family               = PF_INET;
    client_address.sin_addr.s_addr          = htonl(INADDR_ANY);
    client_address.sin_port                 = htons(CLIENT_PORT);

    // Bind server socket to port and listen any address
    if (bind(client_socket, (struct sockaddr*)&client_address, sizeof(struct sockaddr_in)) != 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    opcion = menu_client();
    while (true)
    {   
        //Send opcion to server
        sprintf(sent_buffer, "%d", opcion);
        error = sendto(client_socket, sent_buffer, strlen(sent_buffer), 0, (struct sockaddr*)&server_address, sizeof(server_address));
        if (error == SOCKET_ERROR)
        {
            perror("Option sending failed\n");
            free(recv_buffer);
            exit(EXIT_FAILURE);
        }

        if (opcion == 1)
        {
            error = Get_Frame(client_socket);
            if (error != VmbErrorSuccess)
            {
                perror("Failed to get frame");
                return 1;
            }
        }

        if (opcion == 2)
        {
            error = VideoStreaming(client_socket);
            if (error != VmbErrorSuccess)
            {
                perror("Failed to get Video Streaming");
                return 1;
            }
        }

        if (opcion == 3)
        {
            error = GetFeatures(client_socket, &nFeatures, &FeatureList);
            if (error != VmbErrorSuccess)
            {
                perror("Failed to get features of the camera");
                return 1;
            }
        }

        if (opcion == 4)
        {
            error = PrintFeatures(client_socket, nFeatures, FeatureList);
            if (error != VmbErrorSuccess)
            {
                perror("Failed to print features of the camera");
                return 1;
            }
        }
        
        if (opcion == 5)
        {
            error = HDR_2KP_AUTO_ALL(client_socket);
            if (error != VmbErrorSuccess)
            {
                perror("Failed to get HDR frames with 2 KP");
                return 1;
            }
        }

        if (opcion == 6)
        {
            error = WhiteBalance_Scan(client_socket);
            if (error != VmbErrorSuccess)
            {
                perror("Failed to RedScan");
                return 1;
            }
        }

        if (opcion == 0)
        {
            // Close socket
            closesocket(client_socket);
            WSACleanup();
            system("cls");
            free(recv_buffer);
            free(FeatureList);
            break;
        }
        getchar();
        opcion = menu_client();
    }
    return error;
}