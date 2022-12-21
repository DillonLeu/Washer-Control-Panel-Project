#include "mbed.h" 

#define WAIT_TIME_MS 1000ms 

//Note definitions 

#define NOTE_CS5 554 

#define NOTE_FS5 740 

#define NOTE_F5  698 

#define NOTE_DS5 622 

#define NOTE_AS4 466 

#define NOTE_B4  494 

#define NOTE_GS4 415 

#define NOTE_GS5 831 

#define NOTE_FS6 1480 

#define NOTE_AS6 1865 

#define NOTE_CS7 2217 

#define NOTE_FS7 2960 

#define NOTE_E4  330 

#define NOTE_A4  440 

#define NOTE_C5  523 

#define NOTE_E5  659 

#define NOTE_D5  587 

#define NOTE_G4  392 

 

//Washing machine power on button tune. 

const int poweron_tune[] = {NOTE_FS6,NOTE_AS6,NOTE_CS7,NOTE_AS6,NOTE_CS7,NOTE_FS7}; 

//Washing machine power off button tune. 

const int poweroff_tune[] = {NOTE_FS7,NOTE_CS7,NOTE_AS6,NOTE_CS7,NOTE_AS6,NOTE_FS6}; 

//Wash cycle ended tune. 

const int end_tune[] = {NOTE_CS5,NOTE_FS5,NOTE_F5,NOTE_DS5,NOTE_CS5,NOTE_AS4,NOTE_B4,NOTE_CS5,NOTE_DS5,NOTE_GS4,NOTE_AS4,NOTE_B4,NOTE_AS4,NOTE_CS5,NOTE_CS5,NOTE_FS5,NOTE_F5,NOTE_DS5,NOTE_CS5,NOTE_FS5,NOTE_FS5,NOTE_GS5,NOTE_FS5,NOTE_F5,NOTE_DS5,NOTE_F5,NOTE_FS5}; 

const int insecured_door[] = {NOTE_E4,NOTE_A4,NOTE_C5,NOTE_B4,NOTE_A4,NOTE_E5,NOTE_D5,NOTE_B4,NOTE_A4,NOTE_C5,NOTE_B4,NOTE_G4,NOTE_AS4,NOTE_E4}; 

 

//Power on/power off button. 

DigitalIn button1(PC_10);  

//Start/Pause button. 

DigitalIn button2(PC_11);  

//Restart button. 

DigitalIn button3(PD_2);  

 

//led1 will indicate the status of the washing machine.(ON/OFF status). 

DigitalOut led1(PA_4);  

 

//Choose washing machine mode by turning the pot1. 

AnalogIn pot1(PA_5);  

//Sensor to detect if the door is secured properly. 

AnalogIn FSR(PA_1);  

//Sensor to detect the wash cycle temperature. 

AnalogIn TMP(PC_3); 

 

//Washing status to determine the stage of the wash cycle. 

BusOut leds_bus(PC_0, PC_1, PB_0);  

//Initialise hexdisplay. 

BusOut SegDis(PA_11, PA_12, PB_1, PB_15, PB_14, PB_12, PB_11); 

 

//Initialise buzzer. 

PwmOut buzzer(PA_15); 

//Red indicate above ideal temperature of the washer. 

PwmOut red_LED(PB_3); 

PwmOut blue_LED(PB_4); 

//Blue LED to indicate below ideal temperature of the washer. 

PwmOut green_LED(PB_5); 

 

//Initialise function. 

void doorsensor(); 

void washcycle(); 

void quickwash(); 

void delicatewash(); 

void play_note(int frequency); 

void play_note_slower(int frequency); 

void SegDis_startcycle(); 

void SegDis_endcycle(); 

void SegDis_init(); 

void init_leds(); 

int selectionmode(int mode); 

 

//Establish serial communications between PC and NUCLEO. 

BufferedSerial serial(USBTX, USBRX, 9600); 

//Create a char array called instruct and store the message within it. 

char INSTRUCT[] = "Status Washing Machine: press 'T' or 't' to show the time left for the cycle to end\n"; 

//Create a new array that will continually be erased and updated. 

char *INPUT = new char[1]; 

 

//Initialise Variable. 

int hexDis[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67}; 

int i = 0; 

 

