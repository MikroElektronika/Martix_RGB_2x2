/**
 * @file rgb.h
 *
 * @mainpage
 * @{
 *
 * Firmware for Matrix RGB click.
 *
 * Features :
 * - 8 kB of RAM write able through SPI
 *  + Write whole RAM at once
 *  + Write pixel by pixel
 * - 30 images can be stored on flash memory for faster loading
 * - Power ON/OFF functionality
 * - Brightness in 256 steps
 * - 6bit - 64 colors
 * - High refresh rate
 *
 * @version 2.0.0
 * @authors MilosV, Zeka
 *
 * @todo
 * Partial RAM programming command
 *
 * @todo
 * Vertical and horizontal shifting command
 *
 * @todo
 * Test SPI speed limits
 *
 * @}
 */

#ifndef RGB_H
#define RGB_H

#include <stdint.h>
#include <string.h>

/**
 * @page FWF Firmware Flashing
 * @brief RGB click flashing
 *
 * @{
 *
 * #### 1. Donwload and install mikroProg Suite for FT90x and drivers. ####
 *
 * @par
 * Manual and installation
 * - <a href="http://download.mikroe.com/documents/programmers-debuggers/mikroprog/ft90x/mikroprog-ft90x-manual-v101.pdf"> Setup Instructions </a>
 *
 * @par
 * Download links
 * - <a href="http://download.mikroe.com/setups/programming-software/mikroprog/ft90x/mikroprog-suite-ft90x-programming-software-setup-v110.zip"> mikroProg Suite for FT90x </a>
 * - <a href="http://download.mikroe.com/setups/drivers/mikroprog/arm/mikroprog-suite-ft90x-drivers-v212.zip"> Suite Drivers </a>
 *
 * #### 2. Start Suite ####
 * #### 3. Load HEX ####
 * #### 4. Program FT900 ####
 * @}
 */

/**
 * @page IMF Image Flashing
 * @brief RGB Image code generation
 *
 * @{
 *
 * #### 1. Donwload and install Visual TFT software. ####
 * @par
 * Download links
 * - <a href="http://download.mikroe.com/setups/additional-software/visualtft/visual-tft-v461.zip"> Visual TFT </a>
 *
 *
 * #### 2. Generate Image ####
 * @par
 * - Create image with paint (or whatever) 64x64 pixels and save as BMP 24bit
 * - Start Visual TFT
 * - Create New project ( whatever compiler/screen )
 * - Drag and drop Image from "Components palette" to the Screen area
 * - Right click to the image then Load Image form File
 * - Press Generate Code button on toolbar (toothed wheel)
 * - Copy the content of the array in resources.c named same as loaded file - exclude image header (6 bytes)
 * - Paste content to the @c img.h file @c image var as second dimension's array
 *
 *
 * #### 3. Copmile and flash ####
 * @par
 * - <a href="FWF.html"> Program FT900 Instructions </a>
 *
 * @note
 * Generated image must be in RGB565 format - 2 bytes per pixel LSB first
 *
 * @note
 * Image size must be 8192 bytes (64x64x2)
 *
 * @note
 * Generated image can be also stored to application MCU and transfered using SPI
 *
 * @}
 */

/**
 * @page PSU Panel Setup
 * @brief Panel cabling and power supply
 *
 * @{
 *
 * @par
 * #### Panels position - Front view ####
 * @par
 * | 4 | 3 |
 * |:-:|:-:|
 * | 2 | 1 |
 *
 * @par
 * - RGB click to Panel 1 input
 * - Panel 1 output to Panel 2 input, and so on
 * - 5V power supply
 *
 * @warning
 * Be sure that back side of panel cannot touch cables
 *
 * @warning
 * Each panel consumes 4A when all shining white
 *
 * @}
 */

/**
 * @defgroup SPIC SPI commands
 * @brief SPI related commands
 *
 * @{
 *
 * SPI frame sent by master :
 *
 * | Command byte | Data bytes |
 * |:------------:|:----------:|
 * | 1 byte command | 1 to 8192 bytes of data depend on command |
 *
 */
/**
 * Power ON/OFF control
 *
 * | CMD    | DATA |
 * |:------:|:---:|
 * | 1 byte | 1 byte |
 * | <0x01> | [0x00 or 0x01] - OFF / ON |
 */
#define RGB_POWER       0x01

/**
 * Brightness control
 *
 * | CMD    | DATA |
 * |:------:|:---:|
 * | 1 byte | 1 byte |
 * | <0x02> | [0x00 to 0xFF] - Brightness level |
 *
 */
#define RGB_BRIGHTNESS  0x02

/**
 * Load image stored on flash
 *
 * | CMD    | DATA |
 * |:------:|:---:|
 * | 1 byte | 1 byte |
 * | <0x03> | [0x00 to 0x1E] - Flashed image index |
 */
#define RGB_LOAD_IMG    0x03

/**
 * Transfer image through SPI ( RGB 565 format )
 *
 * Application must send 8192 bytes of data after 0x04 command.
 *
 * | CMD    | DATA |
 * |:------:|:---:|
 * | 1 byte | 8192 bytes |
 * | <0x04> | Image Data |
 *
 */
#define RGB_WRITE_IMG   0x04

/**
 * Trasfer pixel through SPI ( RGB 565 format )
 *
 * Aplication must send 4 bytes of data after 0x05 command.
 * - 1 - Vertical offset
 * - 2 - Horizontal offset
 * - 3 - Color LSB
 * - 4 - Color MSB
 *
 * | CMD    | DATA |
 * |:------:|:---:|
 * | 1 byte | 4 bytes |
 * | <0x05> | [H_OFF] [V_OFF] [LSB_COLOR] [MSB_COLOR] |
 */
#define RGB_WRITE_PIX   0x05

/**
 * Trasfer pixel through SPI ( RGB 565 format )
 *
 * Aplication must send 2 bytes of data after 0x06 command.
 * - 1 - Horizontal offset ( signed value in range -64 to 64 )
 * - 2 - Vertical offset ( signed value in range -64 to 64 )
 *
 * | CMD    | DATA |
 * |:------:|:---:|
 * | 1 byte | 2 bytes |
 * | <0x06> | [H_OFF] [V_OFF] |
 */
#define RGB_OFFSET      0x06


#define RGB_UP      0
#define RGB_DOWN    1
#define RGB_LEFT    2
#define RGB_RIGHT   3

/**
 * @}
 */

void display( void );

void spi_check( void );

void panel_init( void );

#endif // RGB_H
