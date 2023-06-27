#ifndef GET_FEATURES
#define GET_FEATURES

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

#define UDP_PACKET_SIZE 49152
#define MAX_BUFFER_SIZE_OPT 1024
#define MAX_BUFFER_SIZE_FRAME 4004572

#define MAX_NAME_LEN	1024

typedef struct
{
    char Name[MAX_NAME_LEN];
    char Description[MAX_NAME_LEN];
    char Value[MAX_NAME_LEN];
} Feature;


/**
 * @brief Sends the command of getting camera configuration information, recieves it and stores it in a FeatureList
 * @param client_socket UDP socket created to comunicate with the camera handler
 * @param nFeatures The amount of configuration parameters/features that are sent from the camera
 * @param FeatureList The List in which the features are stored
 * @return 
*/
VmbError_t GetFeatures(SOCKET* client_socket, VmbUint32_t* nFeatures, Feature** FeatureList);

#endif

