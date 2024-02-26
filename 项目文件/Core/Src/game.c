#include "game.h"

#define DROP_SPEED_BASE 4//游戏速度
int DROP_SPEED=DROP_SPEED_BASE; //游戏实时速度
#define DROP_GAP 60//刷新最小位置间隔
#define DROP_WAIT_TIME 8//刷新最小时间间隔（按帧计算）
#define DROP_DENSITY 12 //of 100
#define DROP_HONGBAO_RATIO 85 //红包生成比例
#define LOSS_NUM_OF_LUOBO 5//活力值，最多碰到的萝卜数

int DROP_CHANNELS[3]={CHANNEL_1,CHANNEL_2,CHANNEL_3};

drop_node* HEAD;

sram_bmp_file* xinnai;
sram_bmp_file* xinnai_sad;
sram_bmp_file* xinnai_happy;
sram_bmp_file* back;
sram_bmp_file* hongbao;
sram_bmp_file* luobo;
sram_bmp_file* DROP_BMP_FILE[2];

int XINNAI_W;
int x_XINNAI;

int point=0;
int health_point=LOSS_NUM_OF_LUOBO;
int count_down=120;

xinnai_item *XINNAI_ITEM;

int rand_int()
{
	return HAL_RNG_GetRandomNumber(&hrng)%100;
}
void game_init()
{
	HEAD=(drop_node*)malloc(sizeof(drop_node));
	HEAD->bmp_file=NULL;
	HEAD->cur_pos_y=0;
	HEAD->last_pos_y=0;
	HEAD->next=NULL;
	HEAD->pos_channel=0;
	HEAD->pos_x=0;
	HEAD->NAME=-1;
	
  xinnai=sram_load_bmp_from_sd("GAME/XINNAI.bmp");
	xinnai_sad=sram_load_bmp_from_sd("GAME/XINNAI2.bmp");
	xinnai_happy=sram_load_bmp_from_sd("GAME/XINNAI3.bmp");
  back=sram_load_bmp_from_sd("GAME/BACK.bmp");
  hongbao=sram_load_bmp_from_sd("GAME/HONGBAO.bmp");
  luobo=sram_load_bmp_from_sd("GAME/LUOBO.bmp");
	

	DROP_BMP_FILE[HONGBAO]=hongbao;
	DROP_BMP_FILE[LUOBO]=luobo;
	
	XINNAI_W= xinnai->w;
	x_XINNAI=(LCDWIDTH-XINNAI_W)/2;
	
	XINNAI_ITEM=(xinnai_item *)malloc(sizeof(xinnai_item));
	XINNAI_ITEM->bmp_file=xinnai;
	XINNAI_ITEM->pos_y=lcddev.height-xinnai->h;
	XINNAI_ITEM->cur_pos_x=x_XINNAI;
	XINNAI_ITEM->last_pos_x=x_XINNAI;
	
  show_bmp_from_sram(0,0,back);
	
	lcd_fill(0, 0, 480-1, 24-1, LGRAY); 

	lcd_show_font_str(0, 0,"倒计时",24,BLACK,0);
	lcd_show_xnum(71,0,count_down,3,24,0,BLACK);

	lcd_show_font_str(371, 0,"分数",24,BLACK,0);
	lcd_show_xnum(419,0,point,5,24,0,BLACK);

	lcd_show_font_str(197, 0,"活力值",24,BLACK,0);
	lcd_show_xnum(269,0,health_point,2,24,0,BLACK);
}
void game_reinit()
{
	x_XINNAI=(LCDWIDTH-XINNAI_W)/2;
	XINNAI_ITEM->cur_pos_x=x_XINNAI;
	XINNAI_ITEM->last_pos_x=x_XINNAI;
	point=0;
	count_down=120;
	health_point=LOSS_NUM_OF_LUOBO;
	DROP_SPEED=DROP_SPEED_BASE;
	
	drop_node* temp;
	while(HEAD->next!=NULL)
	{
		temp=HEAD->next->next;
		free(HEAD->next);
		HEAD->next=temp;
	}
	show_bmp_from_sram(0,0,back);
		 
	 lcd_fill(0, 0, 480-1, 24-1, LGRAY);
	
	 lcd_show_font_str(0, 0,"倒计时",24,BLACK,0);
	 lcd_show_xnum(71,0,count_down,3,24,0,BLACK);

	 lcd_show_font_str(371, 0,"分数",24,BLACK,0);
	 lcd_show_xnum(419,0,point,5,24,0,BLACK);

	 lcd_show_font_str(197, 0,"活力值",24,BLACK,0);
	 lcd_show_xnum(269,0,health_point,2,24,0,BLACK);
}

