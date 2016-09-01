/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "adc.h"
#include "usart.h"
#include "gpio.h"
#include "tim.h"
#include <string.h>


// Private constants ---------------------------------------------------------
const uint32_t SYSTICK_RATIO = 200;

// Private variables ---------------------------------------------------------

// GLOBAL FLAGS
int right_board_flag = 0;
int left_board_flag = 0;
int battery_inversion_flag = 0;
//*******************

/* ADC handle declaration */
ADC_HandleTypeDef AdcHandle;

uint8_t message[128];

int zaryad_flag = 1;
int razryad_flag = 0;
int current_not_measured0;
int current_not_measured1;
int current_not_measured2;
int current_not_measured3;
int current_not_measured4;
int current_not_measured5;
int current_not_measured6;
int current_not_measured7;

int pulse_counter = 0;

/* ADC channel configuration structure declaration */
ADC_ChannelConfTypeDef        sConfig;

TIM_HandleTypeDef htim2;
TIM_OC_InitTypeDef sConfigOC;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void Error_Handler(void);
void charge(void);
int test_right_board(void);
int test_akb_polarity(void);
int test_akb_voltage(void);

/* Private function prototypes -----------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//            MAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAIN
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{

	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	//MX_GPIO_Init();
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
	//HAL_PWR_EnterSTANDBYMode();
	//HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();



	MX_ADC_Init();
	AdcHandle.Instance = ADC1;
	MX_USART1_UART_Init();
	huart1.Instance = USART1;
	MX_TIM2_Init();
  

	// ### - 2 - Start calibration ############################################
	if (HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_SINGLE_ENDED) != HAL_OK)
	{
	Error_Handler();
	}
	// Select Channel 0 to be converted
	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
	HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
	sConfig.Channel = ADC_CHANNEL_1 | ADC_CHANNEL_2 | ADC_CHANNEL_3 | ADC_CHANNEL_5;
	sConfig.Rank = ADC_RANK_NONE;
	HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
  
    // connect battery
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);

    // red off
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
    
    
    
    volatile long i = 0;
    
    // vklyuchaem vhodnye napryazheniya (ONVIN pa12)
    HAL_GPIO_WritePin(GPIOA, onvin_out_pin, GPIO_PIN_SET);
    right_board_flag = !((int)HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0));
    left_board_flag = !((int)HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1));
    // main loop *******************************************************************************************
    // main loop *******************************************************************************************
    // main loop *******************************************************************************************
    // main loop *******************************************************************************************
    // main loop *******************************************************************************************
    while(1)
    {
        charge();
    }
    // end main loop ***************************************************************************************
    // end main loop ***************************************************************************************
    // end main loop ***************************************************************************************
    // end main loop ***************************************************************************************
    // end main loop ***************************************************************************************
    
    
    while(1)
    {
        /*
        while(1)
        {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
            HAL_Delay(500*SYSTICK_RATIO);
        }
        */
        
        // high level - net napryazheniya, low level - est' napryazhenie
        right_board_flag = !((int)HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0));
        
        if(right_board_flag) // est' napryazhenie na pravom
        {
            // Select Channel 5 to be converted (V inv)
            sConfig.Channel = ADC_CHANNEL_5; 
            sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
            HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
            sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_2 | ADC_CHANNEL_3; 
            sConfig.Rank = ADC_RANK_NONE;
            HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
            //***
            HAL_ADC_Start(&AdcHandle);
            if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
            {
                Error_Handler();
            }
            uint32_t adc_ch5 = HAL_ADC_GetValue(&AdcHandle);
            HAL_ADC_Stop(&AdcHandle);
            uint32_t vinv = adc_ch5 * 3300 * 151 / 4096 / 51;
            if(vinv >= 5000)
                battery_inversion_flag = 1;
            else
                battery_inversion_flag = 0;
            
            while(battery_inversion_flag)  // Battery inversion loop!!!!
            {
                // turn off everything (except onvin)
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
                
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
                HAL_Delay(70*SYSTICK_RATIO);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
                HAL_Delay(200*SYSTICK_RATIO);
                
                // Select Channel 5 to be converted (V inv)
                sConfig.Channel = ADC_CHANNEL_5; 
                sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
                HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
                sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_2 | ADC_CHANNEL_3; 
                sConfig.Rank = ADC_RANK_NONE;
                HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
                //***
                HAL_ADC_Start(&AdcHandle);
                if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
                {
                    Error_Handler();
                }
                uint32_t adc_ch5 = HAL_ADC_GetValue(&AdcHandle);
                HAL_ADC_Stop(&AdcHandle);
                uint32_t vinv = adc_ch5 * 3300 * 151 / 4096 / 51;
                if(vinv >= 5000)
                    battery_inversion_flag = 1;
                else
                    battery_inversion_flag = 0;
            }//end while(battery_inversion_flag)
            
            // zaryad ***********************
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
            razryad_flag = 0;
                
            //HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
            //HAL_Delay(100*SYSTICK_RATIO);
            //HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
            //HAL_Delay(500*SYSTICK_RATIO);
            
            charge();
            
            
        }//end if(right_board_flag)
        else // net pravogo borta
        {
            //probe if there is a left board
            left_board_flag = 0;
            for(i=0; i<10; i++)
            {
                // high level - net napryazheniya, low level - est' napryazhenie
                if((int)HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == 0)
                    left_board_flag = 1;
                HAL_Delay(10*SYSTICK_RATIO);
            }
            
            if(!left_board_flag) // net levogo borta
            {
                // off leds
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
                // rabota na istochnik
                // set pa9 (vkl tok razryada)
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
                razryad_flag = 1;
                HAL_Delay(100*SYSTICK_RATIO);
            }
            else //est' levyi bort
            {
                // otklyuchaem istochnik
                // reset pa9 (vykl tok razryada)
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
                razryad_flag = 0;
                HAL_Delay(100*SYSTICK_RATIO);
                
                // sleep
                __WFI();
            }
        }//end else [if(right_board_flag)]
        
        
        //sprintf((char *)message, "pb=%d; lb=%d\r\n", \
            (int)HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0), (int)HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1));
        //HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
        //HAL_Delay(100*SYSTICK_RATIO);
    }// end main loop **************************************************************************************
    
    
    
    while(1)
    {
        //int16_t T = readThemperature();
        //sprintf((char *)message, "T = %7.1f\r\n", (float)((int16_t)T)/16.0);
        //HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
    }
    
    
    
    
    
    while (1)
    {
        
        
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
        HAL_Delay(200);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
        HAL_Delay(200);
        
        //pulse += inc_dec;
        //TIM2->CCR2 = pulse;
        //if(pulse >= 320)
            //inc_dec = -10;
        //else if(pulse <= 10)
            //inc_dec = 10;
        
        // Select Channel 0 to be converted (V battery)
        sConfig.Channel = ADC_CHANNEL_0; 
        sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
        HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
        sConfig.Channel = ADC_CHANNEL_1 | ADC_CHANNEL_2 | ADC_CHANNEL_3 | ADC_CHANNEL_5; 
        sConfig.Rank = ADC_RANK_NONE;
        HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
        //***
        HAL_ADC_Start(&AdcHandle);
        if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
        {
            Error_Handler();
        }
        uint32_t adc_ch0 = HAL_ADC_GetValue(&AdcHandle);
        uint32_t vbatt = adc_ch0 * 3300 * 151 / 4096 / 51;
        HAL_ADC_Stop(&AdcHandle);
        sprintf((char *)message, "vbatt = %d mV\r\n", vbatt);
        HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
        //-------------------------------------------------------------------
        // Select Channel 1 to be converted (V out)
        sConfig.Channel = ADC_CHANNEL_1; 
        sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
        HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
        sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_2 | ADC_CHANNEL_3 | ADC_CHANNEL_5; 
        sConfig.Rank = ADC_RANK_NONE;
        HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
        //***
        HAL_ADC_Start(&AdcHandle);
        if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
        {
            Error_Handler();
        }
        uint32_t adc_ch1 = HAL_ADC_GetValue(&AdcHandle);
        uint32_t vout = adc_ch1 * 3300 * 151 / 4096 / 51;
        HAL_ADC_Stop(&AdcHandle);
        sprintf((char *)message, "vout = %d mV\r\n", vout);
        HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
        //-------------------------------------------------------------------
        // Select Channel 2 to be converted (I zar)
        sConfig.Channel = ADC_CHANNEL_2; 
        sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
        HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
        sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5; 
        sConfig.Rank = ADC_RANK_NONE;
        HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
        //***
        HAL_ADC_Start(&AdcHandle);
        if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
        {
            Error_Handler();
        }
        uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
        HAL_ADC_Stop(&AdcHandle);
        uint32_t izar = adc_ch2 * 1000 / 2985;
        sprintf((char *)message, "Izar = %d mA\r\n", izar);
        HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
        //-------------------------------------------------------------------
        // Select Channel 3 to be converted (I razr)
        sConfig.Channel = ADC_CHANNEL_3; 
        sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
        HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
        sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_2 | ADC_CHANNEL_5; 
        sConfig.Rank = ADC_RANK_NONE;
        HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
        //***
        HAL_ADC_Start(&AdcHandle);
        if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
        {
            Error_Handler();
        }
        uint32_t adc_ch3 = HAL_ADC_GetValue(&AdcHandle);
        HAL_ADC_Stop(&AdcHandle);
        uint32_t irazr = adc_ch3 * 1000 / 2695;
        sprintf((char *)message, "Irazr = %d mA\r\n", irazr);
        HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
        //-------------------------------------------------------------------
        // Select Channel 5 to be converted (V inv)
         sConfig.Channel = ADC_CHANNEL_5; 
        sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
        HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
        sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_2 | ADC_CHANNEL_3; 
        sConfig.Rank = ADC_RANK_NONE;
        HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
        //***
        HAL_ADC_Start(&AdcHandle);
        if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
        {
            Error_Handler();
        }
        uint32_t adc_ch5 = HAL_ADC_GetValue(&AdcHandle);
        HAL_ADC_Stop(&AdcHandle);
        uint32_t vinv = adc_ch5 * 3300 * 151 / 4096 / 51;
        sprintf((char *)message, "Vinv = %d mV\r\n", vinv);
        HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
        //-------------------------------------------------------------------
        sprintf((char *)message, "pa4=%d; pb0=%d; pb1=%d\r\n", \
            (int)HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4), (int)HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0), (int)HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1));
        HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
    }

}// end main

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                        END MAIN
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int test_right_board()
{
    int right_board_flag;
    // high level - net napryazheniya, low level - est' napryazhenie
    right_board_flag = !((int)HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0));
    
    return right_board_flag;
}

