/*
 * USBTask.c
 *
 * Created: 5/19/2017 4:53:00 PM
 *  Author: Andrew Kaster
 */

#include "USBTask.h"
#include "USBUtils.h"
#include "Timer.h"

/* Manages the usb UDC connection and runs the state machine if the device is attached properly. */
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