drop_node* create_drop_item(int item_name,int channel)
{
	sram_bmp_file* bmp_file=DROP_BMP_FILE[item_name];
	drop_node* drop_item=(drop_node*)malloc(sizeof(drop_node));
	drop_item->NAME=item_name;
	drop_item->bmp_file=bmp_file;
	drop_item->cur_pos_y=-bmp_file->h;
	drop_item->last_pos_y=-bmp_file->h;
	drop_item->next=NULL;
	drop_item->pos_channel=channel;
	drop_item->pos_x=channel-bmp_file->w/2;
	

	
	return drop_item;
}

drop_node* place_drop_item(void)
{
	uint8_t flag_ch[3]={1,1,1};
	drop_node* temp_node =HEAD;
	while(temp_node->next!=NULL)
	{
		temp_node=temp_node->next;
		switch(temp_node->pos_channel)
		{
			case CHANNEL_1:
				if(temp_node->cur_pos_y<DROP_GAP)flag_ch[0]=0;
			break;
			case CHANNEL_2:
				if(temp_node->cur_pos_y<DROP_GAP)flag_ch[1]=0;
			break;
			case CHANNEL_3:
				if(temp_node->cur_pos_y<DROP_GAP)flag_ch[2]=0;
			break;
		}
	}
	
	int probability = (flag_ch[0]+flag_ch[1]+flag_ch[2])*DROP_DENSITY;
	if(rand_int()>probability)//生成概率
	{
		return NULL;
	}
	
	int choose_num=rand_int()%(flag_ch[0]+flag_ch[1]+flag_ch[2]);
	int choose_ch;
	uint8_t count=0;
	
	for(int i=0;i<3;i++)//随机选择生成通道
	{
		if(flag_ch[i]==1)
		{
			if(count==choose_num)
			{
				choose_ch=DROP_CHANNELS[i];
				break;
			}
			count++;
		}
	}
	
	if(rand_int()<DROP_HONGBAO_RATIO)
	{
		return create_drop_item(HONGBAO,choose_ch);
	}
	else 
	{
		return create_drop_item(LUOBO,choose_ch);
	}
}

void add_new_drop_item(void)///////////////////////////
{
	if(fps%DROP_WAIT_TIME==0)
	{
		drop_node* new_item=place_drop_item();
		if(new_item!=NULL)
		{
			drop_node* temp=HEAD->next;
			HEAD->next=new_item;
			new_item->next=temp;
		}
	}
}


int check_overlap_with_xinnai(drop_node* drop_item)
{
	int xinnai_w=xinnai->w;
	int xinnai_h=xinnai->h;
	int drop_w=drop_item->bmp_file->w;
	int drop_h=drop_item->bmp_file->h;
	int L_2x=abs(XINNAI_ITEM->cur_pos_x*2+xinnai_w-drop_item->pos_x*2-drop_w);
	int L_2y=abs(XINNAI_ITEM->pos_y*2+xinnai_h-drop_item->cur_pos_y*2-drop_h);
	if(L_2x<=xinnai_w+drop_w && L_2y<=xinnai_h+drop_h)
		return 1;
	else
		return 0;
}

int check_drop_item(drop_node* drop_item)
{
	if(drop_item->cur_pos_y> lcddev.height-1)
	{
		return 1;
	}
	else if(check_overlap_with_xinnai(drop_item))
	{
		if (drop_item->NAME==LUOBO)//碰到萝卜了
		{
			XINNAI_ITEM->bmp_file=xinnai_sad;
			health_point--;
			lcd_show_xnum(269,0,health_point,2,24,0,BLACK);
		}
		else if (drop_item->NAME==HONGBAO)//碰到红包了
		{
			XINNAI_ITEM->bmp_file=xinnai_happy;
			point+=100;
			lcd_show_xnum(419,0,point,5,24,0,BLACK);
			switch(point)
			{
				case 2000:
					DROP_SPEED=DROP_SPEED_BASE*3/2;
				break;
				case 5000:
					DROP_SPEED=DROP_SPEED_BASE*2;
				break;
								
				case 8000:
					DROP_SPEED=DROP_SPEED_BASE*5/2;
				break;
				
				case 11000:
					DROP_SPEED=DROP_SPEED_BASE*3;
				break;
				
				case 15000:
					DROP_SPEED=DROP_SPEED_BASE*7/2;
				break;
								
				case 20000:
					DROP_SPEED=DROP_SPEED_BASE*4;
				break;
			}
			
			
			
		}
		range_show_bmp_from_sram(drop_item->pos_x,drop_item->last_pos_y,drop_item->bmp_file->w, drop_item->bmp_file->h,back);
		return 1;
	}
	return 0;
}

