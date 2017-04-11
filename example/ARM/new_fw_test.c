#include "image.h"
#include "matrix_rgb_hw.h"

sbit MATRIXRGB_CS       at PTC_PDOR.B4;
sbit MATRIXRGB_READY    at PTB_PDIR.B13;
sbit MATRIXRGB_RST      at PTB_PDOR.B11;

void main() 
{
    uint8_t rep;
    uint8_t blinks;
    uint8_t intens;
    
    GPIO_Digital_Output( &PTC_PDOR, _GPIO_PINMASK_4 );
    GPIO_Digital_Output( &PTB_PDOR, _GPIO_PINMASK_13 );
    GPIO_Digital_Input( &PTB_PDIR, _GPIO_PINMASK_11 ); // Set Reset pin to output
    
    MATRIXRGB_RST = 0;
    Delay_ms(20);
    MATRIXRGB_RST = 1;
    Delay_ms(200);
    
    SPI0_Init_Advanced( 4000000, _SPI_CFG_MASTER | _SPI_CFG_SELECT_CTAR0 |
                        _SPI_CFG_FRAME_SIZE_8BITS | _SPI_CFG_CLK_IDLE_LOW | 
                        _SPI_CFG_CLK_PHASE_CAPT_LEADING | _SPI_CFG_MSB_FIRST, 
                        &_GPIO_Module_SPI0_PC5_7_6 );

    MATRIXRGB_CS = 1;
    Delay_ms( 2000 );
    matrixrgb_init();
    
    // IMAGE FROM FLASH TEST
    matrixrgb_cmd( RGB_IMG_LOAD, 2 );
    Delay_ms( 1000 );
    
    // BRIGHTNESS TEST
    matrixrgb_cmd( RGB_BRIGHTNESS, 5 );
    Delay_ms( 1000 );
    matrixrgb_cmd( RGB_BRIGHTNESS, 50 );
    Delay_ms( 1000 );
    matrixrgb_cmd( RGB_BRIGHTNESS, 100 );
    Delay_ms( 1000 );
    matrixrgb_cmd( RGB_BRIGHTNESS, 5 );
    
    // POWER TEST
    matrixrgb_cmd( RGB_POWER, 0 );
    Delay_ms( 1000 );
    matrixrgb_cmd( RGB_POWER, 1 );
    Delay_ms( 1000 );
    
    // FILL SCREEN AND PIXEL TEST
    matrixrgb_fill( 0x1234 );
    Delay_ms( 1000 );
    matrixrgb_fill( 0xF8A1 );
    Delay_ms( 1000 );
    matrixrgb_fill( 0x0000 );
    Delay_ms( 500 );
    matrixrgb_dot(  0,  0, 0xF100 );
    matrixrgb_dot(  0, 20, 0x07E0 );
    matrixrgb_dot(  0, 40, 0x001F );
    matrixrgb_dot( 63, 63, 0xF100 );
    matrixrgb_dot( 15,  0, 0x07E0 );
    matrixrgb_dot( 25, 40, 0x001F );
    matrixrgb_dot( 50, 20, 0xF100 );
    matrixrgb_dot( 30, 63, 0x07E0 );
    matrixrgb_dot( 41, 25, 0x001F );
    Delay_ms( 2000 );

    // IMAGE UPLOAD TEST
    matrixrgb_img( elogo4x4_bmp );
    Delay_ms( 2000 );
    
    // DYNAMIC IMAGE SCROLL
    while( 1 )
    {
        matrixrgb_img_scroll( NXP_logo_bmp, DIR_UP, SPEED_FAST );
        matrixrgb_img_scroll( NXP_logo_bmp, DIR_DOWN, SPEED_MEDIUM );
        matrixrgb_img_scroll( NXP_logo_bmp, DIR_LEFT, SPEED_SLOW );
        matrixrgb_img_scroll( NXP_logo_bmp, DIR_RIGHT, SPEED_FAST );
    }
}