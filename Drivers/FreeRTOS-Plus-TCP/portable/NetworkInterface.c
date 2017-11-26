/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"

/* The queue used to pass events into the IP-task for processing. */
xQueueHandle xOutputQueue = NULL;

typedef struct OutputInfo_asdf{
	int pNetworkBufferDescriptor_t;
	int portBASE_TYPE bReleaseAfterSend;
} OutputInfo;

void ethernetPollTask(){
	unsigned char *pucUseBuffer;
	int ulReceiveCount, ulResult;
	static NetworkBufferDescriptor_t *pxNextNetworkBufferDescriptor = NULL;
	const unsigned portBASE_TYPE xMinDescriptorsToLeave = 2UL;
	const portTickType xBlockTime = pdMS_TO_TICKS( 100UL );
	static IPStackEvent_t xRxEvent = { eNetworkRxEvent, NULL };

	//create a queue to store addresses of NetworkBufferDescriptors
	xOutputQueue = xQueueCreate( ( unsigned portBASE_TYPE ) ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS, 4 );

	for( ;; ){
		//send any waiting packets first
		for(int i = 0; i < uxQueueMessagesWaiting(xOutputQueue); i++){
// println("try send", 0xFF0000cc);
			OutputInfo out;
			xQueueReceive(xOutputQueue, &out, 1000);
			NetworkBufferDescriptor_t * const pxDescriptor = (NetworkBufferDescriptor_t*)out.pNetworkBufferDescriptor_t;
			//for(int i = 0; i < pxDescriptor->xDataLength; i++){printHex("", ((char*)pxDescriptor->pucEthernetBuffer)[i], 0xFFFFFFFF);}
			USPiSendFrame((void *)pxDescriptor->pucEthernetBuffer, pxDescriptor->xDataLength);
			if(out.bReleaseAfterSend) vReleaseNetworkBufferAndDescriptor( pxDescriptor );
// println("sent", 0xFF0000cc);
		}

		/* If pxNextNetworkBufferDescriptor was not left pointing at a valid
		descriptor then allocate one now. */
		if( ( pxNextNetworkBufferDescriptor == NULL ) && ( uxGetNumberOfFreeNetworkBuffers() > xMinDescriptorsToLeave ) )
		{
			pxNextNetworkBufferDescriptor = pxGetNetworkBufferWithDescriptor( ipTOTAL_ETHERNET_FRAME_SIZE, xBlockTime );
		}
		if( pxNextNetworkBufferDescriptor != NULL )
		{
			/* Point pucUseBuffer to the buffer pointed to by the descriptor. */
			pucUseBuffer = ( unsigned char* ) ( pxNextNetworkBufferDescriptor->pucEthernetBuffer - ipconfigPACKET_FILLER_SIZE );
		}
		else
		{
			/* As long as pxNextNetworkBufferDescriptor is NULL, the incoming
			messages will be flushed and ignored. */
			pucUseBuffer = NULL;
		}

		/* Read the next packet from the hardware into pucUseBuffer. */
		ulResult = USPiReceiveFrame (pucUseBuffer, &ulReceiveCount);

		if( ( ulResult != 1 ) || ( ulReceiveCount == 0 ) )
		{
			/* No data from the hardware. */
			continue;//break;
		}
// printHex("Frame received ", ulReceiveCount, 0xFFFFFFFF);
// if(ulReceiveCount != 0x3c) for(int i = 0; i < ulReceiveCount; i++){printHex("", ((char*)pucUseBuffer)[i], 0xFFFFFFFF);}
		if( pxNextNetworkBufferDescriptor == NULL )
		{
			/* Data was read from the hardware, but no descriptor was available
			for it, so it will be dropped. */
			iptraceETHERNET_RX_EVENT_LOST();
			continue;
		}

		iptraceNETWORK_INTERFACE_RECEIVE();
		pxNextNetworkBufferDescriptor->xDataLength = ( size_t ) ulReceiveCount;
		xRxEvent.pvData = ( void * ) pxNextNetworkBufferDescriptor;

		/* Send the descriptor to the IP task for processing. */
		if( xSendEventStructToIPTask( &xRxEvent, xBlockTime ) != pdTRUE )
		{
			/* The buffer could not be sent to the stack so must be released 
			again. */
			vReleaseNetworkBufferAndDescriptor( pxNextNetworkBufferDescriptor );
			iptraceETHERNET_RX_EVENT_LOST();
			FreeRTOS_printf( ( "prvEMACRxPoll: Can not queue return packet!\n" ) );
		}
		
		/* Now the buffer has either been passed to the IP-task,
		or it has been released in the code above. */
		pxNextNetworkBufferDescriptor = NULL;
	}
}

portBASE_TYPE xNetworkInterfaceInitialise(){
	if (!USPiInitialize ()){
		println("Cannot initialize USPi", 0xFFFFFFFF);
		return pdFAIL;
	}

	if (!USPiEthernetAvailable ()){
		println("Ethernet device not found", 0xFFFFFFFF);
		return pdFAIL;
	}

	xTaskCreate(ethernetPollTask, "poll", 128, NULL, 0, NULL);

	return pdPASS;
}

portBASE_TYPE xNetworkInterfaceOutput( NetworkBufferDescriptor_t * const pxDescriptor, portBASE_TYPE bReleaseAfterSend ){
	OutputInfo out;
	out.pNetworkBufferDescriptor_t = (int)pxDescriptor;
	out.bReleaseAfterSend = bReleaseAfterSend;
	xQueueSendToBack(xOutputQueue, &out, 1000);
	return 1;
}
