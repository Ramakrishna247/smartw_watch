#include "stm32f0xx.h"

volatile unsigned int var=0;
volatile unsigned int mode_var=0;
volatile unsigned int set_var=0;


unsigned int hr=0;
unsigned int min=0;
unsigned int sec=0;
unsigned int dt=0;
unsigned int wk=0;
unsigned int mnt=0;
unsigned int yr=0;


void display_cmd(unsigned char Data);	//display_cmd
void display_data(unsigned char Data);	//display_data	 
void I2C_START(void);
void I2C_WRITE(unsigned char x);
void I2C_STOP(void);
void display_buff(unsigned char ch);
void system_init(void);
void i2c_int(void);
void hex2char(int num,unsigned char *str);
void rtc_init(void);
void delay(void);
void display_date(unsigned int x);
void display_week(unsigned int );
void display_letter(unsigned int ch);
void displayTime(unsigned char *ptr);
void time_loop(void);
void update_time(void);
void display_digit(unsigned int ch);
void display_num(unsigned int n);
void set_time(void);
/*---------------------interrupt Handler for EXTI0----------------*/
void EXTI0_1_IRQHandler(void)
{
  
	
	if(EXTI->PR & EXTI_PR_PR0)
  {
     EXTI->PR |= EXTI_PR_PR0;
     var=3;
  }

}

/*-----------------Interrupt Handler for EXTI4 and 5---------------*/

void EXTI4_15_IRQHandler(void)
{
  
	
	if(EXTI->PR & EXTI_PR_PR4)
  {
     EXTI->PR |= EXTI_PR_PR4;
		
    
		set_var++;
  }

	if(EXTI->PR & EXTI_PR_PR5)
  {
     EXTI->PR |= EXTI_PR_PR5;
		 mode_var++;
		 set_var=0;
  }

}

/*------------------Array for small digits-----------------------------*/
unsigned char buffer[12][7] ={
0x00,0x7e,0xe1,0x91,0x8d,0x7e,0x00, //0
0x00,0x82,0x83,0xff,0x80,0x80,0x00, //1
0x00,0xc2,0xa1,0x91,0x89,0x86,0x00, //2
0x00,0x91,0x91,0x91,0x91,0x6e,0x00, //3
0x00,0x1f,0x10,0x10,0x10,0xff,0x00, //4
0x00,0x8f,0x89,0x89,0x89,0x71,0x00, //5
0x00,0xff,0x91,0x91,0x91,0xf3,0x00, //6
0x00,0x01,0xe1,0x11,0x09,0x07,0x00, //7
0x00,0xff,0x91,0x91,0x91,0xff,0x00, //8
0x00,0x8f,0x89,0x89,0x89,0xff,0x00, //9
0x00,0x00,0x66,0x66,0x66,0x00,0x00, //:
0x00,0x40,0x20,0x10,0x08,0x04,0x00  ///
	};
