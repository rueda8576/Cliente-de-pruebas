#include "../include/f_client.h"

int menu_client() {

    int opcion, c;

    printf("-------------------------------------------------------------------------------------\n");
    printf("                                      CAMERA                                         \n");
    printf("-------------------------------------------------------------------------------------\n");
	printf("Options:\n");
    printf(" 1. Get context image from server\n");
    printf(" 2. Get video streaming\n");
    printf(" 3. Get features of the camera\n");
    printf(" 4. Print features of the camera\n");
    printf(" 5. Get set of frames , with different HDR configurations (2 Knee Point), from server\n");
    printf(" 6. Get set of frames , with different White Balance configurations, from server\n");
	printf(" 0. Exit\n");
    printf("Choose: ");
    
    scanf("%d", &opcion);
        
    // Consume the newline character left in the input stream by scanf
    while ((c = getchar()) != '\n' && c != EOF) {}
    return opcion;
}

