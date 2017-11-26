// main.c
// Authored by Jared Hull
// Modified by Roope Lindström & Emil Pirinen
//
// Main initialises the devices
// Tasks simulate car lights

#include <FreeRTOS.h>
#include <task.h>
#include <string.h>

#include "interrupts.h"
#include "gpio.h"
#include "video.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

//Only for debug, normally should not
//   include private header
#include "FreeRTOS_IP_Private.h"

#define SERVER_PORT 8080

#define ACCELERATE_LED_GPIO 23
#define BRAKE_LED_GPIO 24
#define CLUTCH_LED_GPIO 25

<<<<<<< HEAD
#define ACCELERATE_TASK_DELAY 	4000
#define BRAKE_TASK_DELAY 		2000
#define CLUTCH_TASK_DELAY 		5000
=======
#define TICK_LENGTH 1000
>>>>>>> bde2b855951d9c85961f10b5969bfc06da7fe9db

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef long int32_t;

xTaskHandle accHandle;
xTaskHandle brakeHandle;
xTaskHandle clutchHandle;

int accState = 0;
int brakeState = 0;
int clutchState = 0;

unsigned velocity = 0;
unsigned prevVelocity = 0;

void task(int pin, int *state)
{
    *state = *state ? 0 : 1;
    SetGpio(pin, *state);
}

void taskAccelerate()
{
    task(ACCELERATE_LED_GPIO, &accState);
}

void taskBrake()
{
    task(BRAKE_LED_GPIO, &brakeState);
}

void taskClutch()
{
    task(CLUTCH_LED_GPIO, &clutchState);
}

uint8_t* intToString(unsigned n) {
    if (n > 999) return "NULL";

    uint8_t *str = malloc(64);
    uint8_t *empty = " ";

    int i1 = n / 100;
    int i2 = (n % 100) / 10;
    int i3 = n % 10;

    uint8_t c1 = i1 + '0';
    uint8_t c2 = i2 + '0';
    uint8_t c3 = i3 + '0';

    uint8_t *s1 = &c1;
    uint8_t *s2 = &c2;
    uint8_t *s3 = &c3;

    strncat(str, i1 ? s1 : empty, 1);
    strncat(str, (i2 || i1) ? s2 : empty, 1);
    strncat(str, s3, 1);

    return str;
}

void driveTask() {
    vTaskDelay(5000);

    while (1) {
        velocity++;

        if (velocity != prevVelocity) {
            uint8_t *velocityStr = malloc(256);

            strcat(velocityStr, "Velocity: ");
            strcat(velocityStr, intToString(velocity));
            strcat(velocityStr, " km/h");

            println(velocityStr, WHITE_TEXT);
        }

        vTaskDelay(TICK_LENGTH);
        prevVelocity = velocity;
    }
}

// void changeTaskState(int *state, xTaskHandle *handle) {
//     if (*state) vTaskSuspend(*handle);
//     else vTaskResume(*handle);
//     *state = *state ? 0 : 1;
// }

int checkCommand(uint8_t *cmd1, uint8_t *cmd2) {
    return !strncmp(cmd1, cmd2, strlen((char *)cmd2));
}

void runCommand(uint8_t *cmd) {
    if (checkCommand(cmd, "accel")) taskAccelerate();
    else if (checkCommand(cmd, "brake")) taskBrake();
    else if (checkCommand(cmd, "clutch")) taskClutch();
}

void taskScreenTest()
{
	/*println(" ", WHITE_TEXT);
	println(" ", WHITE_TEXT);
	println(" ", WHITE_TEXT);
	println(" ", WHITE_TEXT);
	println(" ", WHITE_TEXT);
	println("       A A A A A A A A A", WHITE_TEXT);*/
	
    println("a b c d e f g h i j k l m n o p q r s t u v w x y z", BLUE_TEXT);
    println("A B C D E F G H I J K L M N O P Q R S T U V W X Y Z", GREEN_TEXT);
    println("0 1 2 3 4 5 6 7 8 9", RED_TEXT);
	println("{ | } ~ ` _ ^ ] [ \\ @ ? > < = : ; / . - , + * ) ( ' & % $ # \" ! ", ORANGE_TEXT);
	while(1);
}