/*-------------------Arrary for big digits-------------------------------*/
unsigned char bufferNum[11][20] ={
0x00,0xfc,0xfe,0x07,0x87,0xe7,0x37,0xfe,0xfc,0x00,0x00,0x3f,0x7f,0xee,0xe3,0xe0,0xe0,0x7f,0x3f,0x00, //0
0X00,0X08,0X0C,0X0E,0XFF,0XFF,0XFF,0X00,0X00,0X00,0X00,0XC0,0XC0,0XC0,0XFF,0XFF,0XFF,0XC0,0XC0,0X00, //1
0X00,0X1C,0X1E,0X07,0X03,0X03,0X87,0XFE,0XFC,0X00,0X00,0XF0,0XF8,0XFC,0XCE,0XC7,0XC3,0XC1,0XC0,0X00, //2
0x00,0x02,0x83,0x83,0x83,0x83,0x82,0x7c,0x38,0x00,0x00,0x40,0xc1,0xc1,0xc1,0xc1,0x41,0x37,0x1c,0x00, //3
0x00,0xff,0xff,0x00,0x00,0x00,0x00,0xff,0xff,0x00,0x00,0x07,0x07,0x06,0x06,0x06,0x06,0xff,0xff,0x00, //4
0x00,0xff,0xff,0x83,0x83,0x83,0x83,0x07,0x07,0x00,0x00,0xc1,0xc1,0xc1,0xc1,0xc1,0xc1,0x7f,0x3e,0x00, //5
0x00,0xff,0xff,0x03,0x03,0x03,0x03,0x07,0x07,0x00,0x00,0xff,0xff,0xc3,0xc3,0xc3,0xc3,0xff,0xff,0x00, //6
0x00,0x03,0x03,0x03,0x03,0x83,0xc3,0xff,0x7f,0x00,0x00,0x00,0x00,0xfe,0xff,0x03,0x01,0x00,0x00,0x00, //7
0x00,0xff,0xff,0x03,0x03,0x03,0x03,0xff,0xff,0x00,0x00,0xff,0xff,0xc3,0xc3,0xc3,0xc3,0xff,0xff,0x00, //8
0x00,0xff,0xff,0xc3,0xc3,0xc3,0xc3,0xff,0xff,0x00,0x00,0xe1,0xe1,0xc1,0xc1,0xc1,0xc1,0xff,0xff,0x00, //9
0x00,0x00,0x00,0x3c,0x3c,0x3c,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x3c,0x3c,0x3c,0x00,0x00,0x00  //:
};

/*---------------------------Array for Letters----------------------*/
	unsigned char letters[26][9]={
	0x00, 0xfe, 0x11, 0x11, 0x11, 0x11, 0x11, 0xfe, 0x00, //A-1
	0x00, 0xff, 0x91, 0x91, 0x91, 0x91, 0x91, 0x6e, 0x00, //B-2
	0x00, 0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x00, //C-3
	0x00, 0x81, 0xff, 0x81, 0x81, 0x81, 0x81, 0x7e, 0x00, //D-4
	0x00, 0xff, 0x91, 0x91, 0x91, 0x91, 0x91, 0x81, 0x00, //E-5
  0x00, 0xff, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x00, //F-6
  0x00, 0x7e, 0x81, 0x81, 0x81, 0x91, 0x91, 0x70, 0x00, //G-7
  0x00, 0xff, 0x10, 0x10, 0x10, 0x10, 0x10, 0xff, 0x00, //H-8
  0x00, 0x81, 0x81, 0x81, 0xff, 0x81, 0x81, 0x81, 0x00, //I-9
  0x00, 0x60, 0x80, 0x81, 0x81, 0x7f, 0x01, 0x01, 0x00, //J-10
  0x00, 0x00, 0xff, 0x18, 0x24, 0x42, 0x81, 0x00, 0x00, //K-11
  0x00, 0xff, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, //L-12
  0x00, 0xff, 0x02, 0x04, 0x08, 0x04, 0x02, 0xff, 0x00, //M-13
  0x00, 0xff, 0x02, 0x04, 0x08, 0x10, 0x20, 0xff, 0x00, //N-14
  0x00, 0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e, 0x00, //0-15
  0x00, 0xff, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e, 0x00, //P-16
  0x00, 0x7e, 0x81, 0x81, 0x91, 0xa1, 0x41, 0xbe, 0x00, //Q-17
  0x00, 0xff, 0x11, 0x11, 0x31, 0x51, 0x91, 0x0e, 0x00, //R-18
  0x00, 0x8e, 0x91, 0x91, 0x91, 0x91, 0x91, 0x61, 0X00, //S-19
  0x00, 0x01, 0x01, 0x01, 0xff, 0x01, 0x01, 0x01, 0X00, //T-20
  0x00, 0x7f, 0x80, 0x80, 0x80, 0x80, 0x80, 0x7f, 0X00, //U-21
	0x00, 0x1f, 0x20, 0x40, 0x80, 0x40, 0x20, 0x1f, 0X00, //V-22
  0x00, 0xff, 0x40, 0x20, 0x10, 0x20, 0x40, 0xff, 0X00, //W-23
  0x00, 0x81, 0x42, 0x24, 0x18, 0x24, 0x42, 0x81, 0X00, //X-24
  0x00, 0x01, 0x02, 0x04, 0xf8, 0x04, 0x02, 0x01, 0X00, //Y-25
  0x00, 0xc1, 0xa1, 0x91, 0x89, 0x85, 0x83, 0x81, 0X00, //Z-26
	};
	
