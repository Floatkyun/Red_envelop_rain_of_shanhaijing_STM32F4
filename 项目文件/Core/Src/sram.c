#include "sram.h"
#include "usart.h"


SRAM_HandleTypeDef g_sram_handler; /* SRAM句柄 */

/**
 * @brief       初始化 外部SRAM
 * @param       无
 * @retval      无
 */
void sram_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    FSMC_NORSRAM_TimingTypeDef fsmc_readwritetim;

    SRAM_CS_GPIO_CLK_ENABLE();    /* SRAM_CS脚时钟使能 */
    SRAM_WR_GPIO_CLK_ENABLE();    /* SRAM_WR脚时钟使能 */
    SRAM_RD_GPIO_CLK_ENABLE();    /* SRAM_RD脚时钟使能 */
    __HAL_RCC_FSMC_CLK_ENABLE();  /* 使能FSMC时钟 */
    __HAL_RCC_GPIOD_CLK_ENABLE(); /* 使能GPIOD时钟 */
    __HAL_RCC_GPIOE_CLK_ENABLE(); /* 使能GPIOE时钟 */
    __HAL_RCC_GPIOF_CLK_ENABLE(); /* 使能GPIOF时钟 */
    __HAL_RCC_GPIOG_CLK_ENABLE(); /* 使能GPIOG时钟 */

    gpio_init_struct.Pin = SRAM_CS_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_struct.Alternate = GPIO_AF12_FSMC;
    HAL_GPIO_Init(SRAM_CS_GPIO_PORT, &gpio_init_struct); /* SRAM_CS引脚模式设置 */

    gpio_init_struct.Pin = SRAM_WR_GPIO_PIN;
    HAL_GPIO_Init(SRAM_WR_GPIO_PORT, &gpio_init_struct); /* SRAM_WR引脚模式设置 */

    gpio_init_struct.Pin = SRAM_RD_GPIO_PIN;
    HAL_GPIO_Init(SRAM_RD_GPIO_PORT, &gpio_init_struct); /* SRAM_CS引脚模式设置 */

    /* PD0,1,4,5,8~15 */
    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | 
                       GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 |
                       GPIO_PIN_14 | GPIO_PIN_15;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;       /* 推挽复用 */
    gpio_init_struct.Pull = GPIO_PULLUP;           /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; /* 高速 */
    HAL_GPIO_Init(GPIOD, &gpio_init_struct);

    /* PE0,1,7~15 */
    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 |
                       GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |
                       GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &gpio_init_struct);

    /* PF0~5,12~15 */
    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 |
                       GPIO_PIN_5 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOF, &gpio_init_struct);

    /* PG0~5,10 */
    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
    HAL_GPIO_Init(GPIOG, &gpio_init_struct);

    g_sram_handler.Instance = FSMC_NORSRAM_DEVICE;
    g_sram_handler.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;

    g_sram_handler.Init.NSBank = (SRAM_FSMC_NEX == 1) ? FSMC_NORSRAM_BANK1 : \
                                 (SRAM_FSMC_NEX == 2) ? FSMC_NORSRAM_BANK2 : \
                                 (SRAM_FSMC_NEX == 3) ? FSMC_NORSRAM_BANK3 : 
                                                        FSMC_NORSRAM_BANK4; /* 根据配置选择FSMC_NE1~4 */
    g_sram_handler.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;     /* 地址/数据线不复用 */
    g_sram_handler.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;                 /* SRAM */
    g_sram_handler.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;    /* 16位数据宽度 */
    g_sram_handler.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;   /* 是否使能突发访问,仅对同步突发存储器有效,此处未用到 */
    g_sram_handler.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW; /* 等待信号的极性,仅在突发模式访问下有用 */
    g_sram_handler.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;      /* 存储器是在等待周期之前的一个时钟周期还是等待周期期间使能NWAIT */
    g_sram_handler.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;       /* 存储器写使能 */
    g_sram_handler.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;              /* 等待使能位,此处未用到 */
    g_sram_handler.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;          /* 读写使用相同的时序 */
    g_sram_handler.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;  /* 是否使能同步传输模式下的等待信号,此处未用到 */
    g_sram_handler.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;              /* 禁止突发写 */
    /* FSMC读时序控制寄存器 */
    fsmc_readwritetim.AddressSetupTime = 0x02;                              /* 地址建立时间（ADDSET）为2个HCLK 1/168M=6ns*2=12ns */
    fsmc_readwritetim.AddressHoldTime = 0x00;                               /* 地址保持时间（ADDHLD）模式A未用到 */
    fsmc_readwritetim.DataSetupTime = 0x08;                                 /* 数据保存时间为8个HCLK =6*8= 48ns */
    fsmc_readwritetim.BusTurnAroundDuration = 0x00;
    fsmc_readwritetim.AccessMode = FSMC_ACCESS_MODE_A;                      /* 模式A */
    HAL_SRAM_Init(&g_sram_handler, &fsmc_readwritetim, &fsmc_readwritetim);
}






