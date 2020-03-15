
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "spi.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "OLED_Driver.h"
#include "OLED_GFX.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void lcdTestPattern(void);
void testlines(void);
void testfastlines(void);
void testdrawrects(void);
void testfillrects(uint16_t color1, uint16_t color2);
void testfillcircles(uint8_t radius, uint16_t color);
void testdrawcircles(uint16_t color);
void testroundrects(void);
void testtriangles(void);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
OLED_GFX oled = OLED_GFX();
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  oled.Device_Init();

  oled.Display_Interface();
  HAL_Delay(3000);
  oled.Clear_Screen();
  
  oled.Set_Color(RED);
  oled.print_String(0, 0, "Hello World !", FONT_5X8);
  oled.Set_Color(BLUE);
  oled.print_String(0, 8, "Hello WaveShare", FONT_8X16);
  HAL_Delay(2000);
  oled.Clear_Screen();
  
  oled.Set_Color(WHITE);
  oled.Draw_Pixel(50,50);
  HAL_Delay(1000);

  lcdTestPattern();
  HAL_Delay(1000);
  
  testlines();
  HAL_Delay(1000);
  
  testfastlines();
  HAL_Delay(1000);
  
  testdrawrects();
  HAL_Delay(1000);
  
  testfillrects(BLUE,YELLOW);
  HAL_Delay(1000);
  
  oled.Clear_Screen();
  testfillcircles(63, BLUE);
  HAL_Delay(500);
  testdrawcircles(WHITE);
  HAL_Delay(1000);
  
  testroundrects();
  HAL_Delay(1000);
  
  testtriangles();
  HAL_Delay(1000);
  
  oled.Display_bmp();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

void testlines(void)  {

  oled.Set_Color(RED);
  oled.Clear_Screen();
  for (uint16_t x=0; x <= SSD1351_WIDTH - 1; x+=6)  {
    oled.Draw_Line(0, 0, x, SSD1351_HEIGHT - 1);
    HAL_Delay(20);
  }
  for (uint16_t y=0; y < SSD1351_HEIGHT - 1; y+=6)  {
    oled.Draw_Line(0, 0, SSD1351_WIDTH - 1, y);
    HAL_Delay(20);
  }
  
  oled.Set_Color(YELLOW);
  oled.Clear_Screen();
  for (uint16_t x=0; x < SSD1351_WIDTH - 1; x+=6) {
    oled.Draw_Line(SSD1351_WIDTH - 1, 0, x, SSD1351_HEIGHT - 1);
    HAL_Delay(20);
  }
  for (uint16_t y=0; y < SSD1351_HEIGHT - 1; y+=6) {
    oled.Draw_Line(SSD1351_WIDTH - 1, 0, 0, y);
    HAL_Delay(20);
  }
   
  oled.Set_Color(BLUE);
  oled.Clear_Screen();
  for (uint16_t x=0; x < SSD1351_WIDTH - 1; x+=6) {
    oled.Draw_Line(0, SSD1351_HEIGHT - 1, x, 0);
    HAL_Delay(20);
  }
  for (uint16_t y=0; y < SSD1351_HEIGHT - 1; y+=6) {
    oled.Draw_Line(0, SSD1351_HEIGHT - 1, SSD1351_WIDTH - 1, y);
    HAL_Delay(20);
  }

  oled.Set_Color(GREEN);
  oled.Clear_Screen();
  for (uint16_t x=0; x < SSD1351_WIDTH - 1; x+=6) {
  oled.Draw_Line(SSD1351_WIDTH - 1, SSD1351_HEIGHT - 1, x, 0 );
    HAL_Delay(20);
  }
  for (uint16_t y=0; y < SSD1351_HEIGHT - 1; y+=6) {
    oled.Draw_Line(SSD1351_WIDTH - 1, SSD1351_HEIGHT - 1, 0, y);
    HAL_Delay(20);
  }
}