/*-------------------------Main call---------------------------*/	
int main()
{
delay();
system_init();	
i2c_int();

rtc_init();
	

	
/*------------------main while loop-----------------*/	
	while(1)
	{
 
	 if(var>0)
	  {
	  time_loop();
  	}	
		if(mode_var>0)
		{
		display_cmd(0x21); 
    display_cmd(24);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(1);
    display_cmd(7);
		display_letter(7);
		display_digit(10);	
		 
		display_cmd(0x21); 
    display_cmd(62);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(1);
    display_cmd(7);
		display_letter(12);
		display_digit(10);	
		 
		display_cmd(0x21); 
    display_cmd(24);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(3);
    display_cmd(7);
		display_letter(3);
		display_letter(19);
		display_digit(10); 
		
		display_cmd(0x21); 
    display_cmd(68);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(3);
    display_cmd(7);
		display_letter(12);
		display_letter(13);
		display_digit(10); 
		
		display_cmd(0x21); 
    display_cmd(24);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(5);
    display_cmd(7);
		display_letter(22);
		display_letter(10);
		display_digit(10); 
		
		display_cmd(0x21); 
    display_cmd(68);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(5);
    display_cmd(7);
		display_letter(24);
		display_letter(17);
		display_digit(10); 
		update_time();
		}
	}
}

/*----------------time_loop-------------*/

void time_loop(void)
{
	
unsigned int time;
unsigned int date;
unsigned char z[7];

    display_cmd(0x21); //set coloumn address
    display_cmd(0);    //
    display_cmd(127);
    display_cmd(0x22); //setting row address
    display_cmd(0);
    display_cmd(7);
		
		int j=0;
		for (j=0; j<1024; j++)      // Write Zeros to clear the display
    {
        display_data(0);
    }
	
display_cmd(0xAF);//SSD1306_DISPLAY_ON
while(var>0)
{

if((RTC->ISR & RTC_ISR_RSF) == RTC_ISR_RSF)
{
time=RTC->TR; //store time value in the integer variable  dat
date=RTC->DR; // store date value in the integer variabledate
}
hex2char(time,z); //convert integer value to characters and store in character array z 
			
	displayTime(z);
	
	display_date(date);
	display_week(date);
var--;
delay();
}
var=0;
   
    display_cmd(0x21); //set coloumn address
    display_cmd(0);    //
    display_cmd(127);
    display_cmd(0x22); //setting row address
    display_cmd(0);
    display_cmd(7);
		
		
		for (j=0; j<1024; j++)      // Write Zeros to clear the display
    {
        display_data(0);
    }
display_cmd(0xAE);//SSD1306_DISPLAY_Off

}

/*----------------------------set_time------------------*/
void update_time(void)
{
	display_cmd(0xAF);
	if(mode_var==1)
{
	  display_cmd(0x21); 
    display_cmd(40);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(1);
    display_cmd(7);

  if(set_var>24)
	{
	set_var=0;
	}
	
 hr=set_var;
 display_num(hr);
 
}
if(mode_var==2)
{
	  display_cmd(0x21); 
    display_cmd(80);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(1);
    display_cmd(7);
	
  if(set_var>59)
	{
	set_var=0;
	}
	
 min=set_var;
 display_num(min);

}

if(mode_var==3)
{
	  display_cmd(0x21); 
    display_cmd(50);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(3);
    display_cmd(7);

  if(set_var>31)
	{
	set_var=0;
	}
	
 dt=set_var;
 display_num(dt);
 
}

if(mode_var==4)
{
	  display_cmd(0x21); 
    display_cmd(94);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(3);
    display_cmd(7);

  if(set_var>12)
	{
	set_var=0;
	}
	
 mnt=set_var;
 display_num(mnt);
 
}

if(mode_var==5)
{
	  display_cmd(0x21); 
    display_cmd(50);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(5);
    display_cmd(7);

  if(set_var>7)
	{
	set_var=0;
	}
	
 wk=set_var;
 display_num(wk);
 
}


if(mode_var==6)
{
	  display_cmd(0x21); 
    display_cmd(94);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(5);
    display_cmd(7);

  if(set_var>99)
	{
	set_var=0;
	}
	
 yr=set_var;
 display_num(yr);
 
}

if(mode_var==7)
{
	
	set_time();
	mode_var=0;
	set_var=0;
	display_cmd(0xAE);
 
}

if(mode_var>7)
{
mode_var=0;
}

}

