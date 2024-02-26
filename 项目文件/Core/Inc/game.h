#ifndef __GAME_H
#define __GAME_H

#include "sram.h"
#include "usart.h"
#include "main.h"
#include "lcd.h"
#include "delay.h"
#include "touch.h"
#include "rng.h"
#include <stdlib.h>

#define LCDWIDTH 480

#define CHANNEL_1 (LCDWIDTH/6)
#define CHANNEL_2 (LCDWIDTH/2)
#define CHANNEL_3 (LCDWIDTH*5/6)

#define HONGBAO 0 //item_name
#define LUOBO 1


typedef struct xinnai_item 
{
	sram_bmp_file* bmp_file;
	int pos_y;
	int last_pos_x;
	int cur_pos_x;
	
}xinnai_item;

typedef struct drop_node 
{
	sram_bmp_file* bmp_file;
	int pos_channel;//choose from CHANNEL_1 CHANNEL_2 CHANNEL_3
	int pos_x;
	int last_pos_y;
	int cur_pos_y;
	int NAME;//choose from HONGBAO LUOBO
	struct drop_node* next;
	
}drop_node;

extern int DROP_SPEED;
extern int point;
extern int count_down;
extern int health_point;
	
extern drop_node* HEAD;
extern sram_bmp_file* xinnai;
extern sram_bmp_file* xinnai_sad;
extern sram_bmp_file* xinnai_happy;
extern sram_bmp_file* back;
extern sram_bmp_file* hongbao;
extern sram_bmp_file* luobo;

extern sram_bmp_file* DROP_BMP_FILE[2];

extern xinnai_item *XINNAI_ITEM;

extern int XINNAI_W;
extern int x_XINNAI;


int rand_int();//返回随机数

void game_init();   //初始化游戏
void game_reinit();//重新初始化游戏

drop_node* create_drop_item(int item_name,int channel);  //创建一个下落物体对象
drop_node* place_drop_item();//放置一个下落物体对象
void add_new_drop_item(void);//将下落物体对象添加到对象链表中
int check_overlap_with_xinnai(drop_node* drop_item);//检查下落物体对象是否和心奈接触
int check_drop_item(drop_node* drop_item);//检查下落物体对象是否符合销毁条件
void destroy_drop_item(void);//销毁所有应该被销毁的下落物体对象

void move_drop_item(drop_node* drop_item);//向下移动下落物体对象
void move_all_drop_item(void);//向下移动所有在链表中的下落物体对象
void move_xinnai(void);//左右移动心奈

void show_move_drop(drop_node* drop_item,sram_bmp_file* background);//区域刷新屏幕 显示一个下落物体对象
void show_all_move_drop(void);//区域刷新屏幕 显示所有下落物体对象
void show_move_xinnai(void);//区域刷新屏幕 显示心奈

int update_xinnai_pos(void);//从触控模块获取心奈最新位置

int check_click(int x,int w,int y,int h);//检查点击是否在区域内

void game(void);//游戏本体

#endif