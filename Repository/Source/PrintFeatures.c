#include "../include/PrintFeatures.h"

VmbError_t PrintFeatures(SOCKET* client_socket, VmbUint32_t nFeatures, Feature* FeatureList)
{
    VmbError_t  error = VmbErrorSuccess;
    //Print features of the camera
    printf("nFeatures = %d\n", nFeatures);

    // Print the names
    for (int i = 0; i < nFeatures; i++)
    {
        printf("Feature name: %s        \n", FeatureList[i].Name);
        printf("Feature description: %s \n", FeatureList[i].Description);
        printf("Feature value: %s       \n\n", FeatureList[i].Value);
    }
    //TERMINAR return error
    return error;
}


