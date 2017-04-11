#include <string.h>

#include "matrix_rgb_hw.h"
#include "matrix_rgb_hal.h"

static uint8_t*          _font;
static uint16_t         _font_color;
static uint8_t          _font_orientation;
static uint16_t         _font_first_char;
static uint16_t         _font_last_char;
static uint16_t         _font_height;
static uint16_t         x_cord;
static uint16_t         y_cord;
static uint8_t          frame_buffer[8192];

static void character( uint16_t ch )
{
    uint8_t     ch_width = 0;
    uint8_t     x_cnt;
    uint8_t     y_cnt;
    uint16_t    x = 0;
    uint16_t    y = 0;
    uint16_t    tmp;
    uint8_t     temp = 0;
    uint8_t     mask = 0;
    uint32_t    offset;
    uint8_t *ch_table;
    uint8_t *ch_bitmap;
    if( ch < _font_first_char )
        return;
    if( ch > _font_last_char )
        return;
    offset = 0;
    
    tmp = (ch - _font_first_char) << 2;
    ch_table = _font + 8 + tmp;
    ch_width = *ch_table;
    
    offset = (uint32_t)ch_table[1] + ((uint32_t)ch_table[2] << 8) + ((uint32_t)ch_table[3] << 16);
    ch_bitmap = _font + offset;
    if( ( _font_orientation == FO_HORIZONTAL ) ||
        ( _font_orientation == FO_VERTICAL_COLUMN ) )
    {
        y = y_cord;
        for (y_cnt = 0; y_cnt < _font_height; y_cnt++)
        {
            x = x_cord;
            mask = 0;
            for( x_cnt = 0; x_cnt < ch_width; x_cnt++ )
            {
                if( !mask )
                {
                    temp = *ch_bitmap++;
                    mask = 0x01;
                }
                if( temp & mask )
                {
                    matrixrgb_dot( y, x, _font_color );
                    Delay_ms( 3 );
                }
                x++;
                mask <<= 1;
            }
            y++;
        }
        if ( _font_orientation == FO_HORIZONTAL )
            x_cord = x + 1;
        else
            y_cord = y;
    }
    else
    {
        y = x_cord;
        for( y_cnt = 0; y_cnt < _font_height; y_cnt++ )
        {
            x = y_cord;
            mask = 0;
            for( x_cnt = 0; x_cnt < ch_width; x_cnt++ )
            {
                if( mask == 0 )
                {
                    temp = *ch_bitmap++;
                    mask = 0x01;
                }
                if( temp & mask )
                {
                    matrixrgb_dot( y, x, _font_color );
                }
                x--;
                mask <<= 1;
            }
            y++;
        }
        y_cord = x - 1;
    }
}
void matrixrgb_init( void )
{
    matrixrgb_hal_init();
    Delay_ms(300);
}
void matrixrgb_cmd( uint8_t cmd, uint8_t arg )
{
    matrixrgb_hal_write( &cmd, 1 );
    matrixrgb_hal_write( &arg, 1 );
}

void matrixrgb_dot( uint8_t x, uint8_t y, uint16_t color )
{
    uint8_t lsb;
    uint8_t msb;
    msb = color >> 8;
    lsb = color & 0x00FF;
    frame_buffer[ ( x * 64 + y ) * 2 ]    = lsb;
    frame_buffer[ ( x * 64 + y ) * 2 + 1 ] = msb;
}

void matrixrgb_fill( uint16_t color )
{
    uint16_t i;
    uint8_t msb     = color & 0x00FF;
    uint8_t lsb     = (color & 0xFF00) >> 8;
    for( i = 0; i < 8192; i++ )
    {
    	if( i % 2 )
    		frame_buffer[i] = msb;
    	else
    		frame_buffer[i] = lsb;
    }
    matrixrgb_flush();
}

void matrixrgb_img( uint8_t *img )
{
    memcpy( frame_buffer, img, 8192 );
    matrixrgb_flush();
}

void matrixrgb_off( int8_t x_off, int8_t y_off )
{
    uint8_t command;
    command = 0x06;
    matrixrgb_hal_write( &command, 1 );
    matrixrgb_hal_write( &x_off, 1 );
    matrixrgb_hal_write( &y_off, 1 );
}