#undef CREATE_SOCK_TASK
// #define CREATE_SOCK_TASK
#define tcpechoSHUTDOWN_DELAY (pdMS_TO_TICKS(5000))

//server task DOES work in this build, it DOES accept a connection
void serverListenTask()
{
    int status = 0;
    static const portTickType xReceiveTimeOut = portMAX_DELAY;
    const portTickType xDelay8s = pdMS_TO_TICKS(8000UL);
    const portTickType xDelay500ms = pdMS_TO_TICKS(500UL);
    const portBASE_TYPE xBacklog = 4;

    portTickType xTimeOnShutdown;
    uint8_t *pucRxBuffer;
    // for debug
    FreeRTOS_Socket_t *sockt;

    //printAddressConfiguration();
    volatile int times = 10;
    println("Server task starting", BLUE_TEXT);
    if (FreeRTOS_IsNetworkUp())
    {
        println("Network is UP", BLUE_TEXT);
    }
    else
    {
        println("Network is Down", BLUE_TEXT);
        while (!FreeRTOS_IsNetworkUp())
        {
            vTaskDelay(xDelay500ms);
        }
    }
    println("Serv tsk done wait net", BLUE_TEXT);

    Socket_t listen_sock;
    listen_sock = FreeRTOS_socket(FREERTOS_AF_INET, FREERTOS_SOCK_STREAM, FREERTOS_IPPROTO_TCP);
    printHex("listenfd sock: ", (int)listen_sock, BLUE_TEXT);
    printHex("IPPROTO_TCP val: ", FREERTOS_IPPROTO_TCP, BLUE_TEXT);
    if (listen_sock == FREERTOS_INVALID_SOCKET)
    {
        println("Socket is NOT valid", GREEN_TEXT);
    }
    else
    {
        println("Socket is valid", BLUE_TEXT);
        sockt = (FreeRTOS_Socket_t *)listen_sock;
        if (sockt->ucProtocol == FREERTOS_IPPROTO_TCP)
        {
            println("Proto is TCP", BLUE_TEXT);
        }
        else
        {
            printHex("Proto NOT TCP: ", sockt->ucProtocol, BLUE_TEXT);
        }
    }

    /* Set a time out so accept() will just wait for a connection. */
    FreeRTOS_setsockopt(listen_sock,
                        0,
                        FREERTOS_SO_RCVTIMEO,
                        &xReceiveTimeOut,
                        sizeof(xReceiveTimeOut));

    /**
         ** If I dont set REUSE option, accept will never return
        **/
    portBASE_TYPE xReuseSocket = pdTRUE;
    FreeRTOS_setsockopt(listen_sock,
                        0,
                        FREERTOS_SO_REUSE_LISTEN_SOCKET,
                        (void *)&xReuseSocket,
                        sizeof(xReuseSocket));

    struct freertos_sockaddr server, client;
    server.sin_port = FreeRTOS_htons((uint16_t)SERVER_PORT);
    //server.sin_addr = FreeRTOS_inet_addr("192.168.1.9");

    socklen_t cli_size = sizeof(client);
    println("Server task about to bind", BLUE_TEXT);
    status = FreeRTOS_bind(listen_sock, &server, sizeof(server));
    printHex("bind status: ", (int)status, BLUE_TEXT);
    sockt = (FreeRTOS_Socket_t *)listen_sock;
    printHex("Bind port: ", (unsigned int)sockt->usLocPort, BLUE_TEXT);

    println("Server task about to listen", BLUE_TEXT);
    status = FreeRTOS_listen(listen_sock, xBacklog);
    printHex("listen status: ", (int)status, BLUE_TEXT);

    int clients = 0;
    int32_t lBytes, lSent, lTotalSent;

    println("Server task accepting", BLUE_TEXT);
    Socket_t connect_sock = FreeRTOS_accept(listen_sock, (struct freertos_sockaddr *)&client, &cli_size);
    println("Connection accepted", ORANGE_TEXT);

    pucRxBuffer = (uint8_t *)pvPortMalloc(ipconfigTCP_MSS);
    for (;;)
    {
        memset(pucRxBuffer, 0x00, ipconfigTCP_MSS);
        if ((lBytes = FreeRTOS_recv(connect_sock, pucRxBuffer, ipconfigTCP_MSS, 0)) > 0)
        {
            runCommand(pucRxBuffer);
            printHex("Chars Received: ", (unsigned int)lBytes, ORANGE_TEXT);
            println(pucRxBuffer, RED_TEXT);

            lSent = 0;
            lTotalSent = 0;

            uint8_t *messageBuffer = "From server: ";
            int32_t messageBytes = sizeof(uint8_t) * strlen((char *)messageBuffer);
            int32_t totalBytes = lBytes + messageBytes;

            uint8_t *totalBuffer = (uint8_t *)malloc(totalBytes + 1);
            strcpy(totalBuffer, messageBuffer);
            strcat(totalBuffer, pucRxBuffer);

            while ((lSent >= 0) && (lTotalSent < totalBytes))
            {
                lSent = FreeRTOS_send(connect_sock, totalBuffer, totalBytes - lTotalSent, 0);
                lTotalSent += lSent;
            }
            // if (lSent < 0) break;
        }

        FreeRTOS_shutdown(connect_sock, FREERTOS_SHUT_RDWR);

        // Wait for the shutdown to take effect, indicated by FreeRTOS_recv()
        // returning an error.
        xTimeOnShutdown = xTaskGetTickCount();

        println("Waiting for shutdown", GREEN_TEXT);

        do
        {
            if (FreeRTOS_recv(connect_sock, pucRxBuffer, ipconfigTCP_MSS, 0) < 0)
                break;
        } while ((xTaskGetTickCount() - xTimeOnShutdown) < tcpechoSHUTDOWN_DELAY);

        println("Shutdown successful", GREEN_TEXT);

        vPortFree(pucRxBuffer);
        FreeRTOS_closesocket(connect_sock);

        println("Socket closed", GREEN_TEXT);
        break;
    }
}

