/*******************************************************************************
File: delay.c                                                                

Description:
Delay for a certain time.

--------------------------------------------------------------------------------
API
void delayus(u16 time)
Delay for one us when the time is one,
e.g.
delayus(500) means delay for 500us.
void delayms(u16 time)
Delay for one ms when the time is one,
e.g.
delayus(500) means delay for 500us.
*******************************************************************************/
#include<typedefs.h>

/*******************************************************************************

/*******************************************************************************
* Function Definitions
*******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Public Functions */
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Function delayus

Description:
	-
Requires:
	-    
Promises:
	- 
*/
void delayus(u16 time){
    while(time--);
}

/*------------------------------------------------------------------------------
Function delayms

Description:
	-
Requires:
	-    
Promises:
	- 
*/
void delayms(u16 time)
{
	unsigned char i=100,j;
	for( ; time; time--)
	{
		while(--i)
		{
			j=10;
			while(--j);
		}
	}
}