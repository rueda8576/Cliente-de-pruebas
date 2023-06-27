#ifndef GET_FRAME
#define GET_FRAME

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
#define PIXEL_FORMAT                "BGR8Packed"

/**
 * @brief Sends the command of capturing context image to the camera handler, recieves it and saves it in .bmp format
 * @param client_socket UDP socket created to comunicate with the camera handler
 * @return Error code
*/
VmbError_t Get_Frame(SOCKET* client_socket);

#endif
