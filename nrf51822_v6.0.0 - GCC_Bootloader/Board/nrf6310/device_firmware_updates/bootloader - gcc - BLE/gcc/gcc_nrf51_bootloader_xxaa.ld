/* Linker script to configure memory regions. */
SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  /* TODO: take these values from config file */
  /* FLASH (rx) : ORIGIN = 0x00038000, LENGTH = 32K */
  FLASH (rx) : ORIGIN = 0x00034000, LENGTH = 48K
  RAM (rwx) : ORIGIN = 0x20002000, LENGTH = 8K
  BOOTLOADER_SETTINGS (rw) : ORIGIN = 0x0003FC00, LENGTH = 1K
  
}
SECTIONS
{
  /* placing my named section at given address: */
  .bootloaderSettings 0x0003FC00 :
  {
	
  } 
	.uicrBootStartAddress 0x10001014 :
	{
    
  } 
  /* other placements follow here... */
}
/*
.bootloaderSettings (NOLOAD):
{
  *(.bootloaderSettings)
} > BOOTLOADER_SETTINGS
*/
INCLUDE "gcc_nrf51_common.ld"