void destroy_drop_item(void)///////////////////////////
{
	drop_node* temp_node =HEAD;
	while(temp_node->next!=NULL)
	{
		if(check_drop_item(temp_node->next))
		{
			drop_node* temp=temp_node->next->next;
			free(temp_node->next);
			temp_node->next=temp;
		}
		else
		{
			temp_node=temp_node->next;
		}
	}
}
void move_drop_item(drop_node* drop_item)
{
	drop_item->cur_pos_y=drop_item->cur_pos_y+=DROP_SPEED;
}

void show_move_drop(drop_node* drop_item,sram_bmp_file* background)
{
	int height=drop_item->cur_pos_y-drop_item->last_pos_y;
	int width=drop_item->bmp_file->w;

	show_bmp_from_sram_with_background(drop_item->pos_x,drop_item->cur_pos_y,drop_item->bmp_file,background);
	range_show_bmp_from_sram(drop_item->pos_x,drop_item->last_pos_y,width, height+1,background);
	drop_item->last_pos_y=drop_item->cur_pos_y;
}

void move_all_drop_item(void)///////////////
{
	drop_node* temp_node =HEAD;
	while(temp_node->next!=NULL)
	{
		temp_node=temp_node->next;
		move_drop_item(temp_node);
	}
}

void show_all_move_drop(void)////////////////
{
	drop_node* temp_node =HEAD;
	while(temp_node->next!=NULL)
	{
		temp_node=temp_node->next;
		show_move_drop(temp_node,back);
	}
}

void move_xinnai(void)////////////////////////////
{
	x_XINNAI=update_xinnai_pos();
	XINNAI_ITEM->cur_pos_x=x_XINNAI;
}

void show_move_xinnai(void)/////////////////////////
{	
	show_bmp_from_sram_with_background(XINNAI_ITEM->cur_pos_x,XINNAI_ITEM->pos_y,XINNAI_ITEM->bmp_file,back);
	int width=XINNAI_ITEM->cur_pos_x-XINNAI_ITEM->last_pos_x;
	if(width==0)return ;
	int height=xinnai->h;
	int x0;
	int y0=XINNAI_ITEM->pos_y;
	if(width>0)
	{
		x0=XINNAI_ITEM->last_pos_x;
	}
	else if(width<0)
	{
		x0=XINNAI_ITEM->cur_pos_x+xinnai->w;
	}
	range_show_bmp_from_sram(x0,y0,abs(width)+1, height,back);
	XINNAI_ITEM->last_pos_x=XINNAI_ITEM->cur_pos_x;
}

int update_xinnai_pos(void)////////////////////////////
{
  uint8_t maxp = 5;
	int temp;
	int x_pos;
	int flag=0;
  if (lcddev.id == 0x1018)maxp = 10;
  tp_dev.scan(0);
        for (uint8_t t = 0; t < maxp; t++)
        {
            if ((tp_dev.sta) & (1 << t))
            {
							
                if (tp_dev.x[t] > 0&&tp_dev.x[t] < lcddev.width && tp_dev.y[t] < lcddev.height)  /* 坐标在屏幕范围内 */
                {                    							
							if(tp_dev.x[t] < XINNAI_W/2+5)
							{
								temp=84+5;
							}
															
							else if(tp_dev.x[t] > lcddev.width-XINNAI_W/2-5)
							{
								temp=lcddev.width-XINNAI_W/2-5;
							}
							else 
							{
								temp=tp_dev.x[t];
							}
                     x_pos= temp-XINNAI_W/2;
										flag=1;
                }
            }
        }
		if(flag==1)
		{
			return x_pos;
		}
		else
		{
			return x_XINNAI;
		}
}
int check_click(int x,int w,int y,int h)////////////////////////////
{
   uint8_t maxp = 5;
  if (lcddev.id == 0x1018)maxp = 10;
        tp_dev.scan(0);
        for (uint8_t t = 0; t < maxp; t++)
        {
            if ((tp_dev.sta) & (1 << t))
            {
                if (tp_dev.x[t] > x&&tp_dev.x[t] < x+w && tp_dev.y[t] < y+h && tp_dev.y[t] > y)  /* 坐标在范围内 */
                {                    							
										return 1;
                }
            }
        }
		return 0;
}
void game(void)
{
	add_new_drop_item();
	
	move_all_drop_item();
	move_xinnai();
	
	destroy_drop_item();
	
	show_all_move_drop();

// lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color);
	show_move_xinnai();
	
	
}







