#include "../include/VideoStreaming.h"

double          g_dFrequency = 0.0;             // Frequency of tick counter
double          g_dFrameTime = 0.0;             // Timestamp of last frame

double GetTime()
{
    LARGE_INTEGER nCounter;
    QueryPerformanceCounter(&nCounter);
    return ((double)nCounter.QuadPart) / g_dFrequency;
}

void VMB_CALL FrameCallback(const VmbHandle_t cameraHandle, VmbFrame_t* pFrame)
{
    double          dFPS        = 0.0;              // frames per second calculated
    VmbBool_t       bFPSValid   = VmbBoolFalse;     // indicator if fps calculation was valid
    double          dFrameTime  = 0.0;              // reference time for frames
    double          dTimeDiff   = 0.0;              // time difference between frames

    dFrameTime  = GetTime();                        // get current time to calculate frames per second
    dTimeDiff   = dFrameTime - g_dFrameTime;        // build time difference with last frames time
    dFPS        = 1.0 / dTimeDiff;

    // store time for fps calculation in the next call
    g_dFrameTime = dFrameTime;
    printf(" FPS:%.2f\n", dFPS);

    // requeue the frame so it can be filled again
    fflush(stdout);
    VmbCaptureFrameQueue(cameraHandle, pFrame, &FrameCallback);
}

VmbError_t VideoStreaming(SOCKET* client_socket)
{
    //Vimba Parameters
	VmbError_t          error               = VmbErrorSuccess;
    VmbHandle_t         cameraHandle        = NULL;
    VmbFrame_t*         frame;
    VmbAccessMode_t     cameraAccessMode    = VmbAccessModeRead;        // We open the camera with read access
    VmbBool_t           bIsCommandDone      = VmbBoolFalse;             // Has a command finished execution
    const VmbUint32_t   nTimeout            = 2000;                     // Timeout for Grab
    const char*         pPixelFormat        = NULL;                     // The pixel format we use for acquisition
    VmbInt64_t          nPayloadSize        = 3145728;                  // The size of one frame

    LARGE_INTEGER       nFrequency;
    char                confirmation[MAX_BUFFER_SIZE_OPT] = { 0 };

    struct sockaddr_in server_address;
    server_address.sin_addr.s_addr      = inet_addr("192.168.1.20");    // Server IP (Raspberry Pi)
    server_address.sin_family           = PF_INET;
    server_address.sin_port             = htons(SERVER_PORT);
    int server_address_len              = sizeof(server_address);

    QueryPerformanceFrequency(&nFrequency);
    g_dFrequency = (double)nFrequency.QuadPart;

    // Allocate memory for the frame
    frame = (VmbFrame_t*)malloc(sizeof(VmbFrame_t));
    if (frame == NULL)
    {
        printf("Error: Failed to allocate memory for frame\n");
        return error;
    }

    printf("Waiting for configuration done check from server...\n");

    //Wait for configuration done check from server
    int bytes_recv = 0;
    memset(confirmation, 0, sizeof(confirmation));
    bytes_recv = recvfrom(client_socket, confirmation, sizeof(confirmation), 0, (struct sockaddr*)&server_address, &server_address_len);
    if (bytes_recv == -1)
    {
        perror("Error getting configuration done check from server\n");
        free(frame);
        exit(EXIT_FAILURE);
    }

    printf("Configurating client reception...\n");
    // Initialize Vimba
    error = VmbStartup();
    if (error != VmbErrorSuccess)
    {
        printf("Error initializing Vimba: %d\n", error);
        return error;
    }

    // Send discovery packets to GigE cameras and wait 250 ms until they are answered
    error = VmbFeatureCommandRun(gVimbaHandle, "GeVDiscoveryAllOnce");      
    if (VmbErrorSuccess != error)
    {
        printf("Could not ping GigE cameras over the network. Reason: %d\n", error);
        return error;
    }

    // Open camera
    error = VmbCameraOpen("DEV_000F315DF05B", cameraAccessMode, &cameraHandle);
    if (error != VmbErrorSuccess)
    {
        printf("Error opening camera: %d\n", error);
        VmbShutdown();
        return error;
    }

    frame -> buffer       = (unsigned char*)malloc((VmbUint32_t)nPayloadSize);
    frame -> bufferSize   = (VmbUint32_t)nPayloadSize;

    // Announce Frame
    error = VmbFrameAnnounce(cameraHandle, frame, (VmbUint32_t)sizeof(VmbFrame_t));
    if (error != VmbErrorSuccess) {
        printf("Error announcing frame: %d\n", error);
        VmbCaptureEnd(cameraHandle);
        VmbCameraClose(cameraHandle);
        VmbShutdown();
        return error;
    }

    // Start Capture Engine
    error = VmbCaptureStart(cameraHandle);
    if (error != VmbErrorSuccess) {
        printf("Error starting capture: %d\n", error);
        VmbCameraClose(cameraHandle);
        VmbShutdown();
        return error;
    }

    //Queue Frame
    error = VmbCaptureFrameQueue(cameraHandle, frame, &FrameCallback);
    if (error != VmbErrorSuccess) {
        printf("Error queueing frame: %d\n", error);
        VmbCaptureEnd(cameraHandle);
        VmbCameraClose(cameraHandle);
        VmbShutdown();
        return error;
    }

    printf("Getting frames...\n");
    printf("Press <enter> to stop acquisition...\n");
    getchar();

    // Cleanup
    VmbCaptureEnd(cameraHandle);
    VmbCameraClose(cameraHandle);
    VmbShutdown();

    //Send confirmation to client stop acquisition
    memset(confirmation, 0, sizeof(confirmation));
    strcpy(confirmation, "STOP");
    if (sendto(client_socket, confirmation, strlen(confirmation), 0, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)
    {
        printf("Confirmation sending failed\n");
        exit(EXIT_FAILURE);
    }

	return error;
}