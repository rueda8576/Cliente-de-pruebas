#include "../include/pruebas_client.h"

VmbError_t HDR_2KP_AUTO_ALL(SOCKET* client_socket)
{
    // Get N HDR frames with 2 knee point from server
    printf("Waiting for data...\n");

    UINT32  f_bufferSize            = 3145728;
    UINT32  f_imageSize             = 3145728;
    UINT32  f_width                 = 1024;
    UINT32  f_height                = 1024;
    int     num_packets             = 64;
    int     bytes_recv              = 0;
    int     total_bytes_recv        = 0;
    int     num_recv_packets        = 0;
    double  AutoExposure            = 0;

    char confirmation[MAX_BUFFER_SIZE_OPT] = { 0 };
    char* recv_buffer = (char*)malloc(MAX_BUFFER_SIZE_FRAME * sizeof(char));
    if (recv_buffer == NULL) {
        perror("Failed to allocate memory");
        return 1;
    }

    struct sockaddr_in server_address;
    server_address.sin_addr.s_addr      = inet_addr("192.168.1.20"); // Server IP (Raspberry Pi)
    server_address.sin_family           = PF_INET;
    server_address.sin_port             = htons(SERVER_PORT);
    int server_address_len              = sizeof(server_address);

    //Valores para los distintos frames HDR
    //2 knee point
    float EXP_weights   [] = { 0.2      ,   0.4     ,   0.6     ,   0.8     ,   1   ,   1.2 };
    float EXP1_weights  [] = { 0.1      ,   0.2     ,   0.3     ,   0.4                     };
    float EXP2_weights  [] = { 0.8      ,   0.7     ,   0.6     ,   0.5                     };
    int   TH1_v         [] = { 40       ,   45      ,   50      ,   55                      };
    int   TH2_v         [] = { 15       ,   20      ,   25      ,   30                      };

    int EXP_length  = sizeof(EXP_weights)   / sizeof(float);    //k1
    int EXP1_length = sizeof(EXP1_weights)  / sizeof(float);    //k2
    int EXP2_length = sizeof(EXP2_weights)  / sizeof(float);    //k3
    int TH1_length  = sizeof(TH1_v)         / sizeof(int);      //k4
    int TH2_length  = sizeof(TH2_v)         / sizeof(int);      //k5
    int count       = 0;

    for (int k1 = 0; k1 < EXP_length; k1++)
    {
        for (int k2 = 0; k2 < EXP1_length; k2++)
        {
            for (int k3 = 0; k3 < EXP2_length; k3++)
            {
                for (int k4 = 0; k4 < TH1_length; k4++)
                {
                    for (int k5 = 0; k5 < TH2_length; k5++)
                    {
                        AVTBitmap   f_bitmap;
                        char        s_AutoExposure[100] = { 0 };
                        
                        //Get ExposureTimeAbs from server
                        total_bytes_recv    = 0;
                        AutoExposure        = 0;
                        bytes_recv          = 0;
                        bytes_recv = recvfrom(client_socket, s_AutoExposure, sizeof(s_AutoExposure), 0, (struct sockaddr*)&server_address, &server_address_len);
                        if (bytes_recv == -1)
                        {
                            perror("Error getting ExposureTimeAbs\n");
                            free(recv_buffer);
                            exit(EXIT_FAILURE);
                        }
                        AutoExposure = atof(s_AutoExposure);
                        printf("AutoExposure = %f [microseconds]\n", AutoExposure);

                        //Get frame.buffer from server
                        num_recv_packets = 0;
                        for (int j = 0; j < f_bufferSize; j += UDP_PACKET_SIZE)
                        {
                            char* aux_buffer = (char*)malloc(UDP_PACKET_SIZE * sizeof(char));
                            if (aux_buffer == NULL) {
                                perror("Failed to allocate memory\n");
                                free(recv_buffer);
                                free(aux_buffer);
                                return 1;
                            }
                            bytes_recv = 0;
                            bytes_recv = recvfrom(client_socket, aux_buffer, UDP_PACKET_SIZE, 0, (struct sockaddr*)&server_address, &server_address_len);
                            if (bytes_recv == SOCKET_ERROR)
                            {
                                printf("Error getting frame.buffer. Error Code : %d\n", WSAGetLastError());
                                free(recv_buffer);
                                free(aux_buffer);
                                return 1;
                            }
                            if (bytes_recv > 0)
                            {
                                total_bytes_recv = total_bytes_recv + bytes_recv;
                                num_recv_packets++;
                                memcpy(recv_buffer + j, aux_buffer, UDP_PACKET_SIZE);

                                //Send confirmation to client to send next frame
                                memset(confirmation, 0, sizeof(confirmation));
                                strcpy(confirmation, "NEXT");
                                if (sendto(client_socket, confirmation, strlen(confirmation), 0, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)
                                {
                                    printf("Confirmation sending failed\n");
                                    exit(EXIT_FAILURE);
                                }

                            }
                            if (bytes_recv == 0)
                            {
                                printf("bytes_recv == 0\n");
                                free(aux_buffer);
                            }
                            free(aux_buffer);
                        }
                
                        char aux_name[2048];
                        snprintf(aux_name, sizeof(aux_name), "../../Images_HDR/Gabro_LED_AutoEveryIteration/IM-%d-__AUTOExp-%.2f-__EXPweight-%.2f-__TH1v-%d-__EXP1weight-%.2f-__TH2v-%d-__EXP2weight-%.2f-.bmp", count + 1 + 2000, AutoExposure, EXP_weights[k1], TH1_v[k4], EXP1_weights[k2], TH2_v[k5], EXP2_weights[k3]);
                        const char* f_pFileName = aux_name;

                        //Convert to bmp
                        f_bitmap.bufferSize = f_imageSize;
                        f_bitmap.width      = f_width;
                        f_bitmap.height     = f_height;
                        f_bitmap.colorCode  = ColorCodeBGR24;

                        // Create the bitmap
                        if (0 == AVTCreateBitmap(&f_bitmap, recv_buffer))
                        {
                            printf("Could not create bitmap.\n");
                            free(recv_buffer);
                            return 1;
                        }
                        else
                        {
                            // Save the bitmap
                            if (0 == AVTWriteBitmapToFile(&f_bitmap, f_pFileName))
                            {
                                printf("Could not write bitmap to file.\n");
                                free(recv_buffer);
                                return 1;
                            }
                            else
                            {
                                printf("Bitmap successfully written to file \"%s\"\n", f_pFileName);

                                // Release the bitmap's buffer
                                if (0 == AVTReleaseBitmap(&f_bitmap))
                                {
                                    printf("Could not release the bitmap.\n");
                                    free(recv_buffer);
                                    return 1;
                                }

                                count++;
                                printf("count = %d\n", count);
                            }
                        }
                        //Add a delay to prevent errors
                        Sleep(100);

                        memset(recv_buffer, 0, sizeof(recv_buffer));
                        //Send confirmation to client to send next frame
                        memset(confirmation, 0, sizeof(confirmation));
                        strcpy(confirmation, "NEXT");
                        if (sendto(client_socket, confirmation, strlen(confirmation), 0, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)
                        {
                            printf("Confirmation sending failed\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                }
            }
        }   
    }
}

VmbError_t WhiteBalance_Scan(SOCKET* client_socket)
{
    printf("Waiting for data...\n");

    UINT32  f_bufferSize        = 3145728;
    UINT32  f_imageSize         = 3145728;
    UINT32  f_width             = 1024;
    UINT32  f_height            = 1024;
    int     num_packets         = 64;
    int     bytes_recv          = 0;
    int     total_bytes_recv    = 0;
    int     num_recv_packets    = 0;
    double  AutoExposure        = 0;

    char confirmation[MAX_BUFFER_SIZE_OPT] = { 0 };
    char* recv_buffer = (char*)malloc(MAX_BUFFER_SIZE_FRAME * sizeof(char));
    if (recv_buffer == NULL) {
        perror("Failed to allocate memory");
        return 1;
    }

    struct sockaddr_in server_address;
    server_address.sin_addr.s_addr  = inet_addr("192.168.1.20");     // Server IP (Raspberry Pi)
    server_address.sin_family       = PF_INET;
    server_address.sin_port         = htons(SERVER_PORT);
    int server_address_len          = sizeof(server_address);

    //Valores para los distintos frames HDR
    //2 knee point
    float Red_Weight    [] = { 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9, 3 };
    float Blue_Weight   [] = { 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9, 3 };

    int Red_length  = sizeof(Red_Weight)    / sizeof(float);        //k1
    int Blue_length = sizeof(Blue_Weight)   / sizeof(float);        //k2
    int count       = 0;

    for (int k1 = 0; k1 < Red_length; k1++)
    {
        for (int k2 = 0; k2 < Blue_length; k2++)
        {
            AVTBitmap f_bitmap;
            char    s_AutoExposure[100] = { 0 };

            //Get ExposureTimeAbs from server
            total_bytes_recv    = 0;
            AutoExposure        = 0;
            bytes_recv          = 0;
            bytes_recv = recvfrom(client_socket, s_AutoExposure, sizeof(s_AutoExposure), 0, (struct sockaddr*)&server_address, &server_address_len);
            if (bytes_recv == -1)
            {
                perror("Error getting ExposureTimeAbs\n");
                free(recv_buffer);
                exit(EXIT_FAILURE);
            }
            AutoExposure = atof(s_AutoExposure);
            printf("AutoExposure = %f [microseconds]\n", AutoExposure);

            //Get frame.buffer from server
            num_recv_packets = 0;
            for (int j = 0; j < f_bufferSize; j += UDP_PACKET_SIZE)
            {
                char* aux_buffer = (char*)malloc(UDP_PACKET_SIZE * sizeof(char));
                if (aux_buffer == NULL) {
                    perror("Failed to allocate memory\n");
                    free(recv_buffer);
                    free(aux_buffer);
                    return 1;
                }
                bytes_recv = 0;
                bytes_recv = recvfrom(client_socket, aux_buffer, UDP_PACKET_SIZE, 0, (struct sockaddr*)&server_address, &server_address_len);
                if (bytes_recv == SOCKET_ERROR)
                {
                    printf("Error getting frame.buffer. Error Code : %d\n", WSAGetLastError());
                    free(recv_buffer);
                    free(aux_buffer);
                    return 1;
                }
                if (bytes_recv > 0)
                {
                    total_bytes_recv = total_bytes_recv + bytes_recv;
                    num_recv_packets++;
                    memcpy(recv_buffer + j, aux_buffer, UDP_PACKET_SIZE);

                    //Send confirmation to client to send next frame
                    memset(confirmation, 0, sizeof(confirmation));
                    strcpy(confirmation, "NEXT");
                    if (sendto(client_socket, confirmation, strlen(confirmation), 0, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)
                    {
                        printf("Confirmation sending failed\n");
                        exit(EXIT_FAILURE);
                    }

                }
                if (bytes_recv == 0)
                {
                    printf("bytes_recv == 0\n");
                    free(aux_buffer);
                }
                free(aux_buffer);
            }

            char aux_name[2048];
            snprintf(aux_name, sizeof(aux_name), "../../Images_WhiteBalance/BucleGranodiorita/IM-%d-__AUTOExp-%.2f-__RedRatio-%.2f-__BlueRatio-%.2f-.bmp", count + 1, AutoExposure, Red_Weight[k1], Blue_Weight[k2]);
            const char* f_pFileName = aux_name;

            //Convert to bmp
            f_bitmap.bufferSize = f_imageSize;
            f_bitmap.width      = f_width;
            f_bitmap.height     = f_height;
            f_bitmap.colorCode  = ColorCodeBGR24;

            // Create the bitmap
            if (0 == AVTCreateBitmap(&f_bitmap, recv_buffer))
            {
                printf("Could not create bitmap.\n");
                free(recv_buffer);
                return 1;
            }
            else
            {
                // Save the bitmap
                if (0 == AVTWriteBitmapToFile(&f_bitmap, f_pFileName))
                {
                    printf("Could not write bitmap to file.\n");
                    free(recv_buffer);
                    return 1;
                }
                else
                {
                    printf("Bitmap successfully written to file \"%s\"\n", f_pFileName);

                    // Release the bitmap's buffer
                    if (0 == AVTReleaseBitmap(&f_bitmap))
                    {
                        printf("Could not release the bitmap.\n");
                        free(recv_buffer);
                        return 1;
                    }

                    count++;
                    printf("count = %d\n", count);
                }
            }
            //Add a delay to prevent errors
            Sleep(100);

            memset(recv_buffer, 0, sizeof(recv_buffer));
            //Send confirmation to client to send next frame
            memset(confirmation, 0, sizeof(confirmation));
            strcpy(confirmation, "NEXT");
            if (sendto(client_socket, confirmation, strlen(confirmation), 0, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)
            {
                printf("Confirmation sending failed\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}