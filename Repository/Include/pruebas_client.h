#ifndef PRUEBAS_CLIENT_H_
#define PRUEBAS_CLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>
#include <VimbaC/Include/VimbaC.h>
#include "../include/Bitmap.h"

#define CLIENT_PORT                 28000
#define SERVER_PORT                 8080
#define PROCESSOR_PORT              26000
#define UDP_PACKET_SIZE				49152
#define MAX_BUFFER_SIZE_OPT			1024
#define MAX_BUFFER_SIZE_FRAME		4004572

/**
 * @brief Get the set of images with all the possible combinations of HDR configurations from the camera handler and stores them in .bmp format
 * @param client_socket UDP socket created to comunicate with the camera handler
 * @return 
*/
VmbError_t HDR_2KP_AUTO_ALL(SOCKET* client_socket);

/**
 * @brief Get the set of images with all the possible combinations of white balance configurations from the camera handler and stores them in .bmp format
 * @param client_socket UDP socket created to comunicate with the camera handler
 * @return Error code
*/
VmbError_t WhiteBalance_Scan(SOCKET* client_socket);

#endif

