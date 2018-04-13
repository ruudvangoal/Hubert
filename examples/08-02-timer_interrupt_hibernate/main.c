/******************************************************************
 * Hubert Data Logger
 * Timer Interrupt Hibernate Example
 * Developed by Sevun Scientific, Inc.
 * http://sevunscientific.com
 * *****************************************************************
 *
 *    _____/\\\\\\\\\\\_______/\\\\\\\\\\\____/\\\\\\\\\\\_
 *     ___/\\\/////////\\\___/\\\/////////\\\_\/////\\\///__
 *      __\//\\\______\///___\//\\\______\///______\/\\\_____
 *       ___\////\\\___________\////\\\_____________\/\\\_____
 *        ______\////\\\___________\////\\\__________\/\\\_____
 *         _________\////\\\___________\////\\\_______\/\\\_____
 *          __/\\\______\//\\\___/\\\______\//\\\______\/\\\_____
 *           _\///\\\\\\\\\\\/___\///\\\\\\\\\\\/____/\\\\\\\\\\\_
 *            ___\///////////_______\///////////_____\///////////__
 *
 * *****************************************************************
 */

#include <stdint.h>
#include <stdbool.h>
#include "utils/ustdlib.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/hibernate.h"
#include "driverlib/gpio.h"
#include "utils/uartstdio.h"

#define UART_SPEED              115200
#define HIBERNATE_WAKE_DELAY    5

//*****************************************************************************
// Hibernate Interrupt Function
//*****************************************************************************
volatile bool g_bHibernateFlag = 0;        // Timer 0 occurred flag

void HibernateHandler(void)
{

}

int main(void)
{
    // Set microcontroller to use the 16 MHz external crystal
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    //*****************************************************************************
    // Pins Setup
    //*****************************************************************************

    // Enable the Port C peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Sets the pin associated with IND1 to be an output
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

    //*****************************************************************************
    // UART Setup
    //*****************************************************************************

    // Enable the Port A and UART0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Sets the pins associated with UART0
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Initialize the UART0 using uartstdio
    UARTStdioConfig(0, UART_SPEED, SysCtlClockGet());

    //*****************************************************************************
    // Hibernate Setup
    //*****************************************************************************

    // Perform normal power-on initialization
    SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_HIBERNATE)) {}
    HibernateEnableExpClk(SysCtlClockGet());
    HibernateGPIORetentionEnable();
    SysCtlDelay(SysCtlClockGet()/3/50); // Delay 2 ms
    HibernateClockConfig(HIBERNATE_OSC_HIGHDRIVE);
    HibernateRTCEnable();
    HibernateWakeSet(HIBERNATE_WAKE_PIN | HIBERNATE_WAKE_RTC);

    //*****************************************************************************
    // Main Code
    //*****************************************************************************

    uint32_t ui32RTCTime = HibernateRTCGet();
    UARTprintf("\r\n%d seconds",ui32RTCTime);

    // Writes HIGH to pins
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);  // IND1 LED On
    SysCtlDelay(SysCtlClockGet()/3/10);                     // Delay 0.1 second
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);           // IND2 LED Off

    HibernateRTCMatchSet(0,HibernateRTCGet()+HIBERNATE_WAKE_DELAY);
    HibernateRequest();

    while(1) {}  // Repeats this section over and over
}
