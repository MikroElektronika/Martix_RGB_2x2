#include "rgb.h"

void system_init( void )
{
    int i;
    char slave_pins[] = { _GPIO_PIN_NUM_36,                  // SPI0_CLk_Pin
                          _GPIO_PIN_NUM_37,                  // Slave Select Pin
                          _GPIO_PIN_NUM_38,                  // SPI0_MOSI_Pin
                          _GPIO_PIN_NUM_39                   // SPI0_MISO_Pin
                        };
    char output_pins[] = { _GPIO_PIN_NUM_18,                 // Camera Power Down Pin
                           _GPIO_PIN_NUM_17,                 // Ready Pin
                           _GPIO_PIN_NUM_50                  // Camera Reset Pin
                         };
    // PANEL CTL
    GPIO_Digital_Output( &GPIO_PORT_08_15, _GPIO_PINMASK_ALL );
    GPIO_Digital_Output( &GPIO_PORT_32_39, _GPIO_PINMASK_ALL );
    GPIO_Digital_Output( &GPIO_PORT_40_47, _GPIO_PINMASK_4 | _GPIO_PINMASK_5 );
    GPIO_Digital_Output( &GPIO_PORT_48_55, _GPIO_PINMASK_4 );         // OE Pin

    // SPI CTL
    for ( i = 0; i < sizeof( output_pins ); i++ )
    {
        GPIO_Pin_Config( output_pins[i],
                         _GPIO_DIR_OUTPUT,
                         _GPIO_CFG_SLEW_RATE_FAST |
                         _GPIO_CFG_SCHMITT_DISABLE |
                         _GPIO_CFG_PULL_DOWN |
                         _GPIO_CFG_DRIVE_16mA |
                         _GPIO_CFG_SPEC_NONE );
    }

    PWR_EnableModule( _PWR_CLK_SPIS0 );
    for ( i = 0; i < sizeof( slave_pins ); i++ )
    {
        GPIO_Pin_Config( slave_pins[i],
                         ( i == _GPIO_PIN_NUM_39 ) ? _GPIO_DIR_OUTPUT : _GPIO_DIR_INPUT,
                         _GPIO_CFG_SLEW_RATE_FAST |
                         _GPIO_CFG_SCHMITT_DISABLE |
                         _GPIO_CFG_PULL_NONE |
                         _GPIO_CFG_DRIVE_16mA |
                         _GPIO_CFG_SPEC_FUNC_1 );
    }

    SPIS1_CNTL  = ( 1 << SP_IE )   |  // Enable spi slave interrupt( SP_IE )
                  ( 1 << SP_E )    |  // Enable spi ( SP_E )
                  ( 1 << SP_R0 )   |
                  ( 1 << SP_R1 )   |
                  ( 1 << SP_R2 );
    SPIS1_CNTL &= ~( 1 << CLK_POL );
    /* 64 Byte deep FIFOs enabled, tx and rx fifo enabled */
    SPIS1_FIFO_CNTL =  ( 1 << FIFO_SIZE )  |  // Enable deep 64 byte FIFO
                       ( 1 << TX_RST )     |  // Reset TX FIFO
                       ( 1 << RCVR_RST )   |  // Reset RX FIFO
                       ( 1 << FIFO_EN );      // FIFO Enable
    SPIS1_TNSFR_FRMT_CNTL |= ( 1 << MULTI_REC );    // Allow continuous reception of data
}

void main()
{   
    system_init();
    panel_init();

    while( 1 )
    {
        display();
        spi_check();
    }
}