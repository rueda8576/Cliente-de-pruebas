#ifndef F_CLIENT
#define F_CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>                            //Sleep()
#include <VimbaC/Include/VimbaC.h>
#include <VimbaC/Include/VmbCommonTypes.h>

/**
 * @brief Displays the command line inteface to choose an option of the camera handler
 * @return the number associated to the option selected
*/
int menu_client();

#endif