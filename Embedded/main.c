#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <devices.h>
#include <graphics.h>
#include <agui.h>
#include <pointer.h>
#include <touchscreen.h>
#include <unistd.h>
#include <swplatform.h>

adc084s021_t    *adc;
ioport_t        *ioport;
sdhc_t * sdcard;
graphics_t *graphics;
canvas_t *canvas;
agui_t *agui;

extern form_t form;
extern void form1_btn1_action(obj_t *obj, const action_event_t *action);
extern void form1_btn2_action(obj_t *obj, const action_event_t *action);
extern void form1_btn3_action(obj_t *obj, const action_event_t *action);
extern void form1_slr1_action( obj_t *obj, const action_event_t *action);


extern button_t         form1_btn1;
extern button_t         form1_btn2;
extern button_t         form1_btn3;
extern slider_t         form1_slr1;
extern obj_t           *form_children[];

void *update(void *arg);
void *handler(void *arg);
void *action(void *arg);

void init();
void mount_SD();
void unmount_SD();
void measure();

void add(float *A,float *B, int r , int c, float *res);
void substract(float *A,float *B, int r , int c, float *res);
void multiply(float *A,float *B, int r1, int c1, int r2, int c2, float *res);
void transpose(float *A,int r, int c, float *res);
void estimate();
float OCV(float x);
float dOCV(float x);


float R0[2][11] = {{0.112318,0.110695,0.107181,0.103883,0.103883,0.105506,0.107289,0.097865,0.093830,0.105511,0.117297},  //Charging
                   {0.118152,0.116176,0.116176,0.110702,0.114272,0.112429,0.105512,0.107239,0.105615,0.105512,0.099014}}; //Discharging

float R1[2][11] = {{0.016210,0.014520,0.017820,0.018450,0.038900,0.041950,0.017860,0.045990,0.019430,0.009200,0.010570},
                   {0.023490,0.009806,0.014190,0.010840,0.006233,0.003506,0.005641,0.004639,0.006164,0.006330,0.017080}};

float R2[2][11] = {{0.020150,0.028990,0.018930,0.026010,0.020740,0.044160,0.021120,0.046310,0.017450,0.013150,0.046750},
                   {0.002328,0.001417,0.001982,0.005174,0.003128,0.003371,0.003923,0.001184,0.001885,0.001418,0.004515}};
                   
float C1[2][11] = {{937,6873,1565,875,897,792,1260,1469,1722,780,736},
                   {448,377,341,389,320,839,514,854,503,435,417}};
                   
float C2[2][11] = {{750,671,1467,3236,2190,616,1051,1456,2005,546,680},
                   {2307,1430,1886,773,677,932,754,2098,2638,1766,965}};

float X[3][1] = {{0.2},{0.1},{0.1}};
float p[3][3] = {{0.3,0,0},{0,0.2,0},{0,0,0.2}};

float SigmaW[3][3] = {{0.00,0,0},{0,0.1,0},{0,0,0.1}};
float SigmaV[1][1] = {{0.6443}};

float A[3][3] = {{1,0,0},{0,0,0},{0,0,0}};
float B[3][1] = {{0},{0},{0}};
float C[1][3] = {{0,-1,-1}};
float U[1][1] = {{0}};
int m = 0,n = 0;
float y,r[1][1];
float k[3][1] = {{0},{0},{0}};
float temp1[3][3],temp2[3][3],temp3[3][3];

float Eta = 1,Q = 3600*100,del_t,prev_t=0;
int I = 0,set_value;



int handle;

float Vt,Iout,prev_I =0;
float duty1 = 0,duty2 = 0;

