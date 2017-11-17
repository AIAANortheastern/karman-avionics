/**
 * @file USBTask.c
 * @brief Manages USB UDC
 *
 *  Created: 5/19/2017 4:53:00 PM
 *  Author: Andrew Kaster
 *
 * Manages the usb UDC connection and runs the state machine if the device is attached properly.
 */

#include "USBTask.h"
#include "USBUtils.h"
#include "Timer.h"

/**
 * @brief Controls USB interface
 *
 * Manages the usb UDC connection and runs the state machine if the device is 
 * attached properly. Attach UDC device (ASF construct) if debounced VBUS is
 * asserted. Detach UDC device if VBUS is deasserted while UDC is running.
 * If VBUS is asserted UDC is running, run the state machine.
 *
 */
void USB_task_func(void)
{
    if(false == gIsUSBConnected) {
        if(true == gIsUSBActive)
        {
            udc_detach();
        }
    }
    /* USB cable is connected, but UDC is not attached */
    else if(false == gIsUSBActive)
    {
        /* Debounce conenctions by requiring power to be connected for 8 ms */
        if(get_timer_count() > (gUSBConnectTime + EIGHT_MS))
        {
            udc_attach();
        }
    }

    /* USB must be connected and active in order to send messages */
    if((true == gIsUSBConnected) && (true == gIsUSBActive))
    {
        usb_utils_state_mach();
    }
}