int test_left_board()
{
    int i;
    int left_board_flag;
    //probe if there is a left board
    left_board_flag = 0;
    for(i=0; i<10; i++)
    {
        // high level - net napryazheniya, low level - est' napryazhenie
        if((int)HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == 0)
            left_board_flag = 1;
        HAL_Delay(10*SYSTICK_RATIO);
    }
    
    return left_board_flag;
}

int test_akb_polarity()
{
    int battery_inversion_ok;
    
    // Select Channel 5 to be converted (V inv)
    sConfig.Channel = ADC_CHANNEL_5; 
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
    sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_2 | ADC_CHANNEL_3; 
    sConfig.Rank = ADC_RANK_NONE;
    HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
    //***
    HAL_ADC_Start(&AdcHandle);
    if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
    {
        Error_Handler();
    }
    uint32_t adc_ch5 = HAL_ADC_GetValue(&AdcHandle);
    HAL_ADC_Stop(&AdcHandle);
    uint32_t vinv = adc_ch5 * 3300 * 151 / 4096 / 51;
    if(vinv >= 5000)
        battery_inversion_ok = 0;
    else
    	battery_inversion_ok = 1;
    
    return battery_inversion_ok;
}
int test_akb_voltage()
{
    int akb_voltage_flag;
    // Select Channel 0 to be converted (V battery)
    sConfig.Channel = ADC_CHANNEL_0; 
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
    sConfig.Channel = ADC_CHANNEL_1 | ADC_CHANNEL_2 | ADC_CHANNEL_3 | ADC_CHANNEL_5; 
    sConfig.Rank = ADC_RANK_NONE;
    HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
    //***
    HAL_ADC_Start(&AdcHandle);
    if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
    {
        Error_Handler();
    }
    uint32_t adc_ch0 = HAL_ADC_GetValue(&AdcHandle);
    uint32_t vbatt = adc_ch0 * 3300 * 151 / 4096 / 51;
    HAL_ADC_Stop(&AdcHandle);
    //sprintf((char *)message, "vbatt = %d mV\r\n", vbatt);
    //HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
    if(vbatt >= 5000)
        akb_voltage_flag = 1;
    else
        akb_voltage_flag = 0;
    
    return akb_voltage_flag;
}