int main(){ 

    while (true){ 

        init_leds(); 

        SegDis_init(); 

        //When button1(PC_10) is pressed once and When led1(PA_4) status is off. 

        if (button1.read() && led1.read() == 0) { 

            //led1(PA_4) will switch on to indicate the washing machine is on. 

            led1.write(1); 

            //Play washing machine turn on tune.  

            for(int i = 0; i < 6; i++){          

                play_note(poweron_tune[i]);   

            } 

            buzzer.pulsewidth_us(0); 

            ThisThread::sleep_for(WAIT_TIME_MS); 

            printf("Hello, Welcome to LG washer.\n"); 

             

            doorsensor(); 

            printf("Rotating Dial Selection Mode: [Quick Wash mode - Turn left] or [Delicate Wash mode - Turn right]\n"); 

            int mode = selectionmode(mode); 

            do {green_LED.write(0); 

                red_LED.write(0.4); 

                if (mode == 0){ 

                    quickwash(); 

                } 

                 

                else { 

                    delicatewash(); 

                } 

                //Play end of a washing machine cycle tune. 

                for(int i = 0; i < 27; i++){          

                    play_note(end_tune[i]);      

                } 

                buzzer.pulsewidth_us(0); 

                SegDis_endcycle(); 

                red_LED.write(0); 

                green_LED.write(0.4); 

                i = 0; 

                while(i<1){ 

 

                    if(serial.readable()){ 

 

                        serial.read(INPUT, sizeof(INPUT)); 

                        if (*INPUT == 'T'||*INPUT == 't'){ 

                            printf("Time to Finish = %d seconds left\n",0); 

                        } 

                    } 

                    //Pressed button3(PD_2) to restart the washing machine without turning it off. 

                    if (button3.read()){ 

                        play_note(NOTE_AS6); 

                        buzzer.pulsewidth_us(0); 

                        SegDis.write(0x00); 

                        //ThisThread::sleep_for(WAIT_TIME_MS); 

                        printf("Rewash mode\n"); 

                        break; 

                    } 

                    //Pressed button1(PC_10) to power off the washing machine. 

                    if (button1.read()){ 

                        printf("Bye, see you soon.\n"); 

                        for (int i = 0; i < 6; i++){ 

                            //Play washing machine power off tune. 

                            play_note(poweroff_tune[i]); 

                            buzzer.pulsewidth_us(0); 

                        } 

                        i = 1; 

                    } 

                } 

            } while(i<1); 

        } 

    } 

} 

 

//Check if the door is secured or not secured and the user will be notify on the display. 

void doorsensor(){ 

    green_LED.write(0.4); 

    //Only break out of the 'while' loop when door is secured. 

    while (true){ 

        if (FSR.read()*100 > 45) { 

            printf("Door secured!\n"); 

            ThisThread::sleep_for(WAIT_TIME_MS); 

            break; 

        }  

        else if (FSR.read()*100 <= 45 && FSR.read()*100 >=10) { 

            printf("Door not secured properly!\n"); 

            for (int i = 0; i <= 13; i++) { 

                //door insecured tune 

                play_note_slower(insecured_door[i]); 

                buzzer.pulsewidth_us(0); 

            } 

        } 

        ThisThread::sleep_for(WAIT_TIME_MS); 

    } 

} 

 

//Choose between quick wash or delicate mode by turning the potentiometer(PA_5). 

int selectionmode(int mode){ 

    int i = 0; 

    int num = 0; 

    while (i<1){ 

        //Pressed button2(PC_11) to start wash cycle. 

        if (button2.read() == 1){ 

            play_note(NOTE_AS6); 

            buzzer.pulsewidth_us(0); 

            if (pot1.read() < 0.5){ 

                printf("Quick Wash Mode Selected.\n"); 

                num = 0; 

            } 

            else { 

                printf("Delicate Mode Selected.\n"); 

                num = 1; 

            } 

            i = 1; 

        } 

    } 

    return num; 

} 

 

 