/*----------------display_num function----------------*/
void display_num(unsigned int n)
{
int i,x,y;
	y=n/10;
	
	for(i=0;i<8;i++)
	{
	display_data(buffer[y][i]);
	
	}
x=n%10;
for(i=0;i<8;i++)
	{
	display_data(buffer[x][i]);
	
	}

}
/*-----------------------------------------------------------------*/
/*---------------------display command-------------------------------------*/
void display_cmd(unsigned char x)
{
unsigned char control=0x00;
   
	I2C_START();
	I2C_WRITE(control);
	I2C_WRITE(x);
	I2C_STOP();

}


/*-----------------------------------display data----------------------------*/
void display_data(unsigned char x)
{
	unsigned char data=0x40; 
  I2C_START();
	I2C_WRITE(data);
	I2C_WRITE(x);
	I2C_STOP();

}

/*---------------------I2c start---------------------------------------------*/
void I2C_START(void)
{
I2C1->CR2 |=I2C_CR2_START;
while(I2C1->CR2 &I2C_CR2_START){;}  
	                                   
}

/*------------------------------------i2c write-------------------------------*/
void I2C_WRITE(unsigned char x){
I2C1->TXDR =x;
while(!(I2C1->ISR & I2C_ISR_TXE)){;}

}
/* -----------------------------------------i2c stop--------------------------*/
void I2C_STOP(void){

I2C1->CR2|=I2C_CR2_STOP;
while(I2C1->CR2 &I2C_CR2_STOP){;}	
}

/*--------------------System Initialization-----------------------*/



void system_init()
{
/*----------------I2c1 configuration------------------*/
	//PA10 is SDA PIN
	//PA9 IS SCL PIN
	
	

  RCC->AHBENR =RCC_AHBENR_GPIOAEN;    //Enabling clock for gpio port A
  RCC->APB1ENR=RCC_APB1ENR_I2C1EN;    //Enabling clock for I2C port
	RCC->APB2ENR |=RCC_APB2ENR_SYSCFGEN; //enable Syscnfg clock
	
	RCC->CR |=RCC_CR_HSION;    //SWITCHING ON HSI CLOCK
	RCC->CFGR3 &=~RCC_CFGR3_I2C1SW; //SELECTING HSI AS CLOCK SOURCE FOR THE I2C1
	
	GPIOA->MODER |=GPIO_MODER_MODER10_1;    //selecting mode as alternating function mode pin 1-high pin 0-low
	GPIOA->MODER &=~GPIO_MODER_MODER10_0;   //selecting mode as alternating function mode pin 1-high pin 0-low
	
	GPIOA->MODER |=GPIO_MODER_MODER9_1;     //selecting mode as alternating function mode pin 1-high pin 0-low
	GPIOA->MODER &=~GPIO_MODER_MODER9_0;    //selecting mode as alternating function mode pin 1-high pin 0-low
	
	
	GPIOA->OTYPER |=GPIO_OTYPER_OT_10;    //selecting output type as open drain pin-HIGH
	GPIOA->OTYPER |=GPIO_OTYPER_OT_9;     //selecting output type as open drain pin-HIGH
	
	
	GPIOA->PUPDR |=GPIO_PUPDR_PUPDR10_0;   //enabling pull up ressistor and disbling pulldown resistor
	GPIOA->PUPDR &=~GPIO_PUPDR_PUPDR10_1; //enabling pull up ressistor and disbling pulldown resistor
	
	GPIOA->PUPDR |=GPIO_PUPDR_PUPDR9_0;     //enabling pull up ressistor and disblingpulldown resistor
	GPIOA->PUPDR &=~GPIO_PUPDR_PUPDR9_1;    //enabling pull up ressistor and disblingpulldown resistor
	
	
GPIOA->AFR[1]=0x00000440 ;   //aleternate function number for i2c is 4, selected for pin 9,10 (for uart AF number is 1)
	
GPIOA->MODER &= ~GPIO_MODER_MODER0;
GPIOA->PUPDR |=GPIO_PUPDR_PUPDR0_0;
GPIOA->PUPDR &=~GPIO_PUPDR_PUPDR0_1;

GPIOA->MODER &= ~GPIO_MODER_MODER4;
GPIOA->PUPDR |=GPIO_PUPDR_PUPDR4_0;
GPIOA->PUPDR &=~GPIO_PUPDR_PUPDR4_1;

GPIOA->MODER &= ~GPIO_MODER_MODER5;
GPIOA->PUPDR |=GPIO_PUPDR_PUPDR5_0;
GPIOA->PUPDR &=~GPIO_PUPDR_PUPDR5_1;


	
SYSCFG->EXTICR[1] |=SYSCFG_EXTICR1_EXTI0_PA;
EXTI->IMR |= 0x0001; 
EXTI->FTSR |= 0x0001; 
NVIC_EnableIRQ(EXTI0_1_IRQn); 
NVIC_SetPriority(EXTI0_1_IRQn,0);

EXTI->IMR |=0X0030;
EXTI->FTSR |=0X0030;
NVIC_EnableIRQ(EXTI4_15_IRQn);
NVIC_SetPriority(EXTI4_15_IRQn,1);

}

