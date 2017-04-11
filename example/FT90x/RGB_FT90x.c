#include <stdint.h>
#include "image.h"
#include "matrix_rgb_hw.h"

sbit MATRIXRGB_CS       at GPIO_PIN28_bit;
sbit MATRIXRGB_READY    at GPIO_PIN1_bit;
sbit MATRIXRGB_RST      at GPIO_PIN64_bit;

void main()
{
    int i;
    
    GPIO_Digital_Output( &GPIO_PORT_64_66 , _GPIO_PINMASK_0 ); // Set Reset pin to output
    MATRIXRGB_RST = 0;
    Delay_ms(20);
    MATRIXRGB_RST = 1;
    Delay_ms(200);

    GPIO_Digital_Output( &GPIO_PORT_24_31 , _GPIO_PINMASK_4 ); // Set Chip Select pin as output
    GPIO_Digital_Input( &GPIO_PORT_00_07 , _GPIO_PINMASK_1 );  // Set Ready to input
    
    SPIM1_Init_Advanced( _SPI_MASTER_CLK_RATIO_4, _SPI_CFG_PHASE_CAPTURE_FALLING |
                     _SPI_CFG_POLARITY_IDLE_LOW | _SPI_CFG_SS_AUTO_DISABLE |
                     _SPI_CFG_FIFO_DISABLE, _SPI_SS_LINE_NONE );
                                   // Reset slave ( toggle )
    MATRIXRGB_CS = 1;
    Delay_ms( 2000 );
    
    matrixrgb_init();
    
    while( 1 )
    {
        // BLINK
        //matrixrgb_off( 0, 0 );
        matrixrgb_img_scroll_in( discount_bmp, DIR_UP, SPEED_SLOW );
        for( i = 0; i <= 5; i++ )
        {
            Delay_ms( 1000 );
            matrixrgb_cmd( RGB_POWER, 0 );
            Delay_ms( 1000 );
            matrixrgb_cmd( RGB_POWER, 1 );
        }
        matirxrgb_scroll_out( DIR_UP, SPEED_SLOW );
        Delay_ms( 500 );
        
        matrixrgb_img_scroll_in( apps_bmp, DIR_DOWN, SPEED_FAST );
        for( i = 0; i <= 5; i++ )
        {
            Delay_ms( 1000 );
            matrixrgb_cmd( RGB_POWER, 0 );
            Delay_ms( 1000 );
            matrixrgb_cmd( RGB_POWER, 1 );
        }
        matirxrgb_scroll_out( DIR_DOWN, SPEED_FAST );
        Delay_ms( 500 );
        
        /*matrixrgb_img_scroll_in( toster_bmp, DIR_LEFT, SPEED_FAST );
        Delay_ms( 1000 );
        matirxrgb_scroll_out( DIR_LEFT, SPEED_FAST );
        Delay_ms( 500 );

        matrixrgb_img_scroll_in( mixer_bmp, DIR_LEFT, SPEED_FAST );
        Delay_ms( 1000 );
        matirxrgb_scroll_out( DIR_LEFT, SPEED_FAST );
        Delay_ms( 500 );*/
        //matrixrgb_off( 0, 0 );
    }
}