void main( void )
{
    char str[100];
    init();
   /* mount_SD();

    IOPORT_BASE8(DUTY1_BASE_ADDRESS)[0] = (uint8_t)duty1;
    IOPORT_BASE8(DUTY2_BASE_ADDRESS)[0] = (uint8_t)duty2;
    prev_t = clock()/(1.0*CLOCKS_PER_SEC);

    float error,error_int1 = 0,control_output;
    float Kpc = 0.18,Kic = 0.0008,Kdc = 0.006;
    float Kpd = 1,Kid = 0.01,Kdd = 0.005;      */


    while(1){
         measure();
         agui_service(agui);
         sprintf(str, "%d",I);
         graphics_draw_string(canvas,140,60,str, &bitstreamverasans10, WHITE, FS_NONE);

         graphics_draw_string(canvas,10,140,"Desired Current :", &bitstreamverasans10, WHITE, FS_NONE);
         graphics_draw_string(canvas,10,160,"Terminal Voltage:", &bitstreamverasans10, WHITE, FS_NONE);
         graphics_draw_string(canvas,10,180,"Actual Current  :", &bitstreamverasans10, WHITE, FS_NONE);
         graphics_draw_string(canvas,10,200,"State of Charge :", &bitstreamverasans10, WHITE, FS_NONE);


         sprintf(str, "%d",set_value);
         graphics_draw_string(canvas,130,140,str, &bitstreamverasans10, WHITE, FS_NONE);

    }

    /*while(Vt < 15.6){
        measure();
        
        error = 4 - Iout;
        error_int1 = error_int1 + error*del_t;
        control_output = Kpc * error + Kic * error_int1 + Kdc * (Iout - prev_I)/del_t;
        if(duty1+control_output > 100)
               duty1 = 100;
        else if(duty1+control_output < 0)
               duty1 = 0;
        else
               duty1 = duty1 + control_output;

        ioport_set_value(ioport,DUTY1,(uint8_t)duty1);
    }

    error_int1 = 0;
    while(Iout > 7){
        measure();
        
        error = 15.6 - Vt;
        error_int1= error_int1 + error*del_t;
        control_output = Kpc * error + Kic * error_int1;
        if(duty1+control_output > 100)
            duty1 = 100;
        else if(duty1+control_output < 0)
            duty1 = 0;
        else
            duty1 = duty1 + control_output;
        ioport_set_value(ioport,DUTY1,(uint8_t)duty1);
    }

    clock_t t = clock() + CLOCKS_PER_SEC*120;
    duty1 = 0;
    ioport_set_value(ioport,DUTY1,(uint8_t)duty1);
    while(clock() < t){
        measure();
    }

    error_int1 = 0;
    while(Iout > 0.5){
        measure();
        
        error = 13.8 - Vt;
        error_int1= error_int1 + error*del_t;
        control_output = Kpc * error + Kic * error_int1;
        if(duty1+control_output > 100)
            duty1 = 100;
        else if(duty1+control_output < 0)
            duty1 = 0;
        else
            duty1 = duty1 + control_output;
        ioport_set_value(ioport,DUTY1,(uint8_t)duty1);
    }
              */
    //unmount_SD();
}

void measure(){
     float V0,V1,V2,V3;

     V0 = adc084s021_read( adc, 1)*3.3/255.0;
     V1 = adc084s021_read( adc, 2)*3.3/255.0;  //Voltage Sensor
     V2 = adc084s021_read( adc, 3)*3.3/255.0;  //Current Sensor
     V3 = adc084s021_read( adc, 0)*3.3/255.0;  //Offset

     Vt = V1*5;
     prev_I = Iout;
     Iout = (V2*1.515-V3)*15.151 + 0.105;

     char str[30];
     graphics_fill_rect(canvas,130,160,40,80,LIGHTSTEELBLUE);
     sprintf(str, "%.2f",Vt);
     graphics_draw_string(canvas,130,160,str, &bitstreamverasans10, WHITE, FS_NONE);
     sprintf(str, "%.2f",Iout);
     graphics_draw_string(canvas,130,180,str, &bitstreamverasans10, WHITE, FS_NONE);

     float t = clock()/(1.0*CLOCKS_PER_SEC);
     del_t = t - prev_t;
     prev_t = t;
     estimate();
     float soc = X[0][0]*100;
     float soc_err = p[0][0] * 100;

     sprintf(str, "%.2f",soc);
     graphics_draw_string(canvas,130,200,str, &bitstreamverasans10, WHITE, FS_NONE);
     /*sprintf(str, "%.2f",soc_err);
     graphics_draw_string(canvas,130,220,str, &bitstreamverasans10, WHITE, FS_NONE);   */

     //printf("%.2f, %.3f, %.1f, %.1f,SOC=%.2f,%.2f\n",Vt,Iout,duty1,duty2,soc,soc_err);
     /*
     char asd[50];
     sprintf(asd,"%.4f, %.4f, %.4f, %.2f, %.2f,\r\n                       ",Vt,Iout,t,soc,soc_err);
     write( handle,asd, 50 );
     */
}