/**
 * @brief       往SRAM指定地址写入指定长度数据 测试用 不建议实际使用
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始写入的地址(最大32bit)
 * @param       datalen : 要写入的字节数(最大32bit)
 * @retval      无
 */
void sram_write(uint8_t *pbuf, uint32_t addr, uint32_t datalen)
{
    for (; datalen != 0; datalen--)
    {
        *(volatile uint8_t *)(SRAM_BASE_ADDR + addr) = *pbuf;
        addr++;
        pbuf++;
    }
}


void sram_write_(uint8_t *pbuf, uint32_t addr, uint32_t datalen)
{
    for (; datalen != 0; datalen--)
    {
        *(volatile uint8_t *)(SRAM_BASE_ADDR + addr) = 0xff;
        addr++;
        pbuf++;
    }
}


/**
 * @brief       从SRAM指定地址读取指定长度数据 测试用 不建议实际使用 
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始读取的地址(最大32bit)
 * @param       datalen : 要读取的字节数(最大32bit)
 * @retval      无
 */
void sram_read(uint8_t *pbuf, uint32_t addr, uint32_t datalen)
{
    for (; datalen != 0; datalen--)
    {
        *pbuf++ = *(volatile uint8_t *)(SRAM_BASE_ADDR + addr);
        addr++;
    }
}

uint32_t sram_current_add =0;


sram_bmp_file* sram_load_bmp_from_sd_withbuf(char* path)
{		
	FRESULT res;
	bmp_head bmp;
	uint32_t br;
	static sram_bmp_file sram_bmp;
	sram_bmp.addr=sram_current_add;
	static uint8_t buf[960];
	static uint8_t buf2[960];
	res=f_open(&SDFile,path,FA_READ);
	if(res==FR_OK)
	{
		if(f_read(&SDFile,(char*)&bmp+2,sizeof(bmp_head)-2,&br)==FR_OK)
		{
			int Height=bmp.info.Height;
			sram_bmp.h=Height;
			int Width=bmp.info.Width;
			sram_bmp.w=Width;
			sram_bmp.len=Width*2*Height;
			f_lseek(&SDFile,bmp.file.Offset);
			printf("sram_current_add%d\n\r",sram_current_add);
			while(1)
			{
			if(f_read(&SDFile,buf,960,&br)==FR_OK)
			{
				printf("br%d\n\r",br);
				sram_write_(buf,sram_current_add,br);
				sram_current_add+=br;
				printf("write success sram_current_add=%d\n\r",sram_current_add);
			
			}
			else
			{
					break;
			}
			if(br==0)
			{
				break;
			}
		}
		}
		return &sram_bmp;
	}
	else{
		printf("opened fail %x",res);
		return NULL;
	}
}







/**
 * @brief       读取SD卡16bit色深的bmp图像文件到外部SRAM
 * @param       path         SD卡bmp图像文件存储路径
 * @retval      带有数据存储首地址和数据长度等信息的结构体sram_bmp_file
 */
sram_bmp_file* sram_load_bmp_from_sd(char* path)
{		
	FRESULT res;
	bmp_head bmp;
	uint32_t br;
	sram_bmp_file* sram_bmp=(sram_bmp_file*)malloc(sizeof(sram_bmp_file));
	sram_bmp->addr=sram_current_add;
	res=f_open(&SDFile,path,FA_READ);
	if(res==FR_OK)
	{
		if(f_read(&SDFile,(char*)&bmp+2,sizeof(bmp_head)-2,&br)==FR_OK)
		{
			int Height=bmp.info.Height;
			sram_bmp->h=Height;
			int Width=bmp.info.Width;
			sram_bmp->w=Width;
			sram_bmp->len=Width*2*Height;
			f_lseek(&SDFile,bmp.file.Offset);
			if(f_read(&SDFile,(uint8_t *)(SRAM_BASE_ADDR + sram_current_add),sram_bmp->len,&br)==FR_OK)//读取SD卡数据到外部SRAM
			{
				printf("br%d\n\r",br);
				sram_current_add+=br;
			}
			else
			{
				printf("read fail \r\n");
				return NULL;
			}
		}
		f_close(&SDFile);
		printf("%s load success \r\n", path);
		return sram_bmp;
	}
	else{
		printf("opened fail %x\r\n",res);
		return NULL;
	}
}















