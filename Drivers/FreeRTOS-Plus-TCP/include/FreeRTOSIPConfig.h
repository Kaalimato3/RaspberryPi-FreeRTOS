/*
 * FreeRTOS+TCP Labs Build 160112 (C) 2016 Real Time Engineers ltd.
 * Authors include Hein Tibosch and Richard Barry
 *
 *******************************************************************************
 ***** NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ***
 ***                                                                         ***
 ***                                                                         ***
 ***   FREERTOS+TCP IS STILL IN THE LAB (mainly because the FTP and HTTP     ***
 ***   demos have a dependency on FreeRTOS+FAT, which is only in the Labs    ***
 ***   download):                                                            ***
 ***                                                                         ***
 ***   FreeRTOS+TCP is functional and has been used in commercial products   ***
 ***   for some time.  Be aware however that we are still refining its       ***
 ***   design, the source code does not yet quite conform to the strict      ***
 ***   coding and style standards mandated by Real Time Engineers ltd., and  ***
 ***   the documentation and testing is not necessarily complete.            ***
 ***                                                                         ***
 ***   PLEASE REPORT EXPERIENCES USING THE SUPPORT RESOURCES FOUND ON THE    ***
 ***   URL: http://www.FreeRTOS.org/contact  Active early adopters may, at   ***
 ***   the sole discretion of Real Time Engineers Ltd., be offered versions  ***
 ***   under a license other than that described below.                      ***
 ***                                                                         ***
 ***                                                                         ***
 ***** NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ***
 *******************************************************************************
 *
 * FreeRTOS+TCP can be used under two different free open source licenses.  The
 * license that applies is dependent on the processor on which FreeRTOS+TCP is
 * executed, as follows:
 *
 * If FreeRTOS+TCP is executed on one of the processors listed under the Special 
 * License Arrangements heading of the FreeRTOS+TCP license information web 
 * page, then it can be used under the terms of the FreeRTOS Open Source 
 * License.  If FreeRTOS+TCP is used on any other processor, then it can be used
 * under the terms of the GNU General Public License V2.  Links to the relevant
 * licenses follow:
 * 
 * The FreeRTOS+TCP License Information Page: http://www.FreeRTOS.org/tcp_license 
 * The FreeRTOS Open Source License: http://www.FreeRTOS.org/license
 * The GNU General Public License Version 2: http://www.FreeRTOS.org/gpl-2.0.txt
 *
 * FreeRTOS+TCP is distributed in the hope that it will be useful.  You cannot
 * use FreeRTOS+TCP unless you agree that you use the software 'as is'.
 * FreeRTOS+TCP is provided WITHOUT ANY WARRANTY; without even the implied
 * warranties of NON-INFRINGEMENT, MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. Real Time Engineers Ltd. disclaims all conditions and terms, be they
 * implied, expressed, or statutory.
 *
 * 1 tab == 4 spaces!
 *
 * http://www.FreeRTOS.org
 * http://www.FreeRTOS.org/plus
 * http://www.FreeRTOS.org/labs
 *
 */

#ifndef FREERTOS_IP_CONFIG_H
#define FREERTOS_IP_CONFIG_H

#include <mem.h>

#define FreeRTOS_debug_print = 0
// #define FreeRTOS_debug_printf( MSG ) println(MSG, 0xFFFFFFFF);

/*Optional: ipconfigPACKET_FILLER_SIZE This option is a bit tricky:
it makes sure that all 32-bit fields in the network packets are 32-bit aligned.
This means that the 14-byte Ethernet header should start at a 16-bit offset.
Therefore ipconfigPACKET_FILLER_SIZE is defined a 2 (bytes).
I think that most EMAC's have an option to set this 2-byte offset for both incoming and outgoing packets.*/
#define ipconfigPACKET_FILLER_SIZE 0

#define portTICK_PERIOD_MS portTICK_RATE_MS
#define pdMS_TO_TICKS( xTimeInMs ) ( ( portTickType ) xTimeInMs * ( configTICK_RATE_HZ / ( ( portTickType ) 1000 ) ) )

//The size, in words (not bytes), of the stack allocated to the FreeRTOS+TCP RTOS task.
#define ipconfigIP_TASK_STACK_SIZE_WORDS 1024

//sets the priority of the RTOS task that executes the TCP/IP stack.
#define ipconfigIP_TASK_PRIORITY 1

//To use volatile list structure members
#define configLIST_VOLATILE volatile

#define configEMAC_TASK_STACK_SIZE 1024

#endif /* FREERTOS_IP_CONFIG_H */