void charge()
{
    int do_main_loop = 1;
    int right_board_ok = 0;
    int akb_polarity_ok = 0;
    int akb_voltage_ok = 0;
    // opredelim schetchik pereklyucheniya led
    int led_counter = 0;  // on 100 mS, off 400 mS
    int green_led_on_flag = 0;
    
    // tok zaryada
    uint32_t izar = 0;
    
    // charge main loop
    while(do_main_loop)
    {
        // test right board
        right_board_ok = test_right_board();
        
        if(right_board_ok)
        {
            // test akb polarity
            akb_polarity_ok = test_akb_polarity();
            if(akb_polarity_ok)
            {
                // test akb voltage
                akb_voltage_ok = test_akb_voltage();
                if(akb_voltage_ok)
                {
                	// red led off
                	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
                    // start regular charge cycle  **************************************
                    // opredelim peremennuyu, v kotoroi budem hranit' shirinu impulsa
                    int pulse = 160; // ves' period pwm - 640 tikov (chastota timera - 32MHz)
                    TIM2->CCR2 = pulse;
                    //#include "charge_.inc"
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
					if(pulse_counter >= 100*SYSTICK_RATIO)
					{
						pulse_counter = 0;
						if(green_led_on_flag)
						{
							led_counter = 0;
							// led off
							HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
							green_led_on_flag = 0;
						}
						else
						{
							led_counter++;
							if(led_counter >= 10)
							{
								led_counter = 0;
								// led off
								HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
								green_led_on_flag = 1;
							}
						}

						current_not_measured0 = 1;
						current_not_measured1 = 1;
						current_not_measured2 = 1;
						current_not_measured3 = 1;
						current_not_measured4 = 1;
						current_not_measured5 = 1;
						current_not_measured6 = 1;
						current_not_measured7 = 1;
					}

					if(pulse_counter >= 0 && pulse_counter < 90*SYSTICK_RATIO && !zaryad_flag)
					{
						if(izar <= 1140)
						{
							if(pulse <= 450)
								pulse++;
							else
								pulse = 451;
						}
						if(izar >= 1180)
						{
							if(pulse >= 11)
								pulse--;
							else
								pulse = 10;
						}

						// start pwm
						TIM2->CCR2 = pulse;
						if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2) != HAL_OK)
						{
							/* PWM Generation Error */
							Error_Handler();
						}

						zaryad_flag = 1;

					}
					else if(pulse_counter >= 90*SYSTICK_RATIO && pulse_counter < 92*SYSTICK_RATIO && zaryad_flag)
					{
						zaryad_flag = 0;
						// stop pwm
						if (HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2) != HAL_OK)
						{
							/* PWM Generation Error */
							Error_Handler();
						}
					}
					else if(pulse_counter >= 92*SYSTICK_RATIO && pulse_counter < 94*SYSTICK_RATIO && !zaryad_flag && !razryad_flag)
					{
						// set pa9 (vkl tok razryada)
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
						razryad_flag = 1;
					}
					else if(pulse_counter >= 94*SYSTICK_RATIO && pulse_counter < 100*SYSTICK_RATIO && razryad_flag)
					{
						// reset pa9 (vykl tok razryada)
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
						razryad_flag = 0;
					}


					if(pulse_counter >= 10*SYSTICK_RATIO && pulse_counter < 15*SYSTICK_RATIO && current_not_measured0)
					{
						//-------------------------------------------------------------------
						// Select Channel 2 to be converted (I zar)
						sConfig.Channel = ADC_CHANNEL_2;
						sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5;
						sConfig.Rank = ADC_RANK_NONE;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						//***
						HAL_ADC_Start(&AdcHandle);
						if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
						{
							Error_Handler();
						}
						uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
						HAL_ADC_Stop(&AdcHandle);
						izar = adc_ch2 * 1000 / 2985;
						//sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
						//HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
						//-------------------------------------------------------------------

						current_not_measured0 = 0;
					}
					if(pulse_counter >= 20*SYSTICK_RATIO && pulse_counter < 25*SYSTICK_RATIO && current_not_measured1)
					{
						//-------------------------------------------------------------------
						// Select Channel 2 to be converted (I zar)
						sConfig.Channel = ADC_CHANNEL_2;
						sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5;
						sConfig.Rank = ADC_RANK_NONE;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						//***
						HAL_ADC_Start(&AdcHandle);
						if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
						{
							Error_Handler();
						}
						uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
						HAL_ADC_Stop(&AdcHandle);
						izar += adc_ch2 * 1000 / 2985;
						//sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
						//HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
						//-------------------------------------------------------------------

						current_not_measured1 = 0;
					}
					if(pulse_counter >= 30*SYSTICK_RATIO && pulse_counter < 35*SYSTICK_RATIO && current_not_measured2)
					{
						//-------------------------------------------------------------------
						// Select Channel 2 to be converted (I zar)
						sConfig.Channel = ADC_CHANNEL_2;
						sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5;
						sConfig.Rank = ADC_RANK_NONE;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						//***
						HAL_ADC_Start(&AdcHandle);
						if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
						{
							Error_Handler();
						}
						uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
						HAL_ADC_Stop(&AdcHandle);
						izar += adc_ch2 * 1000 / 2985;
						//sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
						//HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
						//-------------------------------------------------------------------

						current_not_measured2 = 0;
					}
					if(pulse_counter >= 40*SYSTICK_RATIO && pulse_counter < 45*SYSTICK_RATIO && current_not_measured3)
					{
						//-------------------------------------------------------------------
						// Select Channel 2 to be converted (I zar)
						sConfig.Channel = ADC_CHANNEL_2;
						sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5;
						sConfig.Rank = ADC_RANK_NONE;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						//***
						HAL_ADC_Start(&AdcHandle);
						if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
						{
							Error_Handler();
						}
						uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
						HAL_ADC_Stop(&AdcHandle);
						izar += adc_ch2 * 1000 / 2985;
						//sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
						//HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
						//-------------------------------------------------------------------

						current_not_measured3 = 0;
					}
					if(pulse_counter >= 50*SYSTICK_RATIO && pulse_counter < 55*SYSTICK_RATIO && current_not_measured4)
					{
						//-------------------------------------------------------------------
						// Select Channel 2 to be converted (I zar)
						sConfig.Channel = ADC_CHANNEL_2;
						sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5;
						sConfig.Rank = ADC_RANK_NONE;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						//***
						HAL_ADC_Start(&AdcHandle);
						if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
						{
							Error_Handler();
						}
						uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
						HAL_ADC_Stop(&AdcHandle);
						izar += adc_ch2 * 1000 / 2985;
						//sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
						//HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
						//-------------------------------------------------------------------

						current_not_measured4 = 0;
					}
					if(pulse_counter >= 60*SYSTICK_RATIO && pulse_counter < 65*SYSTICK_RATIO && current_not_measured5)
					{
						//-------------------------------------------------------------------
						// Select Channel 2 to be converted (I zar)
						sConfig.Channel = ADC_CHANNEL_2;
						sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5;
						sConfig.Rank = ADC_RANK_NONE;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						//***
						HAL_ADC_Start(&AdcHandle);
						if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
						{
							Error_Handler();
						}
						uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
						HAL_ADC_Stop(&AdcHandle);
						izar += adc_ch2 * 1000 / 2985;
						//sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
						//HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
						//-------------------------------------------------------------------

						current_not_measured5 = 0;
					}
					if(pulse_counter >= 70*SYSTICK_RATIO && pulse_counter < 75*SYSTICK_RATIO && current_not_measured6)
					{
						//-------------------------------------------------------------------
						// Select Channel 2 to be converted (I zar)
						sConfig.Channel = ADC_CHANNEL_2;
						sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5;
						sConfig.Rank = ADC_RANK_NONE;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						//***
						HAL_ADC_Start(&AdcHandle);
						if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
						{
							Error_Handler();
						}
						uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
						HAL_ADC_Stop(&AdcHandle);
						izar += adc_ch2 * 1000 / 2985;
						//sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
						//HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
						//-------------------------------------------------------------------

						current_not_measured6 = 0;
					}
					if(pulse_counter >= 80*SYSTICK_RATIO && pulse_counter < 85*SYSTICK_RATIO && current_not_measured7)
					{
						//-------------------------------------------------------------------
						// Select Channel 2 to be converted (I zar)
						sConfig.Channel = ADC_CHANNEL_2;
						sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5;
						sConfig.Rank = ADC_RANK_NONE;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						//***
						HAL_ADC_Start(&AdcHandle);
						if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
						{
							Error_Handler();
						}
						uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
						HAL_ADC_Stop(&AdcHandle);
						izar += adc_ch2 * 1000 / 2985;
						izar /= 8;
						sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
						HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
						//-------------------------------------------------------------------

						current_not_measured7 = 0;
					}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                }
                else
                {
                    // akb voltage bad;
                	// green led off
                	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
                    // start low current charge cycle  **************************************
                    // opredelim peremennuyu, v kotoroi budem hranit' shirinu impulsa
                    int pulse = 160; // ves' period pwm - 640 tikov (chastota timera - 32MHz)
                    TIM2->CCR2 = pulse;
                    //#include "charge_light_.inc"
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
					if(pulse_counter >= 100*SYSTICK_RATIO)
					{
						pulse_counter = 0;
						if(green_led_on_flag)
						{
							led_counter = 0;
							// red led off
							HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
							green_led_on_flag = 0;
						}
						else
						{
							led_counter++;
							if(led_counter >= 10)
							{
								led_counter = 0;
								// red led on
								HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
								green_led_on_flag = 1;
							}
						}

						current_not_measured0 = 1;
						current_not_measured1 = 1;
						current_not_measured2 = 1;
						current_not_measured3 = 1;
						current_not_measured4 = 1;
						current_not_measured5 = 1;
						current_not_measured6 = 1;
						current_not_measured7 = 1;
					}

					if(pulse_counter >= 0 && pulse_counter < 90*SYSTICK_RATIO && !zaryad_flag)
					{
						if(izar <= 640)
						{
							if(pulse <= 450)
								pulse++;
							else
								pulse = 451;
						}
						if(izar >= 680)
						{
							if(pulse >= 11)
								pulse--;
							else
								pulse = 10;
						}

						// start pwm
						TIM2->CCR2 = pulse;
						if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2) != HAL_OK)
						{
							/* PWM Generation Error */
							Error_Handler();
						}

						zaryad_flag = 1;

					}
					else if(pulse_counter >= 90*SYSTICK_RATIO && pulse_counter < 92*SYSTICK_RATIO && zaryad_flag)
					{
						zaryad_flag = 0;
						// stop pwm
						if (HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2) != HAL_OK)
						{
							/* PWM Generation Error */
							Error_Handler();
						}
					}
					else if(pulse_counter >= 92*SYSTICK_RATIO && pulse_counter < 94*SYSTICK_RATIO && !zaryad_flag && !razryad_flag)
					{
						// set pa9 (vkl tok razryada)
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
						razryad_flag = 1;
					}
					else if(pulse_counter >= 94*SYSTICK_RATIO && pulse_counter < 100*SYSTICK_RATIO && razryad_flag)
					{
						// reset pa9 (vykl tok razryada)
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
						razryad_flag = 0;
					}


					if(pulse_counter >= 10*SYSTICK_RATIO && pulse_counter < 15*SYSTICK_RATIO && current_not_measured0)
					{
						//-------------------------------------------------------------------
						// Select Channel 2 to be converted (I zar)
						sConfig.Channel = ADC_CHANNEL_2;
						sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5;
						sConfig.Rank = ADC_RANK_NONE;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						//***
						HAL_ADC_Start(&AdcHandle);
						if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
						{
							Error_Handler();
						}
						uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
						HAL_ADC_Stop(&AdcHandle);
						izar = adc_ch2 * 1000 / 2985;
						//sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
						//HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
						//-------------------------------------------------------------------

						current_not_measured0 = 0;
					}
					if(pulse_counter >= 20*SYSTICK_RATIO && pulse_counter < 25*SYSTICK_RATIO && current_not_measured1)
					{
						//-------------------------------------------------------------------
						// Select Channel 2 to be converted (I zar)
						sConfig.Channel = ADC_CHANNEL_2;
						sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5;
						sConfig.Rank = ADC_RANK_NONE;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						//***
						HAL_ADC_Start(&AdcHandle);
						if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
						{
							Error_Handler();
						}
						uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
						HAL_ADC_Stop(&AdcHandle);
						izar += adc_ch2 * 1000 / 2985;
						//sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
						//HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
						//-------------------------------------------------------------------

						current_not_measured1 = 0;
					}
					if(pulse_counter >= 30*SYSTICK_RATIO && pulse_counter < 35*SYSTICK_RATIO && current_not_measured2)
					{
						//-------------------------------------------------------------------
						// Select Channel 2 to be converted (I zar)
						sConfig.Channel = ADC_CHANNEL_2;
						sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5;
						sConfig.Rank = ADC_RANK_NONE;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						//***
						HAL_ADC_Start(&AdcHandle);
						if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
						{
							Error_Handler();
						}
						uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
						HAL_ADC_Stop(&AdcHandle);
						izar += adc_ch2 * 1000 / 2985;
						//sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
						//HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
						//-------------------------------------------------------------------

						current_not_measured2 = 0;
					}
					if(pulse_counter >= 40*SYSTICK_RATIO && pulse_counter < 45*SYSTICK_RATIO && current_not_measured3)
					{
						//-------------------------------------------------------------------
						// Select Channel 2 to be converted (I zar)
						sConfig.Channel = ADC_CHANNEL_2;
						sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5;
						sConfig.Rank = ADC_RANK_NONE;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						//***
						HAL_ADC_Start(&AdcHandle);
						if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
						{
							Error_Handler();
						}
						uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
						HAL_ADC_Stop(&AdcHandle);
						izar += adc_ch2 * 1000 / 2985;
						//sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
						//HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
						//-------------------------------------------------------------------

						current_not_measured3 = 0;
					}
					if(pulse_counter >= 50*SYSTICK_RATIO && pulse_counter < 55*SYSTICK_RATIO && current_not_measured4)
					{
						//-------------------------------------------------------------------
						// Select Channel 2 to be converted (I zar)
						sConfig.Channel = ADC_CHANNEL_2;
						sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5;
						sConfig.Rank = ADC_RANK_NONE;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						//***
						HAL_ADC_Start(&AdcHandle);
						if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
						{
							Error_Handler();
						}
						uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
						HAL_ADC_Stop(&AdcHandle);
						izar += adc_ch2 * 1000 / 2985;
						//sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
						//HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
						//-------------------------------------------------------------------

						current_not_measured4 = 0;
					}
					if(pulse_counter >= 60*SYSTICK_RATIO && pulse_counter < 65*SYSTICK_RATIO && current_not_measured5)
					{
						//-------------------------------------------------------------------
						// Select Channel 2 to be converted (I zar)
						sConfig.Channel = ADC_CHANNEL_2;
						sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5;
						sConfig.Rank = ADC_RANK_NONE;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						//***
						HAL_ADC_Start(&AdcHandle);
						if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
						{
							Error_Handler();
						}
						uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
						HAL_ADC_Stop(&AdcHandle);
						izar += adc_ch2 * 1000 / 2985;
						//sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
						//HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
						//-------------------------------------------------------------------

						current_not_measured5 = 0;
					}
					if(pulse_counter >= 70*SYSTICK_RATIO && pulse_counter < 75*SYSTICK_RATIO && current_not_measured6)
					{
						//-------------------------------------------------------------------
						// Select Channel 2 to be converted (I zar)
						sConfig.Channel = ADC_CHANNEL_2;
						sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5;
						sConfig.Rank = ADC_RANK_NONE;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						//***
						HAL_ADC_Start(&AdcHandle);
						if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
						{
							Error_Handler();
						}
						uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
						HAL_ADC_Stop(&AdcHandle);
						izar += adc_ch2 * 1000 / 2985;
						//sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
						//HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
						//-------------------------------------------------------------------

						current_not_measured6 = 0;
					}
					if(pulse_counter >= 80*SYSTICK_RATIO && pulse_counter < 85*SYSTICK_RATIO && current_not_measured7)
					{
						//-------------------------------------------------------------------
						// Select Channel 2 to be converted (I zar)
						sConfig.Channel = ADC_CHANNEL_2;
						sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5;
						sConfig.Rank = ADC_RANK_NONE;
						HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
						//***
						HAL_ADC_Start(&AdcHandle);
						if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
						{
							Error_Handler();
						}
						uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
						HAL_ADC_Stop(&AdcHandle);
						izar += adc_ch2 * 1000 / 2985;
						izar /= 8;
						sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
						HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
						//-------------------------------------------------------------------

						current_not_measured7 = 0;
					}

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                }
            }
            else
            {
                // akb polarity bad; cancel charge
                do_main_loop = 0;
            }
        }
        else
        {
            // no right board; cancel charge
            do_main_loop = 0;
        }
    }
    // end charge main loop
}
// end charge()

