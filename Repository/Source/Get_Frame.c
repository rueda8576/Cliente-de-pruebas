#include "../include/Get_Frame.h"

VmbError_t Get_Frame(SOCKET* client_socket)
{
    VmbError_t  error                   = VmbErrorSuccess;
    int         elapsed_time            = 0;
    clock_t     start_time, end_time;

    struct sockaddr_in server_address;
    struct sockaddr_in processor_address;
    
    server_address.sin_addr.s_addr      = inet_addr("192.168.10.13"); // Server IP (Raspberry Pi)
    server_address.sin_family           = PF_INET;
    server_address.sin_port             = htons(SERVER_PORT);
    int server_address_len              = sizeof(server_address);

    processor_address.sin_addr.s_addr   = inet_addr("192.168.1.11");
    processor_address.sin_family        = PF_INET;
    processor_address.sin_port          = htons(PROCESSOR_PORT);
    int processor_address_len           = sizeof(processor_address);

    char confirmation[MAX_BUFFER_SIZE_OPT] = { 0 };
    char* recv_buffer = (char*)malloc(MAX_BUFFER_SIZE_FRAME * sizeof(char));
    if (recv_buffer == NULL) {
        perror("Failed to allocate memory");
        return 1;
    }

    // Get image from server
    start_time              = clock();
    int bytes_recv          = 0;
    int total_bytes_recv    = 0;
    printf("Waiting for data...\n");
   
    // Receive data from server
    int     bufferSize              = 3145728;
    int     imageSize               = 3145728;
    int     width                   = 1024;
    int     height                  = 1024;
    int     num_packets             = 64;
    char    s_Color[100]            = { 0 };
    double  Gain                    = 0;
    double  ExposureTimeAbs         = 60;
    char    s_Gain[20]              = { 0 };
    char    s_ExposureTimeAbs[20]   = { 0 };

    const char* pFileName = "../../Images/image.bmp";           // The path and name of the file wanted to create
    AVTBitmap bitmap;                                           // The bitmap to create

    int num_recv_packets    = 0;
    int i                   = 0;

    //Get ExposureTimeAbs from server
    bytes_recv = 0;
    bytes_recv = recvfrom(client_socket, s_ExposureTimeAbs, sizeof(s_ExposureTimeAbs), 0, (struct sockaddr*)&processor_address, &processor_address_len);
    if (bytes_recv == -1)
    {
        perror("Error getting ExposureTimeAbs\n");
        free(recv_buffer);
        exit(EXIT_FAILURE);
    }
    
    //Get Gain from server
    bytes_recv = 0;
    bytes_recv = recvfrom(client_socket, s_Gain, sizeof(s_Gain), 0, (struct sockaddr*)&processor_address, &processor_address_len);
    if (bytes_recv == -1)
    {
        perror("Error getting s_Gain\n");
        free(recv_buffer);
        exit(EXIT_FAILURE);
    }

    //Get frame.buffer from server
    for (; i < bufferSize; i += UDP_PACKET_SIZE)
    {
        char* aux_buffer = (char*)malloc(UDP_PACKET_SIZE * sizeof(char));
        if (aux_buffer == NULL) {
            perror("Failed to allocate memory\n");
            free(recv_buffer);
            free(aux_buffer);
            return 1;
        }
        bytes_recv = 0;
        bytes_recv = recvfrom(client_socket, aux_buffer, UDP_PACKET_SIZE, 0, (struct sockaddr*)&processor_address, &processor_address_len);
        if (bytes_recv == SOCKET_ERROR)
        {
            printf("Error getting frame.buffer. Error Code : %d\n", WSAGetLastError());
            free(recv_buffer);
            free(aux_buffer);
            return 1;
        }
        if (bytes_recv > 0)
        {
            num_recv_packets++;
            memcpy(recv_buffer + i, aux_buffer, UDP_PACKET_SIZE);
            total_bytes_recv = total_bytes_recv + bytes_recv;
            free(aux_buffer);

            //Send confirmation to client to send next frame
            memset(confirmation, 0, sizeof(confirmation));
            strcpy(confirmation, "NEXT");
            if (sendto(client_socket, confirmation, strlen(confirmation), 0, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)
            {
                printf("Confirmation sending failed\n");
                exit(EXIT_FAILURE);
            }
            printf("total_bytes_recv = %d\n", total_bytes_recv);
        }
        if (bytes_recv == 0)
        {
            printf("bytes_recv == 0\n");
            free(aux_buffer);
        }
    }
    ExposureTimeAbs = atof(s_ExposureTimeAbs);
    Gain            = atof(s_Gain);
    printf("ExposureTimeAbs = %f [microseconds] \n", ExposureTimeAbs);
    printf("Gain = %f                           \n", Gain);

    //Convert to bmp
    bitmap.bufferSize   = imageSize;
    bitmap.width        = width;
    bitmap.height       = height;

    sprintf(s_Color, "%s", PIXEL_FORMAT);
    //ColorCode
    if      (strcmp(s_Color, "Mono8")       == 0) { bitmap.colorCode = ColorCodeMono8; }
    else if (strcmp(s_Color, "BGR8Packed")  == 0) { bitmap.colorCode = ColorCodeBGR24; }
    else if (strcmp(s_Color, "RGB8Packed")  == 0) { bitmap.colorCode = ColorCodeRGB24; }
    else
    {
        printf("Invalid ColorCode from server\n");
        return 1;
    }

    // Create the bitmap
    if (0 == AVTCreateBitmap(&bitmap, recv_buffer))
    {
        printf("Could not create bitmap.\n");
        free(recv_buffer);
        return 1;
    }
    else
    {
        // Save the bitmap
        if (0 == AVTWriteBitmapToFile(&bitmap, pFileName))
        {
            printf("Could not write bitmap to file.\n");
            free(recv_buffer);
            return 1;
        }
        else
        {
            printf("Bitmap successfully written to file \"%s\"\n", pFileName);
            // Release the bitmap's buffer
            if (0 == AVTReleaseBitmap(&bitmap))
            {
                printf("Could not release the bitmap.\n");
                free(recv_buffer);
                return 1;
            }

            end_time = clock();
            // Calculate elapsed time in milliseconds
            elapsed_time = (int)(1000 * (end_time - start_time) / (double)CLOCKS_PER_SEC);
            printf("Total time: %d s\n", elapsed_time/1000);

            return VmbErrorSuccess;
        }
    }
}