/*----------------------------I2C peripheral and display Initialization ------------------------- */

void i2c_int()
{

I2C1->CR1 &=~I2C_CR1_PE;
I2C1->CR1 |=I2C_CR1_TCIE;
I2C1->CR1 |=I2C_CR1_STOPIE;
I2C1->CR1 |=I2C_CR1_TXIE;
I2C1->CR1 |=I2C_CR1_RXIE;

I2C1->TIMINGR = (uint32_t)0x0010020A; 
	
 I2C1->CR1 |=I2C_CR1_PE;
 I2C1->CR2=(0X3C <<1);
 I2C1->CR2 &=~I2C_CR2_RD_WRN;
 I2C1->CR2 |=(2<<16);
	 

	 
    display_cmd(0xAE);
	  display_cmd(0xD5);
    display_cmd(0x80);
    display_cmd(0xA8); //SSD1306_SET_MULTIPLEX_RATIO
    display_cmd(0x3F);
    display_cmd(0xD3); //SSD1306_SET_DISPLAY_OFFSET
    display_cmd(0x0);
    display_cmd(0x40 | 0x0);//SSD1306_SET_START_LINE
    display_cmd(0x8D); //SSD1306_CHARGE_PUMP
    display_cmd(0x14);
    display_cmd(0x20);//SSD1306_MEMORY_ADDR_MODE
    display_cmd(0x00);
    display_cmd(0xA0 | 0x1);//SSD1306_SET_SEGMENT_REMAP
    display_cmd(0xC8);//SSD1306_COM_SCAN_DIR_DEC
    display_cmd(0xDA);//SSD1306_SET_COM_PINS
    display_cmd(0x12);
    display_cmd(0x81);//SSD1306_SET_CONTRAST_CONTROL
    display_cmd(0xcf);
    display_cmd(0xD9);//SSD1306_SET_PRECHARGE_PERIOD
    display_cmd(0xF1);
    display_cmd(0xDB);//SSD1306_SET_VCOM_DESELECT
    display_cmd(0x40);
    display_cmd(0xA4);//SSD1306_DISPLAY_ALL_ON_RESUME
    display_cmd(0xA6);//SSD1306_NORMAL_DISPLAY
		
    display_cmd(0xAF);//SSD1306_DISPLAY_ON
     
		display_cmd(0x21); //set coloumn address
    display_cmd(0);    //
    display_cmd(127);
    display_cmd(0x22); //setting row address
    display_cmd(0);
    display_cmd(7);
		
		int j=0;
		for (j=0; j<1024; j++)      // Write Zeros to clear the display
    {
        display_data(0);
    }
		display_cmd(0x21); 
    display_cmd(24);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(4);
    display_cmd(7);
}