void charge_()
{
    int i = 0;
    
    // tok zaryada
    uint32_t izar = 0;
    // Set the Capture Compare Register value 
    int pulse = 160;
    TIM2->CCR2 = pulse;
    
    int led_counter = 0;
    
    while(1)
    {
        if(pulse_counter >= 100*SYSTICK_RATIO)
        {
            pulse_counter = 0;
            led_counter++;
            if(led_counter >= 10)
            {
                led_counter = 0;
                HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
            }
            
            current_not_measured0 = 1;
            current_not_measured1 = 1;
            current_not_measured2 = 1;
            current_not_measured3 = 1;
            current_not_measured4 = 1;
            current_not_measured5 = 1;
            current_not_measured6 = 1;
            current_not_measured7 = 1;
        }
        
        if(pulse_counter >= 0 && pulse_counter < 90*SYSTICK_RATIO && !zaryad_flag)
        {
            if(izar <= 1140)
            {
                if(pulse <= 450)
                    pulse++;
                else
                    pulse = 451;
            }
            if(izar >= 1180)
            {
                if(pulse >= 11)
                    pulse--;
                else
                    pulse = 10;
            }
            
            // start pwm
            TIM2->CCR2 = pulse;
            if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2) != HAL_OK)
            {
                /* PWM Generation Error */
                Error_Handler();
            }
            
            zaryad_flag = 1;
            
        }
        else if(pulse_counter >= 90*SYSTICK_RATIO && pulse_counter < 92*SYSTICK_RATIO && zaryad_flag)
        {
            zaryad_flag = 0;
            // stop pwm
            if (HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2) != HAL_OK)
            {
                /* PWM Generation Error */
                Error_Handler();
            }
        }
        else if(pulse_counter >= 92*SYSTICK_RATIO && pulse_counter < 94*SYSTICK_RATIO && !zaryad_flag && !razryad_flag)
        {
            // set pa9 (vkl tok razryada)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
            razryad_flag = 1;
        }
        else if(pulse_counter >= 94*SYSTICK_RATIO && pulse_counter < 100*SYSTICK_RATIO && razryad_flag)
        {
            // reset pa9 (vykl tok razryada)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
            razryad_flag = 0;
        }
        
                    
        if(pulse_counter >= 10*SYSTICK_RATIO && pulse_counter < 15*SYSTICK_RATIO && current_not_measured0)
        {
            //-------------------------------------------------------------------
            // Select Channel 2 to be converted (I zar)
            sConfig.Channel = ADC_CHANNEL_2; 
            sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
            HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
            sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5; 
            sConfig.Rank = ADC_RANK_NONE;
            HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
            //***
            HAL_ADC_Start(&AdcHandle);
            if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
            {
                Error_Handler();
            }
            uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
            HAL_ADC_Stop(&AdcHandle);
            izar = adc_ch2 * 1000 / 2985;
            //sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
            //HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
            //-------------------------------------------------------------------
            
            current_not_measured0 = 0;
        }
        if(pulse_counter >= 20*SYSTICK_RATIO && pulse_counter < 25*SYSTICK_RATIO && current_not_measured1)
        {
            //-------------------------------------------------------------------
            // Select Channel 2 to be converted (I zar)
            sConfig.Channel = ADC_CHANNEL_2; 
            sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
            HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
            sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5; 
            sConfig.Rank = ADC_RANK_NONE;
            HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
            //***
            HAL_ADC_Start(&AdcHandle);
            if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
            {
                Error_Handler();
            }
            uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
            HAL_ADC_Stop(&AdcHandle);
            izar += adc_ch2 * 1000 / 2985;
            //sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
            //HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
            //-------------------------------------------------------------------
            
            current_not_measured1 = 0;
        }
        if(pulse_counter >= 30*SYSTICK_RATIO && pulse_counter < 35*SYSTICK_RATIO && current_not_measured2)
        {
            //-------------------------------------------------------------------
            // Select Channel 2 to be converted (I zar)
            sConfig.Channel = ADC_CHANNEL_2; 
            sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
            HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
            sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5; 
            sConfig.Rank = ADC_RANK_NONE;
            HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
            //***
            HAL_ADC_Start(&AdcHandle);
            if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
            {
                Error_Handler();
            }
            uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
            HAL_ADC_Stop(&AdcHandle);
            izar += adc_ch2 * 1000 / 2985;
            //sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
            //HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
            //-------------------------------------------------------------------
            
            current_not_measured2 = 0;
        }
        if(pulse_counter >= 40*SYSTICK_RATIO && pulse_counter < 45*SYSTICK_RATIO && current_not_measured3)
        {
            //-------------------------------------------------------------------
            // Select Channel 2 to be converted (I zar)
            sConfig.Channel = ADC_CHANNEL_2; 
            sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
            HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
            sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5; 
            sConfig.Rank = ADC_RANK_NONE;
            HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
            //***
            HAL_ADC_Start(&AdcHandle);
            if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
            {
                Error_Handler();
            }
            uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
            HAL_ADC_Stop(&AdcHandle);
            izar += adc_ch2 * 1000 / 2985;
            //sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
            //HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
            //-------------------------------------------------------------------
            
            current_not_measured3 = 0;
        }
        if(pulse_counter >= 50*SYSTICK_RATIO && pulse_counter < 55*SYSTICK_RATIO && current_not_measured4)
        {
            //-------------------------------------------------------------------
            // Select Channel 2 to be converted (I zar)
            sConfig.Channel = ADC_CHANNEL_2; 
            sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
            HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
            sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5; 
            sConfig.Rank = ADC_RANK_NONE;
            HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
            //***
            HAL_ADC_Start(&AdcHandle);
            if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
            {
                Error_Handler();
            }
            uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
            HAL_ADC_Stop(&AdcHandle);
            izar += adc_ch2 * 1000 / 2985;
            //sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
            //HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
            //-------------------------------------------------------------------
            
            current_not_measured4 = 0;
        }
        if(pulse_counter >= 60*SYSTICK_RATIO && pulse_counter < 65*SYSTICK_RATIO && current_not_measured5)
        {
            //-------------------------------------------------------------------
            // Select Channel 2 to be converted (I zar)
            sConfig.Channel = ADC_CHANNEL_2; 
            sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
            HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
            sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5; 
            sConfig.Rank = ADC_RANK_NONE;
            HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
            //***
            HAL_ADC_Start(&AdcHandle);
            if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
            {
                Error_Handler();
            }
            uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
            HAL_ADC_Stop(&AdcHandle);
            izar += adc_ch2 * 1000 / 2985;
            //sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
            //HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
            //-------------------------------------------------------------------
            
            current_not_measured5 = 0;
        }
        if(pulse_counter >= 70*SYSTICK_RATIO && pulse_counter < 75*SYSTICK_RATIO && current_not_measured6)
        {
            //-------------------------------------------------------------------
            // Select Channel 2 to be converted (I zar)
            sConfig.Channel = ADC_CHANNEL_2; 
            sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
            HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
            sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5; 
            sConfig.Rank = ADC_RANK_NONE;
            HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
            //***
            HAL_ADC_Start(&AdcHandle);
            if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
            {
                Error_Handler();
            }
            uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
            HAL_ADC_Stop(&AdcHandle);
            izar += adc_ch2 * 1000 / 2985;
            //sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
            //HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
            //-------------------------------------------------------------------
            
            current_not_measured6 = 0;
        }
        if(pulse_counter >= 80*SYSTICK_RATIO && pulse_counter < 85*SYSTICK_RATIO && current_not_measured7)
        {
            //-------------------------------------------------------------------
            // Select Channel 2 to be converted (I zar)
            sConfig.Channel = ADC_CHANNEL_2; 
            sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
            HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
            sConfig.Channel = ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_3 | ADC_CHANNEL_5; 
            sConfig.Rank = ADC_RANK_NONE;
            HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
            //***
            HAL_ADC_Start(&AdcHandle);
            if(HAL_ADC_PollForConversion(&AdcHandle, 1000) != HAL_OK)
            {
                Error_Handler();
            }
            uint32_t adc_ch2 = HAL_ADC_GetValue(&AdcHandle);
            HAL_ADC_Stop(&AdcHandle);
            izar += adc_ch2 * 1000 / 2985;
            izar /= 8;
            sprintf((char *)message, "Izar = %7d mA   PWM = %3d\r\n", izar, pulse);
            HAL_UART_Transmit(&huart1, message, strlen((const char *)message), 500);
            //-------------------------------------------------------------------
            
            current_not_measured7 = 0;
        }
        
        
    }//end while
    
}// end charge






/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/(SYSTICK_RATIO * 1000));  // systick 5 uSec

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * @brief EXTI line detection callback.
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static int odd_even = 0;
    
    if(GPIO_Pin == GPIO_PIN_15)
    {
        HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);
        
        if(odd_even == 0)
        {
            odd_even++;
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
            zaryad_flag = 0;
            /* Stop channel 2 */
            if (HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2) != HAL_OK)
            {
                /* PWM Generation Error */
                Error_Handler();
            }
        }
        else
        {
            odd_even = 0;
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
            zaryad_flag = 1;
            /* Start channel 2 */
            if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2) != HAL_OK)
            {
                /* PWM Generation Error */
                Error_Handler();
            }
        }
        HAL_Delay(1000);
        
        HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
        
    }
  
}

static void Error_Handler(void)
{
    /* Infinite loop */
    while(1)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
        HAL_Delay(300);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
        HAL_Delay(300);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
        HAL_Delay(300);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
        HAL_Delay(300);
    }
}


void load()
{
	// proveryaem pravyi bort

}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}




#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
