#include "../include/GetFeatures.h"

VmbError_t GetFeatures(SOCKET* client_socket, VmbUint32_t* nFeatures, Feature** FeatureList)
{
    VmbError_t  error               = VmbErrorSuccess;
    int         bytes_recv;
    char        s_nFeatures[10]     = { 0 };
    VmbUint32_t i                   = 0;

    struct sockaddr_in server_address;

    server_address.sin_addr.s_addr  = inet_addr("192.168.10.13");     // Server IP (Raspberry Pi)
    server_address.sin_family       = PF_INET;
    server_address.sin_port         = htons(SERVER_PORT);
    int server_address_len          = sizeof(server_address);

    //Get number of features from server
    bytes_recv = 0;
    bytes_recv = recvfrom(client_socket, s_nFeatures, sizeof(s_nFeatures), 0, (struct sockaddr*)&server_address, &server_address_len);
    if (bytes_recv == -1)
    {
        perror("Error getting the amount of features\n");
        exit(EXIT_FAILURE);
    }
    *nFeatures = atoi(s_nFeatures);

    //Para simplificar codigo se declara nCount para uso local
    int nCount = *nFeatures;
    printf("nFeatures = %d\n", nCount);

    //Allocate memory for features list
    *FeatureList = malloc(nCount * sizeof(Feature));

    //Get features from server
    for (i = 0; i < nCount; ++i)
    {
        bytes_recv = 0;
        bytes_recv = recvfrom(client_socket, (*FeatureList)[i].Name, MAX_NAME_LEN, 0, (struct sockaddr*)&server_address, &server_address_len);
        if (bytes_recv == -1)
        {
            perror("Error getting the name of features\n");
            exit(EXIT_FAILURE);
        }
        (*FeatureList)[i].Name[bytes_recv] = '\0'; // Null-terminate the string

        bytes_recv = 0;
        bytes_recv = recvfrom(client_socket, (*FeatureList)[i].Description, MAX_NAME_LEN, 0, (struct sockaddr*)&server_address, &server_address_len);
        if (bytes_recv == -1)
        {
            perror("Error getting the description of features\n");
            exit(EXIT_FAILURE);
        }
        (*FeatureList)[i].Description[bytes_recv] = '\0'; // Null-terminate the string

        bytes_recv = 0;
        bytes_recv = recvfrom(client_socket, (*FeatureList)[i].Value, MAX_NAME_LEN, 0, (struct sockaddr*)&server_address, &server_address_len);
        if (bytes_recv == -1)
        {
            perror("Error getting the value of features\n");
            exit(EXIT_FAILURE);
        }
        (*FeatureList)[i].Value[bytes_recv] = '\0'; // Null-terminate the string
    }
    
    return error;
}