/*---------------------------RTC setup------------------------------------*/

void rtc_init(void)
{
RCC->APB1ENR |=RCC_APB1ENR_PWREN; //ENABLE CLOCK 
//RCC->CSR |=RCC_CSR_LSION;
PWR->CR|=PWR_CR_DBP; //ENABLE WRITE ACCESS TO RTC REGISTERS

	
RCC->BDCR |=RCC_BDCR_BDRST; //BACK UP DOMAIN RESET
RCC->BDCR &=~RCC_BDCR_BDRST;		
	
RCC->BDCR |=RCC_BDCR_RTCSEL_HSE; //select clock source as HSE

RCC->CR |=RCC_CR_HSEON;   //switch ON HSE

	
while(!(RCC->CR & RCC_CR_HSERDY)); //Check if HSE ready or not

/*---------------------------------------*/	
	
RCC->BDCR|=RCC_BDCR_RTCEN;    //Enable rtc peripheral
	


RTC->WPR=0xCA;           //write lock key to wpr
RTC->WPR=0x53;

RTC->ISR |=RTC_ISR_INIT;      //enable interrupts
	


while(!(RTC->ISR & RTC_ISR_INITF));   
                       
RTC->PRER =0x00000F41; //	DIV_S value 249 in hex F9 for LSI 32 khz,FOR HSE 3905 IS F41

RTC->PRER|=0x007f0000; //prer DIV_A value 127 in hex 7f
	
RTC->TR=0x00163000; //time is set as 23:58:00 (hr:min:sec) 
	
RTC->DR=0x00182914; //date is set as 2018/09/10 yyyy/mm/dd
	
RTC->CR &=~RTC_CR_FMT; //set rtc in 24hr format

RTC->ISR &=~ RTC_ISR_INIT; //EXIT INITIALIZATION MODE
RTC->WPR =0XFF;  //ENABLE WRITE PROTECTION

PWR->CR &=~PWR_CR_DBP; 
}

void hex2char(int num,unsigned char *str)
{
	
	int temp;
	int i=0;
	for(i=5;i>=0;i--)
	{
		temp=num&0x00000f; // AND operation with 00000f will make all digits zero except first digit
		str[i]=temp+48;    //adding 48 to hexadecimal ascii value will make it respective ascii character
		num=num>>4;   // right shifting to get next character
		
	}
	str[6]='\0';
	return;
	
  
}

void delay(void)
{
  volatile unsigned int i;

  for (i = 0; i < 800000; i++)
    ;
}
void display_date(unsigned int x)
{
	unsigned char val;
	  display_cmd(0x21); //this statements register pointer to initial position after display of time
    display_cmd(5);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(1);
    display_cmd(7);

val =(x>>4)&0x0f;          //extract date Tens digit and print
	display_digit(val);
	//display_buff(val+48);
val=x&0x0f;                //extract date unit digit and print
	
	display_digit(val);
	//display_buff(val+48);
	
  display_digit(11);
	//display_buff('/');
	
val=(x>>12)&(0x01);	 //etract month digits and print
	
	 display_digit(val);
	//display_buff(val+48);
val=(x>>8)&(0x0f);
	 display_digit(val);
	//display_buff(val+48);

/*	
val=(x>>20)&(0x0f);	
	display_buff(val+48);
val=(x>>16)&(0x0f);
	display_buff(val+48);  */

	
}

