/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//Incluimos la librería
#include "usbd_hid.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM4_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*********************************VARIABLES PARA LA LÓGICA DEL PROGRAMA**********************/
uint8_t modoFuncionamiento = 1; //1 Word 2STMCubeIde
uint8_t modoSeleccion = 0; //Esta variable se activa cuando se presiona la tecla del encoder rotativo para cambiar el modoFuncionamiento
const uint8_t MAX_MODO = 2; //Esta variable se encarga de fijar el número máximo de modos que tiene nuestro encode (Word STMCubeIde....)
/********************************************************************************************/

/*********************************CONFIGURACION DEL TECLADO USB******************************/
//Seleccionamos el Handle del USB como externo
extern USBD_HandleTypeDef hUsbDeviceFS;

//Creamos la estructura del teclado
typedef struct
{
	uint8_t MODIFIER;
	uint8_t RESERVED;
	uint8_t KEYCODE1;
	uint8_t KEYCODE2;
	uint8_t KEYCODE3;
	uint8_t KEYCODE4;
	uint8_t KEYCODE5;
	uint8_t KEYCODE6;
} keyBoardHID;

keyBoardHID keyboardhid = { 0, 0, 0, 0, 0, 0, 0, 0 };
/********************************************************************************************/

/*****************************CONFIGURACION DE LOS LED RGB***********************************/
typedef struct
{
	uint8_t ROJO;
	uint8_t VERDE;
	uint8_t AZUL;
} ledRGB;

ledRGB ledrgb = { 0, 0, 0 };

const uint8_t NUMERODEPARPADEOS = 2;
const uint8_t TIEMPOPARPADEOLENTO = 150;
const uint8_t TIEMPOPARPADEORAPIDO = 50;

void setRGB(uint8_t r, uint8_t g, uint8_t b)
{
	//Fijamos los valores de PWM para el timer4
	htim4.Instance->CCR1 = b;
	htim4.Instance->CCR2 = g;
	htim4.Instance->CCR3 = r;
}

void parpadearLedLento()
{
	//Guardamos los valores de pwm para no perderlos
	ledrgb.AZUL = htim4.Instance->CCR1;
	ledrgb.VERDE = htim4.Instance->CCR2;
	ledrgb.ROJO = htim4.Instance->CCR3;

	//Parpadeamos el led i veces
	for (uint8_t i = 0; i < NUMERODEPARPADEOS; i++)
	{
		//Apagamos el led durante TIEMPOPARPADEOLENTO ms
		setRGB ( 0, 0, 0 );
		HAL_Delay ( TIEMPOPARPADEOLENTO );

		//Encendemos con el color que tenía durante 50 ms
		setRGB ( ledrgb.ROJO, ledrgb.VERDE, ledrgb.AZUL );
		HAL_Delay ( TIEMPOPARPADEOLENTO );
	}
}

void parpadearLedRapido()
{
	//Guardamos los valores de pwm para no perderlos
	ledrgb.AZUL = htim4.Instance->CCR1;
	ledrgb.VERDE = htim4.Instance->CCR2;
	ledrgb.ROJO = htim4.Instance->CCR3;

	//Parpadeamos el led i veces
	for (uint8_t i = 0; i < NUMERODEPARPADEOS; i++)
	{
		//Apagamos el led durante TIEMPOPARPADEOLENTO ms
		setRGB ( 0, 0, 0 );
		HAL_Delay ( TIEMPOPARPADEORAPIDO );

		//Encendemos con el color que tenía durante 50 ms
		setRGB ( ledrgb.ROJO, ledrgb.VERDE, ledrgb.AZUL );
		HAL_Delay ( TIEMPOPARPADEORAPIDO );
	}
}

/********************************************************************************************/

/*****************************CONFIGURACION DEL ENCODER ROTATIVO******************************/
uint8_t oldStateA;
uint8_t newStateA;