void serverLoop()
{
    while (1)
    {
        serverListenTask();
    }
}

int main(void)
{
    SetGpioFunction(ACCELERATE_LED_GPIO, 1);
    SetGpioFunction(BRAKE_LED_GPIO, 1);
    SetGpioFunction(CLUTCH_LED_GPIO, 1);

    initFB();

    // SetGpio(ACCELERATE_LED_GPIO, 1);
    // SetGpio(BRAKE_LED_GPIO, 1);
    // SetGpio(CLUTCH_LED_GPIO, 1);

    DisableInterrupts();
    InitInterruptController();

<<<<<<< HEAD
	//xTaskCreate(serverTask, "server", 128, NULL, 0, NULL);
	//xTaskCreate(serverListenTask, "server", 128, NULL, 0, NULL);

	xTaskCreate(taskScreenTest, "screenTest", 128, NULL, 0, NULL);

=======
    //ensure the IP and gateway match the router settings!
    //const unsigned char ucIPAddress[ 4 ] = {192, 168, 1, 42};
    const unsigned char ucIPAddress[4] = {10, 10, 206, 100};
    const unsigned char ucNetMask[4] = {255, 255, 255, 0};
    const unsigned char ucGatewayAddress[4] = {10, 10, 206, 1};
    const unsigned char ucDNSServerAddress[4] = {10, 10, 206, 1};
    //const unsigned char ucMACAddress[ 6 ] = {0xB8, 0x27, 0xEB, 0x19, 0xAD, 0xA7};
    const unsigned char ucMACAddress[6] = {0xB8, 0x27, 0xEB, 0xA0, 0xE8, 0x54};
    FreeRTOS_IPInit(ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddress);
>>>>>>> bde2b855951d9c85961f10b5969bfc06da7fe9db

    xTaskCreate(serverLoop, "server", 128, NULL, 0, NULL);
    xTaskCreate(driveTask, "drive", 128, NULL, 0, NULL);

    //set to 0 for no debug, 1 for debug, or 2 for GCC instrumentation (if enabled in config)
    loaded = 1;

    println("Starting task scheduler", GREEN_TEXT);

    vTaskStartScheduler();

    /*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
    while (1)
    {
        ;
    }
}
