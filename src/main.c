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

int main(void) {
  hwInit();
  
  while(1) {
    TM_USB_MSCHOST_Process();
    if (TM_USB_MSCHOST_Device() == TM_USB_MSCHOST_Result_Connected) {
      while (1) {
        cwSFPlayDirectory("1:", 0);
      }
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
  printf("\r\n");
  printf("Peripheries initialised\r\n");
  Delayms(1000);
}