void leerEncoder()
{
	if (!HAL_GPIO_ReadPin ( RotaryButton_GPIO_Port, RotaryButton_Pin ))
	{
		modoSeleccion = 1;
		//Igualamos los pines para evitar el bug de que a veces cambiaba
		newStateA = HAL_GPIO_ReadPin ( RotaryA_GPIO_Port, RotaryA_Pin );
		oldStateA = newStateA;
		//Parpadeamos el led para saber que estamos en modo seleccion
		parpadearLedLento ();
		while (modoSeleccion)
		{
			newStateA = HAL_GPIO_ReadPin ( RotaryA_GPIO_Port, RotaryA_Pin );
			if (newStateA != oldStateA)
			{
				if (HAL_GPIO_ReadPin ( RotaryB_GPIO_Port, RotaryB_Pin ) != newStateA)
				{ //Derecha

					if (modoFuncionamiento + 1 <= MAX_MODO)
					{
						modoFuncionamiento++;
						HAL_Delay ( 150 );
					}
					else
					{
						modoFuncionamiento = 1;
						HAL_Delay ( 150 );
					}

					//LED
					switch (modoFuncionamiento)
					{
						case 1: //STMCubeIDE
							setRGB ( 255, 0, 255 );
							break;

						case 2: //Word
							setRGB ( 0, 0, 255 );
							break;
					}
				}
				else //Izquierda
				{
					if (modoFuncionamiento - 1 >= 1)
					{
						modoFuncionamiento--;
						HAL_Delay ( 150 );
					}
					else
					{
						modoFuncionamiento = MAX_MODO;
						HAL_Delay ( 150 );
					}

					//LED
					switch (modoFuncionamiento)
					{
						case 1: //STMCubeIDE
							setRGB ( 255, 0, 255 );
							break;

						case 2: //Word
							setRGB ( 0, 0, 255 );
							break;
					}
				}
				oldStateA = HAL_GPIO_ReadPin ( RotaryA_GPIO_Port, RotaryA_Pin );

			}
			else if (!HAL_GPIO_ReadPin ( RotaryButton_GPIO_Port,
			RotaryButton_Pin ))
			{
				modoSeleccion = 0;
				parpadearLedRapido ();
				HAL_Delay ( 100 );
			}
		}
	}
}

/***************************************SOFTWARE*********************************************/
const uint8_t TIEMPOENTRETECLAS = 20;
const uint16_t TIEMPOESPERA = 300;
void Word()
{
	//-------------------------------------------------
	// NEGRITA ----------------------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton1_GPIO_Port, Boton1_Pin ))
	{
		keyboardhid.MODIFIER = 1; //Modificador CTRL
		keyboardhid.KEYCODE1 = 17; //N
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}

	//-------------------------------------------------
	// CURSIVA ----------------------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton2_GPIO_Port, Boton2_Pin ))
	{
		keyboardhid.MODIFIER = 1; //Modificador CTRL
		keyboardhid.KEYCODE1 = 14; //K
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}

	//-------------------------------------------------
	// SUBRAYADO --------------------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton3_GPIO_Port, Boton3_Pin ))
	{
		keyboardhid.MODIFIER = 1; //Modificador CTRL
		keyboardhid.KEYCODE1 = 22; //S
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}

	//-------------------------------------------------
	// SUB INDICE ------------------------------------- //ESTA POR REVISAR YA QUE NO FUNCIONA EN WORD CTRL+=
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton4_GPIO_Port, Boton4_Pin ))
	{
		keyboardhid.MODIFIER = 0; //Modificador CTRL
		keyboardhid.KEYCODE1 = 0; //=
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}

	//-------------------------------------------------
	// SUPER INDICE -----------------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton5_GPIO_Port, Boton5_Pin ))
	{
		keyboardhid.MODIFIER = 1; //Modificador CTRL
		keyboardhid.KEYCODE1 = 48; //+
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}

	//-------------------------------------------------
	// ALINEAR A LA IZQUIERDA -------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton6_GPIO_Port, Boton6_Pin ))
	{
		keyboardhid.MODIFIER = 1; //Modificador CTRL
		keyboardhid.KEYCODE1 = 20; //Q
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}

	//-------------------------------------------------
	// ALINEAR AL CENTRO ------------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton7_GPIO_Port, Boton7_Pin ))
	{
		keyboardhid.MODIFIER = 1; //Modificador CTRL
		keyboardhid.KEYCODE1 = 23; //T
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}

	//-------------------------------------------------
	// ALINEAR A LA DERECHA ---------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton8_GPIO_Port, Boton8_Pin ))
	{
		keyboardhid.MODIFIER = 1; //Modificador CTRL
		keyboardhid.KEYCODE1 = 7; //F8
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}

	//-------------------------------------------------
	// JUSTIFICAR TEXTO -------------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton9_GPIO_Port, Boton9_Pin ))
	{
		keyboardhid.MODIFIER = 1; //Modificador CTRL
		keyboardhid.KEYCODE1 = 13; //J
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}

	//-------------------------------------------------
	// INSERTAR FORMULA -------------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton10_GPIO_Port, Boton10_Pin ))
	{
		keyboardhid.MODIFIER = 6; //Modificador ALT + SHIT
		keyboardhid.KEYCODE1 = 39; //=
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}

	//-------------------------------------------------
	// REVISION ORTOGR�?FICA ---------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton11_GPIO_Port, Boton11_Pin ))
	{
		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 64; //F7
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}

	//-------------------------------------------------
	// EXPORTAR PDF -----------------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton12_GPIO_Port, Boton12_Pin ))
	{
		keyboardhid.MODIFIER = 1; //Modificador CTRL
		keyboardhid.KEYCODE1 = 19; //P
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //Modificador
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}
}

