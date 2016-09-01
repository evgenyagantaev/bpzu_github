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
        if(led_counter >= 4)
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