void display_letter(unsigned int ch)
{
	int i=0;
	
		for (i=0; i<9; i++)    
    {
			
      display_data(letters[ch][i]);
		
    }
}
void display_week(unsigned int x )
{

unsigned char val;
	  display_cmd(0x21); //this statements register pointer to initial position after display of time
    display_cmd(50);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(1);
    display_cmd(7);

	
	val=(x>>13)&(0x0f);
	
	if(val==1)
	{
	  display_letter(12);
		display_letter(14);
		display_letter(13);
	}
	if(val == 2)
	{
	  display_letter(19);
		display_letter(20);
		display_letter(4);
	}
	if(val == 3)
	{
	  display_letter(22);
		display_letter(4);
		display_letter(3);
	}
	if(val == 4)
	{
	  display_letter(19);
		display_letter(7);
		display_letter(20);
	}
	if(val == 5)
	{
	 display_letter(5);
		display_letter(17);
		display_letter(8);
	}
	if(val == 6)
	{
	  display_letter(18);
		display_letter(0);
		display_letter(19);
	}
	if(val == 7)
	{
	 display_letter(18);
		display_letter(20);
		display_letter(13);
	}
	
	  display_cmd(0x21); //this statements register pointer to initial position after display of time
    display_cmd(24);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(4);
    display_cmd(7); 

}

