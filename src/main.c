/**
 *  STM32F4 template project with LED, Button, MEMS and USB FATFS
 *  using libraries from http://stm32f4-discovery.com
 *
 *  @author		Cahya Wirawan
 *  @email		cahya at gmx.at
 *  @website		http://oldjava.org
 *  @ide		just gcc with makefile
 *  @stdperiph	STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 */

#include "string.h"
#include "stdio.h"
#include "main.h"
/* Include core modules */
#include "stm32f4xx.h"
/* Great STM32f4 libraries created by Tilen Majerle */
#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_usart.h"
#include "tm_stm32f4_lis302dl_lis3dsh.h"
#include "tm_stm32f4_fatfs.h"
#include "tm_stm32f4_usb_msc_host.h"
#include "cwSoundFile.h"

void hwInit();
void fileWrite();


int main(void) {
  TM_LIS302DL_LIS3DSH_t Axes_Data;
  uint counter = 0;
	
  hwInit();
  
  while(1) {
    TM_USB_MSCHOST_Process();
    /* If button pressed */
    if (TM_DISCO_ButtonPressed()) {
      printf("ButtonPressed\r\n");
    /* Turn on leds */
      TM_DISCO_LedOn(LED_RED | LED_GREEN);
      Delayms(1000);
      TM_DISCO_LedOff(LED_RED | LED_GREEN);
      Delayms(500);
      cwSFPlayDirectory("1:", 0);
    } else {
      if(counter%500000 == 0) {
        /* Read axes data from initialized accelerometer */
        TM_LIS302DL_LIS3DSH_ReadAxes(&Axes_Data);
      
        printf("Axes X:%d, Y:%d\n\r", Axes_Data.X, Axes_Data.Y);
      
        /* Turn LEDS on or off */
        /* Check X axes */
        if (Axes_Data.X > 200) {
            TM_DISCO_LedOn(LED_ORANGE);
        } else {
            TM_DISCO_LedOff(LED_ORANGE);
        }
        if (Axes_Data.X < -200) {
            TM_DISCO_LedOn(LED_BLUE);
        } else {
            TM_DISCO_LedOff(LED_BLUE);
        }
        /* Check Y axes */
        if (Axes_Data.Y > 200) {
            TM_DISCO_LedOn(LED_RED);
        } else {
            TM_DISCO_LedOff(LED_RED);
        }
        if (Axes_Data.Y < -200) {
            TM_DISCO_LedOn(LED_GREEN);
        } else {
            TM_DISCO_LedOff(LED_GREEN);
        }
        counter = 0;
      }
      counter++;
    }
  }
}

void hwInit() {
	// Initialize System
	//SystemInit();
	if (SysTick_Config(SystemCoreClock / 1000)) {
		// Capture error
		while (1){};
	}
  // Initialise delay Systick timer
  TM_DELAY_Init();
	// Initialise leds on board
	TM_DISCO_LedInit();
	//Initialise USART2 at 115200 baud, TX: PA2, RX: PA3
  TM_USART_Init(USART2, TM_USART_PinsPack_1, 115200);
	// Initialize button on board
	TM_DISCO_ButtonInit();
  /* Initialize USB MSC HOST */
  TM_USB_MSCHOST_Init();
    
	/* Detect proper device */
  if (TM_LIS302DL_LIS3DSH_Detect() == TM_LIS302DL_LIS3DSH_Device_LIS302DL) {
      /* Turn on GREEN and RED */
      TM_DISCO_LedOn(LED_GREEN | LED_RED);
      /* Initialize LIS302DL */
      TM_LIS302DL_LIS3DSH_Init(TM_LIS302DL_Sensitivity_2_3G, TM_LIS302DL_Filter_2Hz);
  } else if (TM_LIS302DL_LIS3DSH_Detect() == TM_LIS302DL_LIS3DSH_Device_LIS3DSH) {
      /* Turn on BLUE and ORANGE */
      TM_DISCO_LedOn(LED_BLUE | LED_ORANGE);
      /* Initialize LIS3DSH */
      TM_LIS302DL_LIS3DSH_Init(TM_LIS3DSH_Sensitivity_2G, TM_LIS3DSH_Filter_800Hz);
  } else {
      /* Device is not recognized */      
      /* Turn on ALL leds */
      TM_DISCO_LedOn(LED_GREEN | LED_RED | LED_BLUE | LED_ORANGE);
      
      /* Infinite loop */
      while (1);
  }
  
  printf("Peripheries initialised\r\n");
  Delayms(1000);
}

void fileWrite() {
  FATFS USB_Fs;
  FIL USB_Fil;
  char buffer[50];
  char filename[20];
  static uint8_t write = 1;
  uint32_t free, total;
  
  if (TM_USB_MSCHOST_Device() == TM_USB_MSCHOST_Result_Connected) {
    /* If we didn't write data already */
    if (write) {
      /* Try to mount USB device */
      /* USB is at 1: */
      if (f_mount(&USB_Fs, "1:", 1) == FR_OK) {
        TM_DISCO_LedOn(LED_GREEN);
        /* Mounted ok */
        /* Try to open USB file */
        sprintf(filename, "1:file_%d.txt", write);
        if (f_open(&USB_Fil, filename, FA_READ | FA_WRITE | FA_OPEN_ALWAYS) == FR_OK) {
          /* We want to write only once */
          write++;
          
          /* Get total and free space on USB */
          TM_FATFS_USBDriveSize(&total, &free);
          
          /* Put data */
          f_puts("This is my first file with USB and FatFS\n", &USB_Fil);
          f_puts("with USB MSC HOST library from stm32f4-discovery.com\n", &USB_Fil);
          f_puts("----------------------------------------------------\n", &USB_Fil);
          f_puts("USB total and free space:\n\n", &USB_Fil);
          /* Total space */
          sprintf(buffer, "Total: %8u kB; %5u MB; %2u GB\n", (uint) total,(uint) total / 1024, (uint) total / 1048576);
          f_puts(buffer, &USB_Fil);
          /* Free space */
          sprintf(buffer, "Free:  %8u kB; %5u MB; %2u GB\n", (uint) free, (uint) free / 1024, (uint) free / 1048576);
          f_puts(buffer, &USB_Fil);
          f_puts("----------------------------------------------------\n", &USB_Fil);
          /* Close USB file */
          f_close(&USB_Fil);

          /* Turn GREEN LED On and RED LED Off */
          /* Indicate successful write */
          TM_DISCO_LedOn(LED_GREEN);
          TM_DISCO_LedOff(LED_RED);
          printf("file %s has been created successfully\r\n", filename);
        }
      }
      /* Unmount USB */
      f_mount(0, "1:", 1);
    }
  } else {
    /* Not inserted, turn on RED led */
    TM_DISCO_LedOn(LED_RED);
    TM_DISCO_LedOff(LED_GREEN);
    printf("can't open/write a file!!!\r\n");
    
    /* Ready to write next time */
    write = 1;
  }
}
