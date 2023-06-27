#ifndef VIDEO_STREAMING
#define VIDEO_STREAMING

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
 * @brief Sends the command of starting video streaming to the camera handler, start the capture acquisistion and displays the avarage FPS every frame captured
 * @param client_socket client_socket UDP socket created to comunicate with the camera handler
 * @return 0 Error code
*/
VmbError_t VideoStreaming(SOCKET* client_socket);

#endif