void quickwash(){ 

    int counter  = 0; 

    //Initialise to count amount of time in seconds throughout the entire wash cycle. 

    serial.write(INSTRUCT, sizeof(INSTRUCT)); 

    for(int val = 0; val<3; val++){ 

        leds_bus = 0b111 >> val; 

        //Show time left for each stages(wash,rinse,spin) of wash cycle. 

        for(int i = 3; i >= 0; i--) { 

            SegDis.write(hexDis[i]);  

            counter = counter + 1; 

            //Press button2(PC_11) to pause the wash cycle. 

            if (button2.read() == 1){ 

                play_note(NOTE_AS6); 

                buzzer.pulsewidth_us(0); 

                red_LED.write(0); 

                green_LED.write(0.4); 

                if (val == 0 || val == 1){ 

                    printf("Draining Water\n");                 

                    ThisThread::sleep_for(WAIT_TIME_MS); 

                } 

                doorsensor(); 

 

                while (true){ 

                    if (button2.read() == 1){ 

                        play_note(NOTE_AS6); 

                        buzzer.pulsewidth_us(0); 

                        red_LED.write(0.4); 

                        green_LED.write(0); 

                        if (val == 0 || val == 1){ 

                        printf("Filling Up Water\n");                 

                        ThisThread::sleep_for(WAIT_TIME_MS); 

                        } 

                        break; 

                    } 

                    if(serial.readable()){ 

                        serial.read(INPUT, sizeof(INPUT)); 

                        if (*INPUT == 'T'||*INPUT == 't'){ 

                            int timer = 12 - counter; 

                            printf("Time to Finish = %d seconds left\n",timer); 

                        } 

                    } 

                } 

            } 

            //User can interact with the serial monitor by pressing 'T or t' to see the amount of time left for the wash cycle end. 

            if(serial.readable()){ 

                serial.read(INPUT, sizeof(INPUT)); 

                if (*INPUT == 'T'||*INPUT == 't'){ 

                    int timer = 12 - counter; 

                    printf("Time to Finish = %d seconds left\n",timer); 

                } 

            } 

            ThisThread::sleep_for(WAIT_TIME_MS); 

       } 

         

        if (val == 0){ 

            printf("Wash Completed\n"); 

            } 

        else if (val == 1){ 

            printf("Rinse Completed\n"); 

            } 

        else if (val == 2){ 

            printf("Spin Completed\n"); 

            led1 = 1; 

            leds_bus = 0; 

        } 

    } 

} 

 

void delicatewash(){ 

    int counter = 0; 

    //Initialise to count amount of time in seconds throughout the entire wash cycle. 

    serial.write(INSTRUCT, sizeof(INSTRUCT)); 

    for(int val = 0; val<3; val++){ 

        leds_bus = 0b111 >> val; 

        //Show time left for each stages(wash,rinse,spin) of wash cycle. 

        for(int i = 4; i >= 0; i--) { 

            SegDis.write(hexDis[i]); 

            int counter = counter + 1; 

            //Press button2(PC_11) to pause the wash cycle. 

            if (button2.read() == 1){ 

                play_note(NOTE_AS6); 

                buzzer.pulsewidth_us(0); 

                red_LED.write(0); 

                green_LED.write(0.4); 

                if (val == 0 || val == 1){ 

                    printf("Draining Water\n");                 

                    ThisThread::sleep_for(WAIT_TIME_MS); 

                } 

                doorsensor(); 

                 

                while (true){ 

                    if (button2.read() == 1){ 

                        play_note(NOTE_AS6); 

                        buzzer.pulsewidth_us(0); 

                        red_LED.write(0.4); 

                        green_LED.write(0); 

                        if (val == 0 || val == 1){ 

                        printf("Filling Up Water\n");                 

                        ThisThread::sleep_for(WAIT_TIME_MS); 

                        } 

                        break; 

                    } 

                    if(serial.readable()){ 

                        serial.read(INPUT, sizeof(INPUT)); 

                        if (*INPUT == 'T'||*INPUT == 't'){ 

                            int timer = 15 - counter; 

                            printf("Time to Finish = %d seconds left\n",timer); 

                        } 

                    } 

                } 

            } 

            //User can interact with the serial monitor by pressing 'T or t' to see the amount of time left for the wash cycle end. 

            if(serial.readable()){ 

                serial.read(INPUT, sizeof(INPUT)); 

                if (*INPUT == 'T'||*INPUT == 't'){ 

                    int timer = 15 - counter; 

                    printf("Time to Finish = %d seconds left\n",timer); 

                } 

            }  

            ThisThread::sleep_for(WAIT_TIME_MS); 

        } 

 

        if (val == 0){ 

            printf("Wash Completed\n"); 

        } 

        else if (val == 1){ 

            printf("Rinse Completed\n"); 

        } 

        else if (val == 2){ 

            printf("Spin Completed\n"); 

            led1 = 1; 

            leds_bus = 0; 

        } 

    } 

} 

 

