/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
#include "fatfs.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */

/* USER CODE BEGIN Variables */
#include "sdio.h"
#include "usart.h"
#include "lcd.h"
/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */
	FRESULT fre;
	
	fre=f_mount(&SDFatFS,SDPath,1);
	
	if(fre==FR_OK)
	{
		printf("sd mount ok\n\r");
		printf("SD Size %fG\n\r",(float)hsd.SdCard.LogBlockNbr*hsd.SdCard.LogBlockSize/1024/1024/1024);
		
	}
	else
	{
		printf("sd mount fail: %d \n\r",fre);
	}
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */
}

/* USER CODE BEGIN Application */






FRESULT scan_files(char *path)
{
	FATFS fatsd=SDFatFS; //定义文件系统对象结构
	FRESULT res; //定义结果对象
	DIR dir; //定义目录对象
	UINT i; //定义变量
	static FILINFO fno; //定义静态文件信息结构对象
	res = f_opendir(&dir,path); //打开目录，返回状态 和 目录对象的指针
	char pathBuff[256]; //定义路径数组
	if(res == FR_OK) //打开成功
	{
		while(1) //遍历
		{
			res = f_readdir(&dir, &fno); //读取目录，返回状态 和 文件信息的指针
			if(res != FR_OK || fno.fname[0] == 0) break; //若打开失败 或 到结尾，则退出
			char path_pic[64];
			sprintf(path_pic,"%s/%s",path,fno.fname);
			//show_bmp_8bit(100,200,path_pic);//显示一帧图像
			 Load_bmp(0,200,path_pic);
			fps++;
			HAL_Delay(9);
		}
		f_closedir(&dir); //关闭目录
	}
	else
	{
		printf("失败 - %s",&res); //打开失败
	}
	return res; //返回状态
}

















void Font_Read_Test_16(uint8_t* gbk)
{
	uint32_t addr,br;
	char buff[32];
	printf("汉字显示%s,gb2312 :%x %x\n\r",gbk,gbk[0],gbk[1]);
	addr = ((gbk[0]-0xa1)*94 +gbk[1]-0xa1)*32;
	
	if(f_open(&SDFile,"H_16.DZK",FA_READ)==FR_OK)
	{

		printf("open H_16.Dzk");
		f_lseek(&SDFile,addr);
		f_read(&SDFile,buff,32,&br);
		lcd_show_points(0, 0, buff,16, 0, BLACK);
		f_close(&SDFile);
	}
	else
	{
		printf("no such file H_16.Dzk");
	}
}

void Font_Read_Test_24(uint8_t* gbk)
{
	uint32_t addr,br;
	char buff[72];
	printf("汉字显示%s,gb2312 :%x %x\n\r",gbk,gbk[0],gbk[1]);
	addr = ((gbk[0]-0xa1)*94 +gbk[1]-0xa1)*72;
	
	if(f_open(&SDFile,"H_24.DZK",FA_READ)==FR_OK)
	{

		printf("open H_24.Dzk");
		f_lseek(&SDFile,addr);
		f_read(&SDFile,buff,72,&br);
		lcd_show_points(100, 100, buff,24, 0, BLACK);
		f_close(&SDFile);
	}
	else
	{
		printf("no such file 24test.DZK");
	}
}
void Font_Read_Test(uint8_t* gbk)
{
	uint32_t addr,br;
	char buff[128];
	printf("汉字显示%s,gb2312 :%x %x\n\r",gbk,gbk[0],gbk[1]);
	addr = ((gbk[0]-0xa1)*94 +gbk[1]-0xa1)*128;
	
	if(f_open(&SDFile,"H_32.DZK",FA_READ)==FR_OK)
	{

		printf("open 32test.Dzk");
		f_lseek(&SDFile,addr);
		f_read(&SDFile,buff,128,&br);
		lcd_show_points(0, 0, buff,32, 0, BLACK);
		f_close(&SDFile);
	}
	else
	{
		printf("no such file 32test.DZK");
	}
}



void show_chinese_char(uint8_t* gbk,uint16_t size, uint16_t x,uint16_t y)
{
	uint32_t addr,br;
	const uint16_t byte_size=size*size/8;
	uint8_t buff[288];
	//printf("汉字显示%s,gb2312 :%x %x\n\r",gbk,gbk[0],gbk[1]);
	addr = ((gbk[0]-0xa1)*94 +gbk[1]-0xa1)*byte_size;
	
	FRESULT fre=FR_NO_FILE;
	
	switch(size)
	{
		case 16 :
			
			fre=f_open(&SDFile,"H_16.DZK",FA_READ);
			break;
		
		case 24 :
			fre=f_open(&SDFile,"H_24.DZK",FA_READ);
			break;
		
		case 32 :
			fre=f_open(&SDFile,"H_32.DZK",FA_READ);
			break;
		
		case 48 :
			fre=f_open(&SDFile,"H_48.DZK",FA_READ);
			break;
	}
	
	if(fre==FR_OK)
	{
		f_lseek(&SDFile,addr);
		f_read(&SDFile,buff,byte_size,&br);
		lcd_show_points(x, y, buff,size, 0, BLACK);
		f_close(&SDFile);
		printf("汉字显示%s,gb2312 :%x %x\n\r",gbk,gbk[0],gbk[1]);
	}
	
	else
	{
		printf("no such file H_%d.Dzk", size);
	}
}


