#include"C8051F020.h"
int floor = 0;


int ir1F=0; //IR sensor 1 flag
int ir2F=0; //IR sensor 2 flag
int chk1=0; //IR sensor 1 additional check flag variable to detect high to low edge
int chk2=0; //IR sensor 2 additional check flag variable to detect high to low edge
int persons=0; //number of persons inside the elevator
int moving = 0;


unsigned int UP_req[5]= {0,0,0,0,0};
unsigned int level_req[5]= {0,0,0,0,0};
unsigned int down_req[5]= {0,0,0,0,0};
int i=0;


void timer0_init(void)
{
    CKCON &= 0xF0; //T0M = 0; Timer 0 uses SysClock/12 
    TMOD = (TMOD & 0xF0) | 0x01;  /* Set T/C0 Mode mode1*/       
	TH0 = 0xFFFF;
	TL0 = 0x63BF ; //240ms
    ET0 = 1;                      /* Enable Timer 0 Interrupts*/

}

sbit buzzer = P0^1;
sbit ir1 = P0^3;
sbit ir2 = P0^4;


sbit red_led = P1^7;
sbit green_led = P0^0;

// inside_buutons 
sbit floor0 = P2^0;
sbit floor1 = P2^1;
sbit floor2 = P2^2;
sbit floor3 = P2^3;
sbit floor4 = P2^4;
sbit open_doorbtn = P3^7;



sbit motor1 = P3^4 ; 
sbit motor2 = P3^5 ; 

sbit floor1Down = P2^5;
sbit floor2Down = P2^6;
sbit floor3Down = P2^7;





void delay(void)  // Function for creating a very short delay
{
    int i;
    for(i=0;i<200;i++);
}

// void delay_door()
// {
//     int i = 0;
//     int j = 0;
//     for (i=0;i<500;++i){
//         for(j=0;j<500;++j){
//         }
//     }
// }

void count_people(void)
{
	if(ir1==0)
	{
		// open_door();
		if(ir2F==1)
		{
			chk1=1;
			delay();
		}
		else	ir1F = 1;
	}
	else
	{
		if(chk1)
		{
			persons = persons-1;
			chk1=0;
			ir2F=0;
		}
	}
	
	if(ir2==0)
	{
		if(ir1F==1)
		{
			chk2=1;
			delay();
		}
		else	ir2F = 1;
	}
	else
	{
		if(chk2)
		{
			persons = persons+1;
			chk2=0;
			ir1F=0;
		}
	}

    // error handle
    if(persons<0) persons = 0;

	if(persons>4)
	{
		buzzer = 1;
		// open_door();
	}
	else	buzzer = 0;

}

void open_door()
{
    red_led = 1;
    green_led = 0;
	// delay_door();
    //int i = 0;
    for(i = 0; i < 500; i++)
        count_people();
    if(buzzer == 1) {
        open_door();
    }
    red_led =0;
    green_led=1;

}

void motor_delay()
{
    int i = 0;
    int j = 0;
    for (i=0;i<1000;++i){
        for(j=0;j<100;++j){
        }
    }
}


void motor_delay1()
{
    int i = 0;
    int j = 0;
    for (i=0;i<1000;++i){
        for(j=0;j<90;++j){
        }
    }
}
void ascend()
{
    motor1 = 1 ;
    motor2 = 0;
    motor_delay();
    motor1 = 0 ;
    motor2=1;
    motor_delay1();
    motor2=0;
}

void descend()
{
    motor1 = 0 ; 
    motor2 = 1 ;
    motor_delay();
    motor2 = 0 ;
    motor1=1 ;
    motor_delay1();
    motor1=0;
}

void requests(void){
    //inside _buttons
    if(!(floor0)){
        level_req[0]=1;
       
    }
     if(!(floor1)){
        level_req[1]=1;
        
    }
     if(!(floor2)){
        level_req[2]=1;
       
    }
     if(!(floor3)){
        level_req[3]=1;
    }
    if(!(floor4)){
        level_req[4]=1; 
    }

    if(!(floor1Down)){
        down_req[1]=1;
        
    }
     if(!(floor2Down)){
        down_req[2]=1;
       
    }
     if(!(floor3Down)){
        down_req[3]=1;
    }
    
    if(!(P3&0x40)){  //Down_req of zero floor 
        down_req[0]=1;
    }

    if(!(P3&0x01))  {  //001 "0"
        UP_req[1]=1;
       
    } 
    if(!(P3&0x02))  {  //010 "1"
        UP_req[2]=1;
        
    } 
    if(!(P3&0x04))  {  //100 "2"
        UP_req[3]=1;
        
    } 
    if(!(P3&0x08))  {  //1000 "3" // Up_req for 4th floor 
        UP_req[4]=1;
       
    } 
	if(!(open_doorbtn) && !moving){
	 open_door();
	}

    
}


//executed every 65536 count
void timer0_ISR() interrupt 1 {
	TF0 = 0 ; 
    requests();
}



void seg_dispaly(floor){
    if(floor==0)        P1 = 0x3F;
    else if(floor==1)   P1 = 0x06;
    else if(floor==2)   P1 = 0x5B;
    else if(floor==3)   P1 = 0x4F;
    else if(floor==4)   P1 = 0x66;
}


void main(void){
    
 // diable the watch dog
    WDTCN = 0x0DE;
    WDTCN = 0x0AD;

    OSCICN = 0x014; // 2MH clock

    // config cross bar
    XBR0 = 0x00;
    XBR1 = 0x00;
    XBR2 = 0x040; // Cross bar enabled , weak Pull-up enabled


    P0MDOUT |= 0x08; 
    P1MDOUT |= 0x7F;
    P2MDOUT |= 0x00;


    timer0_init();
    EA=1;
    TR0 = 1;    /* Start Timer 0 Running */

    buzzer = 0;

    requests();


    while(1)
    {
        seg_dispaly(floor);   
        
        i=0;
        for(i=0;i<5;i++){
            if((  level_req[i] || UP_req[i] || down_req[i])&& i>floor  ) 
            {
                while(floor != i )
                {
                    moving = 1;
                    ascend();
                    floor++;
                    seg_dispaly(floor);
                    if(  level_req[floor] || UP_req[floor]|| down_req[floor] )
                    { 
                        level_req[floor]=0;
                        UP_req[floor]=0;
                        down_req[floor]=0;
						open_door(); 
                        moving = 0;
                    }
                }
            }
        }

        i=4;
        for( i=4;i>=0;i--){
            if(( level_req[i] || down_req[i] || UP_req[i]) &&  i< floor ){
                while(floor != i )
                {
                    moving = 1;
                    descend();
                    floor--;
                    seg_dispaly(floor);
                    if( level_req[floor]  || down_req[floor]||UP_req[floor])
                    { 

                       level_req[floor]=0;
                       down_req[floor]=0; 
                       UP_req[floor]=0;
					   open_door();
                       moving = 0;
                    }
                }
        }
        }
    }

}