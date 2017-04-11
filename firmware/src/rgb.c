#include "rgb.h"
#include "img.h"

#define RGB_BUSY(x)                 (GPIO_PIN17_bit = (x))

/** Panel Pins ****************************************************************/
unsigned char PAN_DATA     at GPIO_PORT_08_15;
sbit          PAN_A        at GPIO_PIN32_bit;
sbit          PAN_B        at GPIO_PIN33_bit;
sbit          PAN_C        at GPIO_PIN34_bit;
sbit          PAN_D        at GPIO_PIN35_bit;
sbit          PAN_CLK      at GPIO_PIN45_bit;
sbit          PAN_STB      at GPIO_PIN44_bit;
sbit          PAN_OE       at GPIO_PIN52_bit;

/** Variables *****************************************************************/
/** Row LUT */
static uint8_t _rw_map[ 8 ] = {0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
/** Command byte - 1st byte of SPI transfer */
static uint8_t _cmd;
/** Data bytes - 2nd and other bytes of SPI transfer - FIFO size */
static uint8_t _in[ 64 ];

static uint8_t _line[ 128 ];
/** RAM space - Height (64) x Width (64) x Bytes per color (2) */
static uint8_t _ram[ 8192 ];
/** SUB frames - 3 shadows */
static uint8_t _shadow[ 3 ][ 8 ][ 256 ];
/** Display properties */
static uint8_t _power;
static uint8_t _brightness;
/** Offset values */
static int8_t _x_offset;
static int8_t _y_offset;

/** Private Prototypes ********************************************************/
static void _convert_pix( uint8_t x, uint8_t y );
static void _convert_ram( void );

static void _write_pix( void );
static void _write_line( void );
static void _load_image( void );
static void _set_brightness( void );
static void _set_power( void );
static void _set_offset( void );
static void _shift_ram( void );

/** Private Definitions *******************************************************/
static void _convert_pix( uint8_t x, uint8_t y )
{
    uint16_t input;
    uint8_t red;
    uint8_t gre;
    uint8_t blu;
    uint8_t tmp;
    uint8_t xp;
    uint8_t yp;
    uint8_t pan;
    uint8_t row;
    uint8_t col;
    uint8_t sh;
    uint8_t lsb;
    uint8_t msb;
    uint8_t mask;

    // Position calculation
    xp = x % 32; yp = y % 32; pan = x / 32 + y / 16;
    row = yp % 8;
    col = ( ( yp % 16 ) >= 8 ) ? ( xp ) : ( 32 + xp );
    if( yp >= 16 )
    {
        sh = 3;
        col += 64 * ( ( pan + 3 ) % 4 );
    }
    else
    {
        sh = 0;
        col += 64 * pan;
    }
    mask = ~( 0x7 << sh );
    lsb = _ram[ ( y * 64 + x ) * 2 ];
    msb = _ram[ ( y * 64 + x ) * 2 + 1 ];

    // Mapping
    input = msb; input <<= 8; input |= lsb;
    red = ( 1 << ( ( input & 0xC000 ) >> 14 ) ) - 1;
    gre = ( 1 << ( ( input & 0x0600 ) >> 9  ) ) - 1;
    blu = ( 1 << ( ( input & 0x0018 ) >> 3  ) ) - 1;
    tmp = _shadow[0][row][col]; tmp &= mask;
    tmp |= ( ( red & 1 ) | ( ( gre & 1 ) << 1 ) | ( ( blu & 1 ) << 2 ) ) << sh;
    _shadow[0][row][col] = tmp;
    red >>= 1; gre >>= 1; blu >>= 1;
    tmp = _shadow[1][row][col]; tmp &= mask;
    tmp |= ( ( red & 1 ) | ( ( gre & 1 ) << 1 ) | ( ( blu & 1 ) << 2 ) ) << sh;
    _shadow[1][row][col] = tmp;
    red >>= 1; gre >>= 1; blu >>= 1;
    tmp = _shadow[2][row][col]; tmp &= mask;
    tmp |= ( ( red & 1 ) | ( ( gre & 1 ) << 1 ) | ( ( blu & 1 ) << 2 ) ) << sh;
    _shadow[2][row][col] = tmp;
}

static void _convert_ram( void )
{
    uint8_t red;
    uint8_t gre;
    uint8_t blu;
    uint8_t msb;
    uint8_t lsb;
    uint8_t tmp;
    uint16_t input;
    uint8_t x;
    uint8_t y;
    uint8_t xp;
    uint8_t yp;
    uint8_t pan;
    uint8_t row;
    uint8_t col;
    uint8_t shift;
    uint8_t mask;

    uint8_t i;

    for( x = 0; x < 64; x++ )
    {
        for( y = 0; y < 64; y++ )
        {
            xp = x % 32;
            yp = y % 32;
            pan = x / 32 + y / 16;

            row = yp % 8;
            col = ((yp % 16) >= 8) ? (xp) : (32 + xp);

            if( yp >= 16 )
            {
                shift = 3;
                col += 64 * ((pan + 3) % 4);
            }
            else
            {
                shift = 0;
                col += 64 * pan;
            }
            mask = ~( 0x7 << shift );

            if( ( x < _x_offset ) || ( y < _y_offset ) ||
                ( x >= ( _x_offset + 64 ) ) || ( y >= ( _y_offset + 64 ) ) )
            {
                lsb = 0;
                msb = 0;
            }
            else
            {
                lsb = _ram[ ( ( y - _y_offset ) * 64 + ( x - _x_offset ) ) * 2 ];
                msb = _ram[ ( ( y - _y_offset ) * 64 + ( x - _x_offset ) ) * 2 + 1 ];
            }

            input = msb;
            input <<= 8;
            input |= lsb;

            red = ( 1 << ( ( input & 0xC000 ) >> 14 ) ) - 1;
            gre = ( 1 << ( ( input & 0x0600 ) >> 9  ) ) - 1;
            blu = ( 1 << ( ( input & 0x0018 ) >> 3  ) ) - 1;


            tmp = _shadow[0][row][col];
            tmp &= mask;
            tmp |= ( ( red & 1 ) | ( ( gre & 1 ) << 1 ) | ( ( blu & 1 ) << 2 ) ) << shift;
            _shadow[0][row][col] = tmp;

            red >>= 1;
            gre >>= 1;
            blu >>= 1;

            tmp = _shadow[1][row][col];
            tmp &= mask;
            tmp |= ( ( red & 1 ) | ( ( gre & 1 ) << 1 ) | ( ( blu & 1 ) << 2 ) ) << shift;
            _shadow[1][row][col] = tmp;

            red >>= 1;
            gre >>= 1;
            blu >>= 1;

            tmp = _shadow[2][row][col];
            tmp &= mask;
            tmp |= ( ( red & 1 ) | ( ( gre & 1 ) << 1 ) | ( ( blu & 1 ) << 2 ) ) << shift;
            _shadow[2][row][col] = tmp;
        }
    }
}

static void _write_pix( void )
{
    while(SPIS1_RX_FIFO_COUNT < 4)
        ;
    STREAMIN_B(_in, &SPIS1_DATA, 4);
    _ram[ (_in[0]*64 + _in[1]) * 2 ] = _in[2];
    _ram[ (_in[0]*64 + _in[1]) * 2 + 1 ] = _in[3];
    _convert_pix(_in[1], _in[0]);
    _cmd = 0;
}

static void _write_ram( void )
{
    uint16_t cnt;
    
    for(cnt = 0; cnt < 1024; cnt++)
    {
        while(SPIS1_RX_FIFO_COUNT < 8)
            ;
        STREAMIN_B(_in, &SPIS1_DATA, 8);
        memcpy(_ram + cnt * 8, _in, 8);
    }
    _convert_ram();
    _cmd = 0;
}


static void _load_image( void )
{
    uint16_t    cnt;
     
    while(SPIS1_RX_FIFO_COUNT < 1)
        ;
    STREAMIN_B(_in, &SPIS1_DATA, 1);
    for( cnt = 0; cnt < 8192; cnt++ )
    {
        _ram[ cnt ] = image[ _in[0] ][ cnt ];
    }
    _convert_ram();
    _cmd = 0;
}

static void _set_brightness( void )
{
    while( SPIS1_RX_FIFO_COUNT < 1 )
        ;
    STREAMIN_B( _in, &SPIS1_DATA, 1 );
    _brightness = _in[ 0 ];
    _in[ 0 ] = 0;
    _cmd = 0;
}

static void _set_power( void )
{
    while( SPIS1_RX_FIFO_COUNT < 1 )
         ;
    STREAMIN_B( _in, &SPIS1_DATA, 1 );
    _power = _in[ 0 ];
    _in[ 0 ] = 0;
    _cmd = 0;
}

static void _set_offset( void )
{
    while( SPIS1_RX_FIFO_COUNT < 2 )
         ;
    STREAMIN_B( _in, &SPIS1_DATA, 2 );
    _x_offset = _in[ 0 ];
    _y_offset = _in[ 1 ];
    _convert_ram();
    _cmd = 0;
}

/** Public Functions **********************************************************/
void panel_init( void )
{
    PAN_DATA = 0;
    PAN_A    = 0;
    PAN_B    = 0;
    PAN_C    = 0;
    PAN_D    = 0;
    PAN_CLK  = 0;
    PAN_STB  = 1;
    PAN_OE   = 0;
    Delay_ms(100);
    _convert_ram();
    _x_offset = 0;
    _y_offset = 0;
    _power = 1;
    _brightness = 1;
}

void display( void )
{
    uint8_t     sh, rw;
    uint16_t    dt, cn;

    GPIO_PIN17_bit = 1;
    for( sh = 0; sh < 3; sh++ )
    {
        for( rw = 0; rw < 8; rw++ )
        {
            PAN_A =  _rw_map[rw] & 0x01;
            PAN_B = (_rw_map[rw] & 0x02) >> 1;
            PAN_C = (_rw_map[rw] & 0x04) >> 2;
            PAN_D = (_rw_map[rw] & 0x08) >> 3;
            for( dt = 0; dt < 256; dt++ )
            {
                PAN_STB     = 0;
                PAN_DATA    = _shadow[sh][rw][dt];
                PAN_CLK     = 1;
                PAN_CLK     = 0;
                PAN_STB     = 1;
            }
            if( _power )
            {
                PAN_OE = 0;
            }
            for( cn = 0; cn < _brightness; cn++ )
            {
                 Delay_1us();
            }
            PAN_OE = 1;
        }
    }
    PAN_OE = 1;
    GPIO_PIN17_bit = 0;
}

void spi_check( void )
{
    if( SPIS1_RX_FIFO_COUNT > 0 )
    {
        STREAMIN_B( &_cmd, &SPIS1_DATA, 1 );
    }

    switch( _cmd )
    {
    case RGB_BRIGHTNESS:
        _set_brightness();
        break;
    case RGB_POWER:
        _set_power();
        break;
    case RGB_LOAD_IMG:
        _load_image();
        break;
    case RGB_WRITE_IMG:
        _write_ram();
        break;
    case RGB_WRITE_PIX:
        _write_pix();
        break;
    case RGB_OFFSET:
        _set_offset();
        break;
    default:
        _cmd = 0;
        break;
    }
}
/**                                                                 End *******/