chinese_char* load_chinese_char(uint8_t* gbk,uint16_t size)
{
	uint32_t addr,br;
	const uint16_t byte_size=size*size/8;
	addr = ((gbk[0]-0xa1)*94 +gbk[1]-0xa1)*byte_size;
	
	FRESULT fre=FR_NO_FILE;
	switch(size)
	{
		case 16 :
			
			fre=f_open(&SDFile,"H_16.DZK",FA_READ);
			break;
		
		case 24 :
			fre=f_open(&SDFile,"H_24.DZK",FA_READ);
			break;
		
		case 32 :
			fre=f_open(&SDFile,"H_32.DZK",FA_READ);
			break;
		
		case 48 :
			fre=f_open(&SDFile,"H_48.DZK",FA_READ);
			break;
	}
	
	if(fre==FR_OK)
	{
		uint8_t* buff=(uint8_t*)malloc(sizeof(uint8_t)*byte_size);
		chinese_char* CHINESE_CH=(chinese_char*)malloc(sizeof(chinese_char));
		CHINESE_CH->size=size;
		f_lseek(&SDFile,addr);
		f_read(&SDFile,buff,byte_size,&br);
		CHINESE_CH->data=buff;
		f_close(&SDFile);
		printf("font load success%s,gb2312 :%x %x\n\r",gbk,gbk[0],gbk[1]);
		return CHINESE_CH;
	}
	
	else
	{
		printf("no such file H_%d.Dzk", size);
		return NULL;
	}
}

chinese_char* load_chinese_str(char *str,uint16_t size)
{
	chinese_char* CHINESE_STR=(chinese_char*)malloc(sizeof(chinese_char));
	CHINESE_STR->len=strlen((char*)str)/2;
	uint8_t len=CHINESE_STR->len;
	chinese_char* temp=CHINESE_STR;
	for(uint8_t idex=0;idex<len;idex++)
	{
			chinese_char* CHINESE_CHAR=load_chinese_char((uint8_t*)&str[idex*2], size);
			if(CHINESE_CHAR==NULL)
			{
				printf("load_chinese_str %s failed",str);
				return NULL;
			}
			else
			{
				temp->next=CHINESE_CHAR;
				CHINESE_CHAR->next=NULL;
				temp=temp->next;
			}
	}
	return CHINESE_STR;
}

void show_chinese_from_struct(int x,int y,chinese_char*CHINESE_CH)
{
	lcd_show_points(x, y, CHINESE_CH->data,CHINESE_CH->size, 0, BLACK);
}

void show_chinese_str_from_struct(int x,int y,chinese_char*CHINESE_STR)
{
	if(CHINESE_STR==NULL)
	{
		printf("show_chinese_str_from_struct failed");
		return ;
	}
	uint8_t len=CHINESE_STR->len;
	uint8_t idex=0;
	chinese_char* temp=CHINESE_STR->next;
	for(idex=0;idex<len;idex++)
	{
		show_chinese_from_struct(x,y,temp);
		x+=temp->size;
		temp=temp->next;
	}
}



uint16_t RGB565toRGB565(uint8_t c1,uint8_t c2)
{
	uint16_t color=0;
	uint16_t R=((uint16_t)(c2&0x1f));
	uint16_t G=((uint16_t)(c1&0x07)<<3)|(uint16_t)c2>>5;
	uint16_t B=(uint16_t)c1>>3;
	color= (R<<11)|(G<<5)|B;
	return color;
}


uint16_t RGB888toRGB565(uint8_t R,uint8_t G,uint8_t B)
{
	uint16_t color=0;
	color |=(uint16_t)(B>>3);
	color |=((uint16_t)(G>>2))<<5;
	color |=((uint16_t)(R>>3))<<11;
	return color;
}


uint16_t RGB332toRGB565(uint8_t rgb332)
{
	uint16_t color=0;


	color |= (uint16_t)((rgb332)>>6 ) <<3;		
	color |= (uint16_t)((rgb332& 0xc0)>> 4)<<9;	
	color |= (uint16_t)((rgb332& 0x0c )>>2)<<14 ;
	return color;
}