void STM32CubeIde()
{
	//-------------------------------------------------
	// RESUME -----------------------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton1_GPIO_Port, Boton1_Pin ))
	{
		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 65; //F8
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}

	//-------------------------------------------------
	// SUSPEND ----------------------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton2_GPIO_Port, Boton2_Pin ))
	{
		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 66; //F9
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}

	//-------------------------------------------------
	// STEP INTO --------------------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton3_GPIO_Port, Boton3_Pin ))
	{
		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 62; //F5
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}

	//-------------------------------------------------
	// STEP OVER --------------------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton4_GPIO_Port, Boton4_Pin ))
	{
		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 63; //F6
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}

	//-------------------------------------------------
	// STEP RETURN ------------------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton5_GPIO_Port, Boton5_Pin ))
	{
		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 64; //F7
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}

	//-------------------------------------------------
	// RUN TO LINE ------------------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton6_GPIO_Port, Boton6_Pin ))
	{
		keyboardhid.MODIFIER = 1; //Modificador CTRL
		keyboardhid.KEYCODE1 = 21; //R
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}

	//-------------------------------------------------
	// RESTART ----------------------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton7_GPIO_Port, Boton7_Pin ))
	{
		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 67; //F10
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}

	//-------------------------------------------------
	// TERMINATE AND RELAUNCH -------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton8_GPIO_Port, Boton8_Pin ))
	{
		keyboardhid.MODIFIER = 1; //Modificador CTRL
		keyboardhid.KEYCODE1 = 65; //F8
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}

	//-------------------------------------------------
	// TERMINATE --------------------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton9_GPIO_Port, Boton9_Pin ))
	{
		keyboardhid.MODIFIER = 1; //Modificador CTRL
		keyboardhid.KEYCODE1 = 59; //F2
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}

	//-------------------------------------------------
	// RUN --------------------------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton10_GPIO_Port, Boton10_Pin ))
	{
		keyboardhid.MODIFIER = 1; //Modificador CTRL
		keyboardhid.KEYCODE1 = 68; //F11
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}

	//-------------------------------------------------
	// DEBUG ------------------------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton11_GPIO_Port, Boton11_Pin ))
	{
		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 68; //F11
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}

	//-------------------------------------------------
	// BUILD ------------------------------------------
	//-------------------------------------------------
	if (!HAL_GPIO_ReadPin ( Boton12_GPIO_Port, Boton12_Pin ))
	{
		keyboardhid.MODIFIER = 1; //Modificador CTRL
		keyboardhid.KEYCODE1 = 5; //B
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOENTRETECLAS );

		keyboardhid.MODIFIER = 0; //Modificador
		keyboardhid.KEYCODE1 = 0; //Modificador
		USBD_HID_SendReport ( &hUsbDeviceFS, (uint8_t*) &keyboardhid, sizeof(keyboardhid) );
		HAL_Delay ( TIEMPOESPERA );
	}
}