void show_bmp_from_sram(int  x,int y,sram_bmp_file* bmp_file)
{
			lcd_show_inverted_pic(x,  y, x+bmp_file->w-1,y+bmp_file->h-1,( uint8_t *)(SRAM_BASE_ADDR + bmp_file->addr));
}
void show_bmp_from_sram_cut(int  x,int y,sram_bmp_file* bmp_file)
{
			lcd_show_inverted_pic_cut(x,  y, x+bmp_file->w-1,y+bmp_file->h-1,( uint8_t *)(SRAM_BASE_ADDR + bmp_file->addr));
}

void show_bmp_from_sram_full(sram_bmp_file* bmp_file)
{
 lcd_show_inverted_pic_full( bmp_file->h, bmp_file->w, ( uint8_t *)(SRAM_BASE_ADDR + bmp_file->addr));	
}











/**
 * @brief       显示bmp图片中的形状不规则物体 不遮挡背景显示 
 * @param       x、y           显示开始位置
 * @param       bmp_file       要显示的图片对象 
 * @param       background_bmp 要显示的背景图对象
 * @retval      空
 */
void show_bmp_from_sram_with_background(int  x,int y,sram_bmp_file* bmp_file,sram_bmp_file* background_bmp)
{
	   uint16_t height, width;
	int cursorx,cursory;
		uint8_t* color=( uint8_t *)(SRAM_BASE_ADDR + bmp_file->addr);
		uint8_t* back_color=( uint8_t *)(SRAM_BASE_ADDR + background_bmp->addr);
    width = bmp_file->w;          /* 得到填充的宽度 */
    height = bmp_file->h;           /* 高度 */
	cursorx=x;
	cursory=y+height-1;
	uint16_t tempcolor;
					
		for(int i=0;i<height*width*2;i+=2)
				
		{
					if(cursorx==x+width)
					{
						cursorx=x;
						cursory--;
						lcd_set_cursor(cursorx, cursory); /* 设置光标位置 */
						lcd_write_ram_prepare();  
					}

					if(cursory<24||cursory>lcddev.height-1)/* 限制显示范围 */
					{
						cursorx++;
						continue;
					}
					tempcolor=((((uint16_t)color[i+1])<<8)|((uint16_t)color[i]));
					if(tempcolor!=0xffff)
					{					
						LCD->LCD_RAM =tempcolor;
					}
					else
					{
						int num=lcddev.width*(lcddev.height-1-cursory)+cursorx;
						LCD->LCD_RAM =(((uint16_t)back_color[num*2+1])<<8)|((uint16_t)back_color[num*2]);
					}
					cursorx++;
		}
		lcd_set_cursor(480-1, 800-1);//设置光标到屏幕最末 防止沿原光标继续读取噪声作为显示内容
}







/**
 * @brief       局部刷新背景图  输入图像要与LCD屏幕大小一致
 * @param       x、y           显示开始位置
 * @param       width、height  显示内容的宽、高
 * @param       background_bmp 要显示的背景图对象
 * @retval      空
 */
void range_show_bmp_from_sram(int  x,int y,int width,int height,sram_bmp_file* background_bmp)
{
		int cursorx,cursory;
		uint8_t* back_color=( uint8_t *)(SRAM_BASE_ADDR + background_bmp->addr);
		cursorx=x;
		cursory=y+height-1;
				for(int i=0;i<height*width*2;i+=2)
		{
					if(cursorx==x+width)
					{
						cursorx=x;
						cursory--;
						lcd_set_cursor(cursorx, cursory); /* 设置光标位置 */
						lcd_write_ram_prepare();  
					}			
					if(cursory<24||cursory>lcddev.height-1)/* 限制显示范围 */
					{
						cursorx++;
						continue;
					}
					int num=lcddev.width*(lcddev.height-1-cursory)+cursorx;/* 计算当前坐标对应的数据地址 */
					LCD->LCD_RAM =(((uint16_t)back_color[num*2+1])<<8)|((uint16_t)back_color[num*2]);
					cursorx++;
		}
		lcd_set_cursor(lcddev.width-1, lcddev.height-1);//设置光标到屏幕最末 防止沿原光标继续读取噪声作为显示内容
}