char Load_bmp(uint16_t x,uint16_t y,char* path) //从sd卡显示图片
{
	FRESULT res;
	bmp_head bmp;
	uint32_t br;
	static uint8_t colortb[256*4];
	uint32_t cursorx=x,cursory=y;
	int I=0; 
	
	static uint8_t buf[40000];
	res=f_open(&SDFile,path,FA_READ);
	if(res==FR_OK)
	{
		if(f_read(&SDFile,(char*)&bmp+2,sizeof(bmp_head)-2,&br)==FR_OK)
		{
			
			int Height=bmp.info.Height;
			int Width=bmp.info.Width;
			if (bmp.info.Count==32)
			{
				I=4;
			}
			else if(bmp.info.Count==24)
			{
				I=3;
			}
			else if(bmp.info.Count==8)
			{
				f_lseek(&SDFile,54);
				f_read(&SDFile, colortb,256*4,&br);
				//printf("br=%d",br);
			}
			
			f_lseek(&SDFile,bmp.file.Offset);
			if((bmp.info.Count==24)||(bmp.info.Count==32))
			{
			if(Height<0)
			{
			while(1)
			{
			if(f_read(&SDFile,buf,40000,&br)==FR_OK)
			{
				for(int i=0;i<br;i+=3)
				{
					if(cursorx==x+Width)
					{
						cursorx=x;
						cursory++;
						lcd_set_cursor(cursorx, cursory); /* 设置光标位置 */
						lcd_write_ram_prepare();  
					}
					 LCD->LCD_RAM =RGB888toRGB565(buf[i+2],buf[i+1],buf[i]);
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
					if(Height>0)
			{
				cursorx=x;
				cursory=y+Height-1;
			while(1)
			{
			if(f_read(&SDFile,buf,40000,&br)==FR_OK)
			{
				for(int i=0;i<br;i+=I)
				{
					if(cursorx==x+Width)
					{
						cursorx=x;
						cursory--;
						lcd_set_cursor(cursorx, cursory); /* 设置光标位置 */
						lcd_write_ram_prepare();  
					}
					 LCD->LCD_RAM =RGB888toRGB565(buf[i+2],buf[i+1],buf[i]);
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
}
			else if(bmp.info.Count==8)
			{
								cursorx=x;
				cursory=y+Height-1;
			while(1)
			{
			if(f_read(&SDFile,buf,40000,&br)==FR_OK)
			{
				for(int i=0;i<br;i+=1)
				{
					if(cursorx==x+Width)
					{
						cursorx=x;
						cursory--;
						lcd_set_cursor(cursorx, cursory); /* 设置光标位置 */
						lcd_write_ram_prepare();  
					}
					LCD->LCD_RAM =RGB888toRGB565(colortb[buf[i]*4+0],colortb[buf[i]*4+1],colortb[buf[i]*4+2]);

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

			else if(bmp.info.Count==16)
			{
								cursorx=x;
				cursory=y+Height-1;
			while(1)
			{
			if(f_read(&SDFile,buf,40000,&br)==FR_OK)
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
					
					LCD->LCD_RAM =(((uint16_t)buf[i+1])<<8)|((uint16_t)buf[i]);

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


}
		f_close(&SDFile);
		return 0;
	}
	else
		
	{
		printf("opened fail %x",res);
		return -1;
	}
	
}

char show_bmp_8bit(uint16_t x,uint16_t y,char* path) 
{
	FRESULT res;
	bmp_head bmp;//bmp文件头、信息头
	uint32_t br;//
	static uint8_t colortb[256*4];//调色板缓存
	uint32_t cursorx,cursory;//当前写入屏幕位置
	static uint8_t buf[80000];//读取缓存
	res=f_open(&SDFile,path,FA_READ);
	if(res==FR_OK)
	{
		if(f_read(&SDFile,(char*)&bmp+2,sizeof(bmp_head)-2,&br)==FR_OK)//读取bmp文件头、信息头
		{
			int Height=bmp.info.Height;//读取bmp图像高
			int Width=bmp.info.Width;//读取bmp图像宽
			f_read(&SDFile, colortb,256*4,&br);//读取bmp调色板
			f_lseek(&SDFile,bmp.file.Offset);
			cursorx=x;
			cursory=y+Height-1;
			while(1){
				if(f_read(&SDFile,buf,80000,&br)==FR_OK){
					for(int i=0;i<br;i+=1){
						if(cursorx==x+Width){
							cursorx=x;
							cursory--;
							lcd_set_cursor(cursorx, cursory); /* 设置光标位置 */
							lcd_write_ram_prepare();  /* 准备写入显存 */
						}
						LCD->LCD_RAM =RGB888toRGB565(colortb[buf[i]*4+0],colortb[buf[i]*4+1],colortb[buf[i]*4+2]);//显示
						cursorx++;
					}
				}
				else{
					break;/*读取错误，跳出循环 */
				}
				if(br==0){
					break;/*文件读完，跳出循环*/
				}
			}
		}
		f_close(&SDFile);/*关闭文件 */
		return 0;
	}
  else{
		printf("opened fail %x",res);/*printf重定向 */ /*读取错误，串口输出信息 */
		return -1;
  }
}






/* USER CODE END Application */