/********************************************************************************************/
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */
	//Leemos el estado antiguo del pin para evitar el bug de que a veces saltaba solo;
	oldStateA = HAL_GPIO_ReadPin ( RotaryA_GPIO_Port, RotaryA_Pin );
	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init ();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config ();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init ();
	MX_USB_DEVICE_Init ();
	MX_TIM4_Init ();
	MX_USART1_UART_Init ();
	/* USER CODE BEGIN 2 */
	//Habilitamos el TIM4 para el led RGB
	HAL_TIM_PWM_Start ( &htim4, TIM_CHANNEL_1 );
	HAL_TIM_PWM_Start ( &htim4, TIM_CHANNEL_2 );
	HAL_TIM_PWM_Start ( &htim4, TIM_CHANNEL_3 );

	//Ponemos el led en azul por defecto ("MODO WORD");
	setRGB ( 0, 0, 255 );
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		leerEncoder ();
		switch (modoFuncionamiento)
		{
			case 1: //Modo Word
				Word ();
				break;

			case 2: //Modo STM32CubeIde
				STM32CubeIde ();
				break;
		}
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
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig ( &RCC_OscInitStruct ) != HAL_OK)
	{
		Error_Handler ();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig ( &RCC_ClkInitStruct, FLASH_LATENCY_2 ) != HAL_OK)
	{
		Error_Handler ();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
	PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
	if (HAL_RCCEx_PeriphCLKConfig ( &PeriphClkInit ) != HAL_OK)
	{
		Error_Handler ();
	}
}

/**
 * @brief TIM4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM4_Init(void)
{

	/* USER CODE BEGIN TIM4_Init 0 */

	/* USER CODE END TIM4_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	TIM_OC_InitTypeDef sConfigOC = { 0 };

	/* USER CODE BEGIN TIM4_Init 1 */

	/* USER CODE END TIM4_Init 1 */
	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 1076;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = 254;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init ( &htim4 ) != HAL_OK)
	{
		Error_Handler ();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource ( &htim4, &sClockSourceConfig ) != HAL_OK)
	{
		Error_Handler ();
	}
	if (HAL_TIM_PWM_Init ( &htim4 ) != HAL_OK)
	{
		Error_Handler ();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization ( &htim4, &sMasterConfig ) != HAL_OK)
	{
		Error_Handler ();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel ( &htim4, &sConfigOC, TIM_CHANNEL_1 ) != HAL_OK)
	{
		Error_Handler ();
	}
	if (HAL_TIM_PWM_ConfigChannel ( &htim4, &sConfigOC, TIM_CHANNEL_2 ) != HAL_OK)
	{
		Error_Handler ();
	}
	if (HAL_TIM_PWM_ConfigChannel ( &htim4, &sConfigOC, TIM_CHANNEL_3 ) != HAL_OK)
	{
		Error_Handler ();
	}
	/* USER CODE BEGIN TIM4_Init 2 */

	/* USER CODE END TIM4_Init 2 */
	HAL_TIM_MspPostInit ( &htim4 );

}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void)
{

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init ( &huart1 ) != HAL_OK)
	{
		Error_Handler ();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE( );
	__HAL_RCC_GPIOD_CLK_ENABLE( );
	__HAL_RCC_GPIOA_CLK_ENABLE( );
	__HAL_RCC_GPIOB_CLK_ENABLE( );

	/*Configure GPIO pins : RotaryA_Pin RotaryB_Pin RotaryButton_Pin */
	GPIO_InitStruct.Pin = RotaryA_Pin | RotaryB_Pin | RotaryButton_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init ( GPIOC, &GPIO_InitStruct );

	/*Configure GPIO pins : Boton12_Pin Boton11_Pin Boton10_Pin Boton9_Pin
	 Boton8_Pin Boton7_Pin Boton6_Pin Boton5_Pin */
	GPIO_InitStruct.Pin = Boton12_Pin | Boton11_Pin | Boton10_Pin | Boton9_Pin | Boton8_Pin | Boton7_Pin | Boton6_Pin | Boton5_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init ( GPIOA, &GPIO_InitStruct );

	/*Configure GPIO pins : Boton4_Pin Boton3_Pin Boton2_Pin Boton1_Pin */
	GPIO_InitStruct.Pin = Boton4_Pin | Boton3_Pin | Boton2_Pin | Boton1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init ( GPIOB, &GPIO_InitStruct );

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
