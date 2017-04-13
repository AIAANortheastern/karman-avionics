#include "usb_driver.h"
#include "conf_usb.h"
#include "FlashMem.h"

void user_callback_vbus_action(bool b_vbus_high){}

void dump_to_usb() {

  flash_data_hdr_t header;
  flashmem_hdrstatus_t hdr_status = flashmem_verify_header(&header);
  flash_data_entry_t entry;
  switch(hdr_status) {
    case HDR_VALID:
      for (int i = 0; i < header.num_entries; i++) {
        flashmem_read_entry(&entry, i);
        while (!udi_cdc_is_tx_ready()) {
          asm volatile ("nop \n\t");
        }
        udi_cdc_write_buf(&entry, sizeof(entry));
        //udi_cdc_putc, getc, write_buff, read_buff
      }
    break;
    case HDR_INVALID:
    break;
    case HDR_ZERO:
    break;
    case HDR_READFAIL:
    break;
  }
}