void lcdTestPattern(void)
{
  uint32_t i,j;
  oled.Set_Coordinate(0, 0);
  
  for(i=0;i<128;i++)  {
    for(j=0;j<128;j++)  {
      if(i<16)  {
        oled.Set_Color(RED);
        oled.Write_Data(color_byte, 2);
      }
      else if(i<32) {
        oled.Set_Color(YELLOW);
        oled.Write_Data(color_byte, 2);
      }
      else if(i<48) {
        oled.Set_Color(GREEN);
        oled.Write_Data(color_byte, 2);
      }
      else if(i<64) {
        oled.Set_Color(CYAN);
        oled.Write_Data(color_byte, 2);
      }
      else if(i<80) {
        oled.Set_Color(BLUE);
        oled.Write_Data(color_byte, 2);
      }
      else if(i<96) {
        oled.Set_Color(MAGENTA);
        oled.Write_Data(color_byte, 2);
      }
      else if(i<112)  {
        oled.Set_Color(BLACK);
        oled.Write_Data(color_byte, 2);
      }
      else {
        oled.Set_Color(WHITE);
        oled.Write_Data(color_byte, 2);
      }
    }
  }
}

void testfastlines(void) {
  
  oled.Set_Color(WHITE);
  oled.Clear_Screen();
  
  for (uint16_t y=0; y < SSD1351_WIDTH - 1; y+=5) {
    oled.Draw_FastHLine(0, y, SSD1351_WIDTH - 1);
    HAL_Delay(10);
  }
  for (uint16_t x=0; x < SSD1351_HEIGHT - 1; x+=5) {
    oled.Draw_FastVLine(x, 0, SSD1351_HEIGHT - 1);
    HAL_Delay(10);
  }
}

void testdrawrects(void)  {
  oled.Clear_Screen();
  for (uint16_t x=0; x < SSD1351_HEIGHT - 1; x+=6)  {
    oled.Draw_Rect((SSD1351_WIDTH-1)/2 - x/2, (SSD1351_HEIGHT-1)/2 - x/2 , x, x);
    HAL_Delay(10);
  }
}

void testfillrects(uint16_t color1, uint16_t color2)  {
  
  uint16_t x = SSD1351_HEIGHT - 1;
  oled.Clear_Screen();
  oled.Set_Color(color1);
  oled.Set_FillColor(color2);
  for(; x > 6; x-=6)  {
    oled.Fill_Rect((SSD1351_WIDTH-1)/2 - x/2, (SSD1351_HEIGHT-1)/2 - x/2 , x, x);
  oled.Draw_Rect((SSD1351_WIDTH-1)/2 - x/2, (SSD1351_HEIGHT-1)/2 - x/2 , x, x);
  }
}

void testfillcircles(uint8_t radius, uint16_t color)  {
  
  oled.Set_Color(color);

  oled.Fill_Circle(64, 64, radius);
  }


void testdrawcircles(uint16_t color)  {
  
  uint8_t r = 0;
  oled.Set_Color(color);
  
  for (; r < SSD1351_WIDTH/2; r+=4)  {
    oled.Draw_Circle(64, 64, r);
    HAL_Delay(10);
  }
}

void testroundrects(void) {

  int color = 100;
  int x = 0, y = 0;
  int w = SSD1351_WIDTH - 1, h = SSD1351_HEIGHT - 1;
  
  oled.Clear_Screen();
  
  for(int i = 0 ; i <= 20; i++) {

    oled.Draw_RoundRect(x, y, w, h, 5);
    x += 2;
    y += 3;
    w -= 4;
    h -= 6;
    color += 1100;
    oled.Set_Color(color);
  }
}

void testtriangles(void)  {
  oled.Clear_Screen();
  int color = 0xF800;
  int t;
  int w = SSD1351_WIDTH/2;
  int x = SSD1351_HEIGHT-1;
  int y = 0;
  int z = SSD1351_WIDTH;
  for(t = 0 ; t <= 15; t+=1) {
    oled.Draw_Triangle(w, y, y, x, z, x);
    x-=4;
    y+=4;
    z-=4;
    color+=100;
    oled.Set_Color(color);
  }
}



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
