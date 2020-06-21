#include "FileGroup.h"
#include "GUI_Private.h"

/**
  * @brief  ����LOGO����
  * @param  x:X����
  * @param  y:Y����
  * @param  time:֡��ʱʱ��
  * @param  dir:�������Ʒ���
  * @retval ��
  */
void Draw_RGBBitmap_Logo(float x, float y, uint16_t time, bool dir)
{
    const unsigned char* gImage_Bitmap_Logox[] = {
        gImage_Bitmap_Logo1,
        gImage_Bitmap_Logo2,
        gImage_Bitmap_Logo3,
        gImage_Bitmap_Logo4,
        gImage_Bitmap_Logo5,
        gImage_Bitmap_Logo6,
        gImage_Bitmap_Logo7,
        gImage_Bitmap_Logo8,
        gImage_Bitmap_Logo9,
        gImage_Bitmap_Logo10,
        gImage_Bitmap_Logo11,
        gImage_Bitmap_Logo12,
    };

    if(dir)
    {
        for(uint8_t i = 12; i > 0; i--)
        {
            screen.drawRGBBitmap(screen.width() * x - 93 / 2, screen.height() * y - 5, (uint16_t*)gImage_Bitmap_Logox[i], 93, 10);
            delay(time);
        }
    }
    else
    {
        for(uint8_t i = 0; i < 12; i++)
        {
            screen.drawRGBBitmap(screen.width() * x - 93 / 2, screen.height() * y - 5, (uint16_t*)gImage_Bitmap_Logox[i], 93, 10);
            delay(time);
        }
    }
}
