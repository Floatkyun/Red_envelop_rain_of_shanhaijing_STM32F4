工程文件在MDK-ARM\USTAR1.uvprojx （工程文件名跟着项目一代代传下来的）

实验现象：LCD正常显示游戏资源、可以和游戏进行交互、串口输出调试和帧率信息（波特率11520）

如果你只是想体验游戏 那么你只需修改game.c 文件中开始的几个的宏定义就可以直接更改游戏参数

如果你想利用已有函数进行自己的开发，那么你可以关注sram.c、fatfs.c、lcd.c这几个文件和对应的头文件
sram.c中有：（在使用这些函数之前，你需要确保要读取的图片格式为bmp并且色深为16bit采用RGB565颜色编码）
1. sram的FSMC初始化函数（我的CubeMX配置的sram的FSMC初始化函数会导致后面访问寄存器的时候卡hardfault handler 所以移植了一个初始化函数过来）
2. sram测试函数以及读写函数（实际使用直接访问寄存器地址就可以，不用这个读写函数）
3. sram_load_bmp_from_sd 从sd卡加载图片到sram的函数（如果这个图片需要经常用来显示，那就先把它加载到sram，让后从sram进行读取显示，这样会极大提高图片刷新的速度和稳定性）
4. show_bmp_from_sram 从sram显示图片到LCD的函数
5.show_bmp_from_sram_with_background 从sram显示带有背景的图片（你可以利用它来在背景上显示非矩形或镂空物体，只需要确保图片中不想要的部分为纯白色即可）
6.range_show_bmp_from_sram 从sram显示背景图片的一个区域的函数（你可以利用它来遮盖物体移动后留下的残影 实现局部刷新）
7. Load_bmp_with_backgroung_from_sram 从sd卡显示带有背景的图片（你可以利用它来在背景上显示不需要经常刷新的非矩形或镂空物体，只需要确保图片中不想要的部分为纯白色即可）（函数有大型静态数组，确保有足够的栈空间）

fatfs.c中有：
显示汉字：
1. show_chinese_char 从sd卡显示单个汉字 若要从sd卡显示汉字字符串，请到lcd中找对应函数
2. load_chinese_str 从sd卡加载汉字字符串到结构体 如果你有经常需要刷新的文字，那么就先把它加载到结构体中 让后从结构体进行读取显示（缺少使用测试）
3. show_chinese_str_from_struct 从结构体显示汉字字符串到lcd（缺少使用测试）
显示图片：
4. scan_files 接收文件夹路径为参数 遍历并显示文件夹下所有bmp图片 实现动画效果（只支持色深为16bit采用RGB565颜色编码）
5. Load_bmp 接收bmp图片路径作为参数 支持显示8bit、16bit、24bit、32bit 色深的bmp图片（函数有大型静态数组，确保有足够的栈空间）

lcd.c中有(只写原来驱动中没有的且实用的函数)：
1. lcd_show_pic 显示图片，测试图片数据在pic.h中 用这条代码查看显示效果 lcd_show_pic(0, 0, 240-1, 400-1, gImage_catmiao240_400);
2. lcd_show_inverted_pic 由于从bmp图片中读取到的数据扫描方向特殊，需要用这个函数进行显示
3. lcd_show_font_str 从sd卡显示中文、英文、数字混合字符串

另外，你可以在game.c中找到check_click函数，用于检查触控点击是否在指定矩形区域中

驱动移植：
LCD屏幕驱动：
lcd.c、lcd_ex.c
lcd.h、lcd_ex.h、lcdfont.h
触控芯片驱动：
24cxx.c、ctiic.c、ft5206.c、gt9xxx.c、myiic.c、touch.c
24cxx.h、ctiic.h、ft5206.h、gt9xxx.h、myiic.h、touch.h