void estimate(){
    if(fabs(Iout) < 0.2)
        Iout = 0;
    
    if(Iout > 0)
        m = 0;
    else if(Iout < 0)
        m = 1;


    int num = X[0][0]*10;
    n = num < 0 ? num - 0.5 : num + 0.5;

    A[1][1] = exp(-del_t/(R1[m][n]*C1[m][n]));
    A[2][2] = exp(-del_t/(R1[m][n]*C1[m][n]));
    B[0][0] = Eta*del_t/Q;
    B[1][0] = -R1[m][n]*(1-exp(-del_t/(R1[m][n]*C1[m][n])));
    B[2][0] = -R2[m][n]*(1-exp(-del_t/(R2[m][n]*C2[m][n])));
    
    U[0][0] = Iout;

    // EKF for State Estimation STARTS
    multiply(A,X,3,3,3,1,temp1);
    multiply(B,U,3,1,1,1,temp2);
    add(temp1,temp2,3,1,X);

    multiply(A,p,3,3,3,3,temp1);
    transpose(A,3,3,temp2);
    multiply(temp1,temp2,3,3,3,3,temp3);
    add(temp3,SigmaW,3,3,p);

    num = X[0][0]*10;
    n = num < 0 ? num - 0.5 : num + 0.5;

    C[0][0] = dOCV(X[0][0]) ;
    y = OCV(X[0][0]) - X[1][0] -X[2][0] + R0[m][n]*Iout;

    transpose(C,1,3,temp1);
    multiply(p,temp1,3,3,3,1,temp2);
    multiply(C,temp2,1,3,3,1,temp3);
    add(temp3,SigmaV,1,1,temp1);
    temp3[0][0] = temp1[0][0];
    multiply(temp2,temp3,3,1,1,1,k);

    r[0][0] = Vt - y;
    multiply(k,r,3,1,1,1,temp1);
    add(X,temp1,3,1,X);
    if(X[0][0]>1.04)
        X[0][0]=1.04;
    else if(X[0][0]<-0.04)
        X[0][0]=-0.04;

    multiply(k,C,3,1,1,3,temp2);
    multiply(temp2,p,3,3,3,3,temp3);
    substract(p,temp3,3,3,p);

}

void mount_SD(){
    int mountflags = 0;
    int err;

    puts( "Please insert sd card..." );
    while( !sdhc_card_detect( sdcard ) );   // Wait for card to be inserted

    for ( int i = 0; i < 1; i++ )
    {
        puts( "Initializing..." );
        err = sdhc_card_init( sdcard, SDHC_INIT_POWERON );
        if ( sdhc_is_memcard(err) ) break;
    }

    /* Try to mount partition #1 */
    err = mount( "/dev/BLOCKIO_1" ,"/sdcard", "fatfs", 1, mountflags);
    if (err != 0)
    {
        /* ... and if that fails try the entire disk */
        err = mount("/dev/BLOCKIO_1", "/sdcard", "fatfs", 0, mountflags );
    }
    if ( err != 0 )
    {
        puts( "Mount failed, please remove card!" );
        while( sdhc_card_detect( sdcard ) );
        puts( "Card removed" );
        mount_SD();
    }else{
        puts( "File system mounted" );
        handle = open( "/sdcard/data.txt", O_RDWR|O_CREAT ) ;
    }

}

void unmount_SD(){
     close(handle);
     if ( sdhc_card_removed( sdcard ) )
     {
         unmount( "/sdcard", MOUNT_FLAG_FORCE );
         puts( "Forced unmount" );
     }
     else
     {
         umount( "/sdcard" );
         puts( "File system unmounted, please remove card" );
     }
     while( sdhc_card_detect( sdcard ) );
     puts( "Card removed" );
}

