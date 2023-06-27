#ifndef PRINT_FEATURES
#define PRINT_FEATURES

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>
#include <VimbaC/Include/VimbaC.h>
#include "../include/Bitmap.h"

#define PORT 8080
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
 * @brief Displays in command line the information about the features stored in the FeatureList object
 * @param client_socket UDP socket created to comunicate with the camera handler
 * @param nFeatures The amount of configuration parameters/features that are sent from the camera
 * @param FeatureList The List in which the features are stored
 * @return 
*/
VmbError_t PrintFeatures(SOCKET* client_socket, VmbUint32_t nFeatures, Feature* FeatureList);

#endif