void play_note(int frequency){ 

    //Set the period of the pwm signal (in us). 

    buzzer.period_us((float) 1000000.0f/ (float) frequency);     

    //Set pulse width of the pwm to 1/2 the period. 

    buzzer.pulsewidth_us(buzzer.read_period_us()/2);             

    //Play sound for 275ms. 

    ThisThread::sleep_for(275ms);                                

} 

 

void play_note_slower(int frequency){ 

    //Set the period of the pwm signal (in us). 

    buzzer.period_us((float) 1000000.0f/ (float) frequency);     

    //Set pulse width of the pwm to 1/2 the period. 

    buzzer.pulsewidth_us(buzzer.read_period_us()/2);             

    //Play sound for 275ms. 

    ThisThread::sleep_for(400ms);     

} 

 

void SegDis_startcycle(){ 

    SegDis.write(0x00); 

    thread_sleep_for(250); 

    SegDis.write(0x01); 

    thread_sleep_for(250); 

    SegDis.write(0x03); 

    thread_sleep_for(250); 

    SegDis.write(0x07); 

    thread_sleep_for(250); 

    SegDis.write(0x0F); 

    thread_sleep_for(250); 

    SegDis.write(0x1F); 

    thread_sleep_for(250); 

    SegDis.write(0x3F); 

    thread_sleep_for(250); 

    SegDis.write(0x7F); 

    thread_sleep_for(250); 

    //Display 'Cycle Begin' 

    SegDis.write(0x39); 

    thread_sleep_for(500); 

    SegDis.write(0x66); 

    thread_sleep_for(500); 

    SegDis.write(0x39); 

    thread_sleep_for(500); 

    SegDis.write(0x38); 

    thread_sleep_for(500); 

    SegDis.write(0x79); 

    thread_sleep_for(500); 

    SegDis.write(0x00); 

    thread_sleep_for(500); 

    SegDis.write(0x7F); 

    thread_sleep_for(500); 

    SegDis.write(0x79); 

    thread_sleep_for(500); 

    SegDis.write(0x3D); 

    thread_sleep_for(500); 

    SegDis.write(0x06); 

    thread_sleep_for(500); 

    SegDis.write(0x37); 

    thread_sleep_for(500); 

} 

 

void SegDis_endcycle(){     

    SegDis.write(0x00); 

    thread_sleep_for(250); 

    //Display 'End of Cycle' 

    SegDis.write(0x79); 

    thread_sleep_for(500); 

    SegDis.write(0x37); 

    thread_sleep_for(500); 

    SegDis.write(0x3F); 

    thread_sleep_for(500); 

    SegDis.write(0x00); 

    thread_sleep_for(500); 

    SegDis.write(0x3F); 

    thread_sleep_for(500); 

    SegDis.write(0x71); 

    thread_sleep_for(500); 

    SegDis.write(0x00); 

    thread_sleep_for(500); 

    SegDis.write(0x39); 

    thread_sleep_for(500); 

    SegDis.write(0x66); 

    thread_sleep_for(500); 

    SegDis.write(0x39); 

    thread_sleep_for(500); 

    SegDis.write(0x38); 

    thread_sleep_for(500); 

    SegDis.write(0x79); 

    thread_sleep_for(500); 

    SegDis.write(0x00); 

    thread_sleep_for(500); 

} 

 

void init_leds(){ 

    //Initialise all leds. 

    leds_bus = 0; 

    red_LED = 0; 

    blue_LED = 0; 

    green_LED = 0; 

    buzzer = 0; 

    led1 = 0; 

} 

 

void SegDis_init(){ 

    //Turn off the display by setting all segments to '0'. 

    SegDis.write(0x00);  

    ThisThread::sleep_for(WAIT_TIME_MS); 

} 

 
