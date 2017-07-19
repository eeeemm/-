/**********************************************************************************************************************
File: configuration.h      

Description:
Main configuration header file for project.  This file bridges many of the generic features of the 
firmware to the specific features of the design. The definitions should be updated
to match the target hardware.  
***********************************************************************************************************************/

#ifndef __CONFIGURATION_H_
#define __CONFIGURATION_H_

/**********************************************************************************************************************
Includes
***********************************************************************************************************************/
/* Common header files */
#include <reg51.h>
#include <string.h>
#include "typedefs.h"

/* Common driver header files */
#include "delay.h"
#include "lcdDrive.h"
#include "UARTDrive.h"
#include "KS103Drive.h"

#endif
/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/