char Load_bmp_with_backgroung_from_sram(uint16_t x,uint16_t y,char* path,sram_bmp_file* background_bmp) 
{
	FRESULT res;
	bmp_head bmp;
	uint32_t br;
	uint32_t cursorx=x,cursory=y;
	static uint8_t buf[2400];
	res=f_open(&SDFile,path,FA_READ);
	if(res==FR_OK)
	{
		if(f_read(&SDFile,(char*)&bmp+2,sizeof(bmp_head)-2,&br)==FR_OK)
		{			
			int Height=bmp.info.Height;
			int Width=bmp.info.Width;
			f_lseek(&SDFile,bmp.file.Offset);
				uint8_t* back_color=( uint8_t *)(SRAM_BASE_ADDR + background_bmp->addr);
				cursorx=x;
				cursory=y+Height-1;
			while(1)
			{
			if(f_read(&SDFile,buf,2400,&br)==FR_OK)
			{
				for(int i=0;i<br;i+=2)
				{
					if(cursorx==x+Width)
					{
						cursorx=x;
						cursory--;
						lcd_set_cursor(cursorx, cursory); /* 设置光标位置 */
						lcd_write_ram_prepare();  
					}
					uint16_t tempcolor=(((uint16_t)buf[i+1])<<8)|((uint16_t)buf[i]);
					if(tempcolor!=0xffff)
					{					
						LCD->LCD_RAM =tempcolor;
					}
					else
					{
						int num=lcddev.width*(lcddev.height-1-cursory)+cursorx;
						LCD->LCD_RAM =(((uint16_t)back_color[num*2+1])<<8)|((uint16_t)back_color[num*2]);
					}
					cursorx++;
				}
			}
			else
			{
				break;
			}
			if(br==0)
			{
				break;
			}
		}
}
		f_close(&SDFile);
	lcd_set_cursor(480-1, 800-1);
		return 0;
	}
	else
		
	{
		printf("opened fail %x",res);
		return -1;
	}
	
}










//void show_bmp_from_sram(int  x,int y,sram_bmp_file* bmp_file)
//{
//uint16_t height, width;
//	int cursorx,cursory;

//		uint8_t picH,picL;
//    width = bmp_file->w;            /* 得到填充的宽度 */
//    height = bmp_file->h;           /* 高度 */
//	cursorx=x;
//	cursory=y+height-1;
//	volatile uint8_t* color =(volatile uint8_t *)(SRAM_BASE_ADDR + bmp_file->addr);
//					
//		for(int i=0;i<height*width*2;i+=2)
//				
//		{
//					if(cursorx==x+width)
//					{
//						cursorx=x;
//						cursory--;
//						lcd_set_cursor(cursorx, cursory); /* 设置光标位置 */
//						lcd_write_ram_prepare();  
//					}
//					
//		LCD->LCD_RAM =(((uint16_t)color)<<8)|((uint16_t)color[i]);
//					cursorx++;
//		}
//		lcd_set_cursor(480-1, 800-1);
//}

































/*******************测试函数**********************************/

/**
 * @brief       测试函数 在SRAM指定地址写入1个字节
 * @param       addr    : 开始写入的地址(最大32bit)
 * @param       data    : 要写入的字节
 * @retval      无
 */
void sram_test_write(uint32_t addr, uint8_t data)
{
    sram_write(&data, addr, 1); /* 写入1个字节 */
}

/**
 * @brief       测试函数 在SRAM指定地址读取1个字节
 * @param       addr    : 开始读取的地址(最大32bit)
 * @retval      读取到的数据(1个字节)
 */
uint8_t sram_test_read(uint32_t addr)
{
    uint8_t data;
    sram_read(&data, addr, 1); /* 读取1个字节 */
    return data;
}
 


void fsmc_sram_test(void)
{
	  uint32_t i = 0;
    uint8_t temp = 0;
    uint8_t sval = 0; /* 在地址0读到的数据 */

    /* 每隔4K字节,写入一个数据,总共写入256个数据,刚好是1M字节 */
    for (i = 0; i < 1024 * 1024; i += 4096)
    {
        sram_write(&temp, i, 1);
        temp++;
    }

    /* 依次读出之前写入的数据,进行校验 */
    for (i = 0; i < 1024 * 1024; i += 4096)
    {
        sram_read(&temp, i, 1);

        if (i == 0)
        {
            sval = temp;
        }
        else if (temp <= sval)
        {
            break; /* 后面读出的数据一定要比第一次读到的数据大 */
        }
				printf("%d\n\r" ,(uint16_t)(temp - sval + 1) * 4);

    }
	
}