void matrixrgb_img_scroll( uint8_t *img, scroll_dir_t direction, scroll_speed_t speed )
{
    uint8_t *ptr = img;
    int8_t   off;
    int8_t   del;
    matrixrgb_img( ptr );
    switch( direction )
    {
    case DIR_DOWN :
         for( off = -64; off <= 64; off++ )
         {
             matrixrgb_off( 0, off );
             
             for( del = 0; del < speed; del++ )
                  Delay_1ms();
         }
         break;
    case DIR_UP :
        for( off = 64; off >= -64; off-- )
        {
             matrixrgb_off( 0, off );
             for( del = 0; del < speed; del++ )
                  Delay_1ms();
        }
        break;
    case DIR_LEFT :
        for( off = 64; off >= -64; off-- )
        {
            matrixrgb_off( off, 0 );
            for( del = 0; del < speed; del++ )
                  Delay_1ms();
        }
        break;
    case DIR_RIGHT :
        for( off = -64; off <= 64; off++ )
        {
             matrixrgb_off( off, 0 );
             for( del = 0; del < speed; del++ )
                  Delay_1ms();
        }
        break;
    }
}

void matrixrgb_img_scroll_in( uint8_t *img, scroll_dir_t direction, scroll_speed_t speed )
{
    uint8_t *ptr = img;
    int8_t   off;
    int8_t   del;
    matrixrgb_img( ptr );
    switch( direction )
    {
    case DIR_DOWN :
         for( off = -64; off <= 0; off++ )
         {
             matrixrgb_off( 0, off );
             for( del = 0; del < speed; del++ )
                  Delay_1ms();
         }
         break;
    case DIR_UP :
        for( off = 64; off >= 0; off-- )
        {
             matrixrgb_off( 0, off );
             for( del = 0; del < speed; del++ )
                  Delay_1ms();
        }
        break;
    case DIR_LEFT :
        for( off = 64; off >= 0; off-- )
        {
            matrixrgb_off( off, 0 );
            for( del = 0; del < speed; del++ )
                  Delay_1ms();
        }
        break;
    case DIR_RIGHT :
        for( off = -64; off <= 0; off++ )
        {
             matrixrgb_off( off, 0 );
             for( del = 0; del < speed; del++ )
                  Delay_1ms();
        }
        break;
    }
}

void matirxrgb_scroll_out( scroll_dir_t direction, scroll_speed_t speed )
{
    int8_t   off;
    int8_t   del;
    switch( direction )
    {
    case DIR_DOWN :
         for( off = 1; off <= 64; off++ )
         {
             matrixrgb_off( 0, off );
             for( del = 0; del < speed; del++ )
                  Delay_1ms();
         }
         break;
    case DIR_UP :
        for( off = -1; off >= -64; off-- )
        {
             matrixrgb_off( 0, off );
             for( del = 0; del < speed; del++ )
                  Delay_1ms();
        }
        break;
    case DIR_LEFT :
        for( off = -1; off >= -64; off-- )
        {
            matrixrgb_off( off, 0 );
            for( del = 0; del < speed; del++ )
                  Delay_1ms();
        }
        break;
    case DIR_RIGHT :
        for( off = 1; off <= 64; off++ )
        {
             matrixrgb_off( off, 0 );
             for( del = 0; del < speed; del++ )
                  Delay_1ms();
        }
        break;
    }
}

void matrixrgb_text( char *text, uint16_t x, uint16_t y )
{
    uint8_t *ptr = text;
    if( ( x >= 64 ) || ( y >= 64 ) )
        return;
    x_cord = x;
    y_cord = y;
    while( *ptr )
    {
        character( *ptr );
        ptr++;
    }
}

void matrixrgb_flush()
{
	uint8_t command = 0x04;
	matrixrgb_hal_write( &command, 1 );
	matrixrgb_hal_write( frame_buffer, 8192 );
}

void matrixrgb_set_font( uint8_t *font, uint16_t color, uint8_t orientation )
{
    _font               = font;
    _font_first_char    = font[2] + (font[3] << 8);
    _font_last_char     = font[4] + (font[5] << 8);
    _font_height        = font[6];
    _font_color         = color;
    _font_orientation   = orientation ;
}
/*************** END OF FUNCTIONS *********************************************/