void init(){

    adc = adc084s021_open( DRV_ADC084S021_1 );
    ioport = ioport_open( DRV_IOPORT_1 );
    sdcard = sdhc_open( DRV_SDHC_1 ) ;

    agui_show_form(AGUI_HANDLE(form));
    cursor_show(agui);
    agui = agui_open(AGUI_1);

    graphics = graphics_open(GRAPHICS_1);
    canvas = graphics_get_visible_canvas(graphics);
    graphics_fill_canvas(canvas, BLACK);
}

void multiply(float *A,float *B, int r1, int c1, int r2, int c2, float *res){
     int i,j,k;
     for(i = 0; i < r1; i++)
     {
           for(j = 0; j < c2; j++)
           {
                 *((res+i*c2) + j) = 0;
                 for(k = 0; k < c1; k++)
                 {
                       *((res+i*c2) + j) += *((A+i*c1) + k)**((B+k*c2) + j);
                 }
           }
     }

}

void transpose(float *A, int r, int c, float *res){
     int i,j;
     for(i = 0; i < c; ++i)
     {
           for(j = 0; j < r; j++)
           {
                 *((res+i*r) + j) = 0;
           }
     }

     for(i = 0; i < c; i++)
     {
           for(j = 0; j < r; j++)
           {

                  *((res+i*r) + j) = *((A+j*c) + i);

           }
     }

}

void add(float *A,float *B, int r , int c, float *res){
     int i,j;
     for(i = 0; i < r; i++)
     {
           for(j = 0; j < c; j++)
           {

                  *((res+i*c) + j) = *((A+i*c) + j) + *((B+i*c) + j);

           }
     }
}

void substract(float *A,float *B, int r , int c, float *res){
     int i,j;
     for(i = 0; i < r; i++)
     {
           for(j = 0; j < c; j++)
           {

                  *((res+i*c) + j) = *((A+i*c) + j) - *((B+i*c) + j);

           }
     }
}

float OCV(float x){
   float Voc = 11.41*pow(x,5) - 24.38*pow(x,4) + 17.85*pow(x,3) - 5.233*pow(x,2) + 0.9281*x + 12.23;
   return Voc;
}

float dOCV(float x){
   float d = 1.41*5*pow(x,4) - 24.38*4*pow(x,3)+ 17.85*3*pow(x,2) - 5.233*2*x + 0.9281;
   return d;
}


button_t form1_btn1 =
{
    .obj.x = 20,
    .obj.y = 40,
    .obj.width =  30,
    .obj.height = 30,
    .obj.draw = button_draw,
    .obj.handler = button_handler,
    .obj.parent = AGUI_HANDLE(form),
    .obj.agui_index = AGUI_1,
    .obj.action = form1_btn1_action,
    .obj.cursor_shape = &cursor_arrow,
    .obj.visible = true,
    .obj.enabled = true,
    .obj.pressed = false,
    .label.x = 0,
    .label.y = 0,
    .label.text = " - ",
    .label.font = &bitstreamverasans8,
    .label.color = BLACK,
    .label.fontstyle = FS_NONE,
    .label.align = ALIGN_CENTRE,
    .relief = RELIEF_LOWERED,
    .color = CADETBLUE
};


button_t form1_btn2 =
{
    .obj.x = 260,
    .obj.y = 40,
    .obj.width =  30,
    .obj.height = 30,
    .obj.draw = button_draw,
    .obj.handler = button_handler,
    .obj.parent = AGUI_HANDLE(form),
    .obj.agui_index = AGUI_1,
    .obj.action = form1_btn2_action,
    .obj.cursor_shape = &cursor_arrow,
    .obj.visible = true,
    .obj.enabled = true,
    .obj.pressed = false,
    .label.x = 0,
    .label.y = 0,
    .label.text = " + ",
    .label.font = &bitstreamverasans8,
    .label.color = BLACK,
    .label.fontstyle = FS_NONE,
    .label.align = ALIGN_CENTRE,
    .relief = RELIEF_LOWERED,
    .color = CADETBLUE
};


