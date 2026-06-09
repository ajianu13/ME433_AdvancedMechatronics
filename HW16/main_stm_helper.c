uint8_t c;

while (1)
{
    if (HAL_UART_Receive(&huart2, &c, 1, 10) == HAL_OK)
    {
        switch(c)
        {
            case 'a':   // read ADC
                adc_value = read_adc();
                printf("ADC: %u\r\n", adc_value);
                break;

            case 'i':   // read INA219 current
                current_ma = read_ina219();
                printf("INA219: %d mA\r\n", (int)current_ma);
                break;

            case 'p':   // increase PWM
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,
                    __HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_1) + 50);
                printf("PWM1 increased\r\n");
                break;

            case 'o':   // decrease PWM
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,
                    __HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_1) - 50);
                printf("PWM1 decreased\r\n");
                break;

            case 'c':   // CAN transmit test
                txHeader.Identifier = 0x123;
                txHeader.IdType = FDCAN_STANDARD_ID;
                txHeader.TxFrameType = FDCAN_DATA_FRAME;
                txHeader.DataLength = FDCAN_DLC_BYTES_8;
                HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &txHeader, (uint8_t*)txData);
                printf("CAN TX\r\n");
                break;

            default:
                printf("Unknown cmd: %c\r\n", c);
                break;
        }
    }
}