void displayTime(unsigned char *ptr)
 {
	 int i,j,k=0;
	 unsigned char str[8];
	 for(i=0;i<6;i++)
	 {
	 
		 if((i!=0)&&((i%2)==0))
			{
				
				str[k]=':';
				k++;
			}	
	    str[k]=ptr[i];
	    k++;
	 }
	 
	for(j=0;j<=7;j++)
   {	 
	
		display_cmd(0x21);
    display_cmd(20+j*10);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(4);
    display_cmd(7);
		 
	 switch(str[j])
	 {
	   case'0':
		 {
		 for(i=0;i<=10;i++)
			 {
			 
			 display_data(bufferNum[0][i]);
			}
     
		display_cmd(0x21);
    display_cmd(21+j*10);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(5);
    display_cmd(7);
		
		
		for (i=11; i<=20; i++)      // display data byte by byte
    {
      display_data(bufferNum[0][i]);
    }
		
		 }
		 break;
	  case'1':
		 {
		 for(i=0;i<=10;i++)
			 {
			 
			 display_data(bufferNum[1][i]);
			}
     
		display_cmd(0x21);
    display_cmd(21+j*10);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(5);
    display_cmd(7);
		
		
		for (i=11; i<=20; i++)      // display data byte by byte
    {
      display_data(bufferNum[1][i]);
    }
		 }
		 break;
		 case'2':
		 {
		 for(i=0;i<=10;i++)
			 {
			 
			 display_data(bufferNum[2][i]);
			}
     
		display_cmd(0x21);
    display_cmd(21+j*10);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(5);
    display_cmd(7);
		
		
		for (i=11; i<=20; i++)      // display data byte by byte
    {
      display_data(bufferNum[2][i]);
    }
		 }
		 break;
		 case'3':
		 {
		 for(i=0;i<=10;i++)
			 {
			 
			 display_data(bufferNum[3][i]);
			}
     
		display_cmd(0x21);
    display_cmd(21+j*10);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(5);
    display_cmd(7);
		
		
		for (i=11; i<=20; i++)      // display data byte by byte
    {
      display_data(bufferNum[3][i]);
    }
		 }
		 break;
		 case'4':
		 {
		 for(i=0;i<=10;i++)
			 {
			 
			 display_data(bufferNum[4][i]);
			}
     
		display_cmd(0x21);
    display_cmd(21+j*10);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(5);
    display_cmd(7);
		
		
		for (i=11; i<=20; i++)      // display data byte by byte
    {
      display_data(bufferNum[4][i]);
    }
		 }
		 break;
		 case'5':
		 {
		 for(i=0;i<=10;i++)
			 {
			 
			 display_data(bufferNum[5][i]);
			}
     
		display_cmd(0x21);
    display_cmd(21+j*10);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(5);
    display_cmd(7);
		
		
		for (i=11; i<=20; i++)      // display data byte by byte
    {
      display_data(bufferNum[5][i]);
    }
		 }
		 break;
		 case'6':
		 {
		 for(i=0;i<=10;i++)
			 {
			 
			 display_data(bufferNum[6][i]);
			}
     
		display_cmd(0x21);
    display_cmd(21+j*10);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(5);
    display_cmd(7);
		
		
		for (i=11; i<=20; i++)      // display data byte by byte
    {
      display_data(bufferNum[6][i]);
    }
		 }
		 break;
		 case'7':
		 {
		 for(i=0;i<=10;i++)
			 {
			 
			 display_data(bufferNum[7][i]);
			}
     
		display_cmd(0x21);
    display_cmd(21+j*10);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(5);
    display_cmd(7);
		
		
		for (i=11; i<=20; i++)      // display data byte by byte
    {
      display_data(bufferNum[7][i]);
    }
		 }
		 break;
		 
		 case'8':
		 {
		 for(i=0;i<=10;i++)
			 {
			 
			 display_data(bufferNum[8][i]);
			}
     
		display_cmd(0x21);
    display_cmd(21+j*10);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(5);
    display_cmd(7);
		
		
		for (i=11; i<=20; i++)      // display data byte by byte
    {
      display_data(bufferNum[8][i]);
    }
		 }
		 break;
		 
		 case'9':
		 {
		 for(i=0;i<=10;i++)
			 {
			 
			 display_data(bufferNum[9][i]);
			}
     
		display_cmd(0x21);
    display_cmd(21+j*10);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(5);
    display_cmd(7);
		
		
		for (i=11; i<=20; i++)      // display data byte by byte
    {
      display_data(bufferNum[9][i]);
    }
		 }
		 break;
		 
		 case':':
		 {
		 for(i=0;i<=10;i++)
			 {
			 
			 display_data(bufferNum[10][i]);
			}
     
		display_cmd(0x21);
    display_cmd(21+j*10);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(5);
    display_cmd(7);
		
		
		for (i=11; i<=20; i++)      // display data byte by byte
    {
      display_data(bufferNum[10][i]);
    }
		 }
		 break;
	   default:
		 {
		 for(i=0;i<=10;i++)
			 {
			 
			 display_data(bufferNum[j][i]);
			}
     
		display_cmd(0x21);
    display_cmd(21+j*10);
    display_cmd(127);

    display_cmd(0x22);
    display_cmd(5);
    display_cmd(7);
		
		
		for (i=11; i<=20; i++)      // display data byte by byte
    {
      display_data(bufferNum[j][i]);
    }
		 
		 }
	 
	 }
 }
 }
 
 void display_digit(unsigned int ch)
{
	int i=0;
	
		for (i=0; i<8; i++)    
    {
			
      display_data(buffer[ch][i]);
    }
}
void set_time(void)
{
	
unsigned int trv=0x0000;
unsigned int drv=0x0000;
unsigned int tmp;


	tmp=hr/10;
	trv |=tmp<<20;
	
	tmp=hr%10;
	trv |=tmp<<16;
	
	
	tmp=min/10;
	trv |= tmp <<12;
	
	tmp=min%10;
	trv |=tmp<<8;
	
	tmp=yr/10;
	drv |=tmp<<20;
	
	tmp=yr%10;
	drv |=tmp<<16;
	tmp=wk&0xf;
	drv|=tmp<<13;
	
	tmp=mnt/10;
	drv |=tmp<<12;
	
	tmp=mnt%10;
	drv |=tmp<<8;
	
	tmp=dt/10;
	drv |=tmp<<4;
	
	tmp=dt%10;
	drv |=tmp;
	
	
RCC->APB1ENR |=RCC_APB1ENR_PWREN; //ENABLE CLOCK 
PWR->CR|=PWR_CR_DBP; //ENABLE WRITE ACCESS TO RTC REGISTERS

RTC->WPR=0xCA;           //write lock key to wpr
RTC->WPR=0x53;
RTC->ISR |=RTC_ISR_INIT;      //enable interrupts
while(!(RTC->ISR & RTC_ISR_INITF));   
                       
RTC->TR=trv; //time is set as 23:58:00 (hr:min:sec) 
RTC->DR=drv; //date is set as 2018/09/10 yyyy/mm/dd
	
	
RTC->CR &=~RTC_CR_FMT; //set rtc in 24hr format
RTC->ISR &=~ RTC_ISR_INIT; //EXIT INITIALIZATION MODE
RTC->WPR =0XFF;  //ENABLE WRITE PROTECTION
PWR->CR &=~PWR_CR_DBP; 



}