button_t form1_btn3 =
{
    .obj.x = 140,
    .obj.y = 90,
    .obj.width =  30,
    .obj.height = 30,
    .obj.draw = button_draw,
    .obj.handler = button_handler,
    .obj.parent = AGUI_HANDLE(form),
    .obj.agui_index = AGUI_1,
    .obj.action = form1_btn3_action,
    .obj.cursor_shape = &cursor_arrow,
    .obj.visible = true,
    .obj.enabled = true,
    .obj.pressed = false,
    .label.x = 0,
    .label.y = 0,
    .label.text = " Enter ",
    .label.font = &bitstreamverasans8,
    .label.color = BLACK,
    .label.fontstyle = FS_NONE,
    .label.align = ALIGN_CENTRE,
    .relief = RELIEF_LOWERED,
    .color = CADETBLUE
};

slider_t form1_slr1 =
{
    .obj.x = 60,
    .obj.y = 40,
    .obj.width = 180,
    .obj.height = 30,
    .obj.draw = slider_draw,
    .obj.handler = slider_handler,
    .obj.parent = AGUI_HANDLE(form),
    .obj.agui_index = AGUI_1,
    .obj.action = form1_slr1_action,
    .obj.cursor_shape = &cursor_arrow,
    .obj.visible = true,
    .obj.enabled = true,
    .low = -15,
    .high = 15,
    .position = 0,
    .handlesize = 15,
    .relief = RELIEF_LOWERED,
    .color1 = CADETBLUE,
    .color2 = CADETBLUE,
    .handlecolor = MAROON
};

obj_t *form_children[] =
{
    AGUI_HANDLE(form1_btn1),
    AGUI_HANDLE(form1_btn2),
    AGUI_HANDLE(form1_slr1),
    AGUI_HANDLE(form1_btn3)
};

form_t form =
{
    .obj.x = 0,
    .obj.y = 0,
    .obj.width = 320,
    .obj.height = 240,
    .obj.draw = form_draw,
    .obj.handler = form_handler,
    .obj.parent = NULL,
    .obj.agui_index = AGUI_1,
    .obj.cursor_shape = &cursor_crosshair,
    .obj.visible = true,
    .obj.enabled = true,
    .caption.x = 0,
    .caption.y = 0,
    .caption.text = "Battery Monitor",
    .caption.font = &bitstreamverasans8,
    .caption.color = BLACK,
    .caption.fontstyle = FS_BOLD_ITALIC,
    .caption.align = ALIGN_CENTRE,
    .captionbarcolor = SKYBLUE,
    .children = form_children,
    .n_children = sizeof(form_children) / sizeof(form_children[0]),
    .relief = RELIEF_LOWERED,
    .color = LIGHTSTEELBLUE
};

void form1_btn1_action(obj_t *obj, const action_event_t *action)
{
    if (action->button == BUTTON_LEFT && action->event == ACTION_CLICKED)
    {
           if(I > -15)
           I = I-1;
           graphics_fill_rect(canvas,140,60,20,20,LIGHTSTEELBLUE);
           slider_set_position(form_children[2], form1_slr1.position-1);
    }
}

void form1_btn2_action(obj_t *obj, const action_event_t *action)
{
    if (action->button == BUTTON_LEFT && action->event == ACTION_CLICKED)
    {
       //I = 10;
       if(I < 15)
       I = I+1;
       graphics_fill_rect(canvas,140,60,20,20,LIGHTSTEELBLUE);
       slider_set_position(form_children[2], form1_slr1.position+1);
    }
}

void form1_btn3_action(obj_t *obj, const action_event_t *action)
{
    if (action->button == BUTTON_LEFT && action->event == ACTION_CLICKED)
    {
       set_value = form1_slr1.position;
       graphics_fill_rect(canvas,130,140,20,20,LIGHTSTEELBLUE);
    }
}

void form1_slr1_action( obj_t *obj, const action_event_t *action)
{
    if (action->button == BUTTON_LEFT && action->event == ACTION_DRAGGED)
    {
        I = form1_slr1.position;
        graphics_fill_rect(canvas,140,60,20,20,LIGHTSTEELBLUE);
    }
}



