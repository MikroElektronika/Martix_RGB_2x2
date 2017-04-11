/**
 * @file matrixrgb_hw.h
 *
 * Library for Matrix RGB click board.
 *
 * @authors
 *     - Milos Vidojevic		- Firmware
 *     - Aleksandar Zecevix	    - KDS Adaptation
 */
#ifndef MATRIXRGB_INC_MATRIXRGB_HW_H_
#define MATRIXRGB_INC_MATRIXRGB_HW_H_

#include <stdint.h>

#define FO_HORIZONTAL           (0)
#define FO_VERTICAL             (1)
#define FO_VERTICAL_COLUMN      (2)
#define RGB_POWER       0x01	/**< Power Control */
#define RGB_BRIGHTNESS  0x02	/**< Brightness Control */
#define RGB_IMG_LOAD    0x03	/**< Load image stored on clickboard */

typedef enum
{
    DIR_UP           = 0,
    DIR_DOWN         = 1,
    DIR_LEFT         = 2,
    DIR_RIGHT        = 3
}scroll_dir_t;

typedef enum
{
    SPEED_FAST      = 15,
    SPEED_MEDIUM    = 20,
    SPEED_SLOW      = 25
}scroll_speed_t;
/**
 * @brief Initialization
 *
 * Initialization of HAL and HW layer.
 */
void matrixrgb_init( void );
/**
 * @brief Send Simple Command
 *
 * @param[in] cmd command
 * @param[in] arg argument
 *
 * Commands :
 * - POWER
 * - BRIGHTNESS
 * - LOAD IMAGE FROM FLASH
 */
void matrixrgb_cmd( uint8_t cmd, uint8_t arg );
/**
 * @brief Write Image to RAM
 *
 * @param[in] img pointer to image array
 *
 * @note
 * Image must be in RGB565 format LSB first.
 */
void matrixrgb_img( uint8_t *img );
/**
 * @brief Write Pixel
 *
 * @param[in] x horizontal position
 * @param[in] y vertical position
 * @param[in] color pixel color
 */
void matrixrgb_dot( uint8_t x, uint8_t y, uint16_t color );
/**
 * @brief Fill Screen
 *
 * @param[in] color screen color
 */
void matrixrgb_fill( uint16_t color );
/**
 * @brief Offset command
 *
 * @param[in] x_off horizontal offset
 * @param[in] y_off vertical offset
 *
 * Offset is signed value and must be in range from -64 to 64.
 */
void matrixrgb_off( int8_t x_off, int8_t y_off );
/**
 * @brief Scroll Effect
 *
 * @param[in] img pointer to image
 * @param[in] direction scroll direction
 * @param[in] speed scroll speeed
 *
 * Scrolls image across the screen/
 */
void matrixrgb_img_scroll( uint8_t *img, scroll_dir_t direction, scroll_speed_t speed );
/**
 * @brief Scroll In Effect
 *
 * @param[in] img pointer to image
 * @param[in] direction scroll direction
 * @param[in] speed scroll speeed
 *
 * Scrolls image in until it arrives to the center.
 */
void matrixrgb_img_scroll_in( uint8_t *img, scroll_dir_t direction, scroll_speed_t speed );
/**
 * @brief Scroll Out Effect
 *
 * @param[in] direction scroll direction
 * @param[in] speed scroll speeed
 *
 * Scrolls out current content of the screen.
 */
void matirxrgb_scroll_out( scroll_dir_t direction, scroll_speed_t speed );
/**
 * @brief Writes text
 *
 * @param[in] x - horizontal offset
 * @param[in] y - vertical offset
 *
 * Writing starts at provided offset
 */
void matrixrgb_text( char *text, uint16_t x, uint16_t y );
/**
 * @brief Flush Function
 *
 * Flushes the frame buffer onto the MCU controlling the LED panels.
 */
void matrixrgb_flush();
/**
 * @brief Set Font Function
 *
 * Must be called before write text function to adjust text related properties.
 */
void matrixrgb_set_font( uint8_t *font, uint16_t color, uint8_t orientation );

#endif /* MATRIXRGB_INC_MATRIXRGB_HW_H_ */
