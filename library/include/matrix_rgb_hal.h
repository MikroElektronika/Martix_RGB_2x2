/****************************************************************************
* Title                 :   Matrix RGB HAL
* Filename              :   matrixrgb_hal.h
* Author                :   CAL
* Origin Date           :   05/23/2016
* Notes                 :   None
*****************************************************************************/
/**************************CHANGE LIST **************************************
*
*    Date     Software Version    Initials       Description
*  05/23/2016      1.0.0            CAL        Module Created.
*
*****************************************************************************/
/**
 * @file matrixrgb_hal.h
 * @brief <h2> HAL layer </h2>
 *
 * @par
 * HAL layer for
 * <a href="http://www.mikroe.com">MikroElektronika's</a> Matrix RGB click
 * board.
 */
 
#ifndef MATRIXRGB_HAL_H_
#define MATRIXRGB_HAL_H_
/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
/******************************************************************************
* Preprocessor Constants
*******************************************************************************/
#define DUMMY_BYTE                                                  0x00
/******************************************************************************
* Configuration Constants
*******************************************************************************/

/******************************************************************************
* Macros
*******************************************************************************/

/******************************************************************************
* Typedefs
*******************************************************************************/

/******************************************************************************
* Variables
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief <h3> CS High </h3>
 *
 * @par
 * Used by HW layer functions to set CS PIN high ( deselect )
 */
void matrixrgb_hal_cs_high( void );

/**
 * @brief <h3> CS Low </h3>
 *
 * @par
 * Used by HW layer functions to set CS PIN low ( selecet )
 */
void matrixrgb_hal_cs_low( void );

/**
 * @brief <h3> Hardware Reset </h3>
 *
 * @par
 * Resets the module via RST PIN
 */
//void matrixrgb_hal_reset( void );

/**
 * @brief <h3> HAL Initialization </h3>
 *
 * Hal layer initialization. Must be called before any other function.
 */
void matrixrgb_hal_init( void );

/**
 * @brief <h3> HAL Write </h3>
 *
 * @par
 * Writes data through SPI bus
 *
 * @note Function have no affect to the CS PIN state - chip select is
 * controled directly from HW layer.
 *
 * @param[in] buffer
 * @param[in] count
 */
void matrixrgb_hal_write( uint8_t *buffer,
                          uint16_t count );

/**
 * @brief <h3> HAL Read </h3>
 *
 * @par
 * Reads data from SPI bus
 *
 * @note Function have no affect to the CS PIN state - chip select is
 * controled directly from HW layer
 *
 * @param[out] buffer
 * @param[in] count
 */
void matrixrgb_hal_read( uint8_t *buffer,
                         uint16_t count );

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* MATRIXRGB_HAL_H_ */

/*** End of File **************************************************************/