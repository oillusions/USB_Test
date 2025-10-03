#pragma once
#include "stm32g0xx_hal.h"
#ifdef __cplusplus
extern "C" {
#endif
    void USB_UCPD1_2_IRQHandler();
    void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd);
    void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd);
    void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd);
    void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd);
    void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd);
    void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd);
    void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd);

    void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
    void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
    void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
    void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
#ifdef __cplusplus
}
#endif

void user_main();

