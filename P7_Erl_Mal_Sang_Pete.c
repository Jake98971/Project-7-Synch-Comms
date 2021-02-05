// Project 7   : Synchronous Communication
// Names       : Erl Abayao, Malvin Emin, Peter Luong, Sangye Sherpa
// Date        : Due 11/23/2020
// Course      : ELEC3371-01 Embedded Systems
// Description : This project simulates a home security system. There are 3 zones that can be set and armed and a message will be sent to
//               the master device (or the USART in teh case of using only one device) notifying if a zone is armed or if a zone has been
//               breached. Inputs 'S' or 's' will tell the master to go into arming mode. Inputs 'C' or 'c' will cancel arming mode. Inputs
//               'A' or 'a' will arm the alarms (if at least one zone is armed). After at elast one zone is set and is being armed, a
//               countdown from 10 will be displayed on PORT E LEDs. When the countdown reaches zero, the zones will be armed. Before the
//               countdown reaches zero, PA0 can be pressed to cancel the countdown and cancel arming. Zones 1, 2, and 3's armed status
//               will be displayed on PA4, PA5, and PA6. Breaches can be toggled on the corresponding zones using PD4, PD5, and PD6.
//
// NOTE *      : Needed to do Project -> edit project -> Load Scheme -> STM32F107VC_PLL_25_to_72MHz.cfgsch
//
// ----------------------------------------------------------------------------------------------------------------------------------------

// GLOBAL VARIABLES
unsigned int input;                   // variable to store USART input into
unsigned int i;                       // arbitrary variable for printing custom messages to USART
unsigned int zone_1_set = 0;          // boolean variable to check if zone 1 is set
unsigned int zone_2_set = 0;          // boolean variable to check if zone 2 is set
unsigned int zone_3_set = 0;          // boolean variable to check if zone 3 is set
unsigned int zone_1_armed = 0;        // boolean variable to check if zone 1 is armed
unsigned int zone_2_armed = 0;        // boolean variable to check if zone 2 is armed
unsigned int zone_3_armed = 0;        // boolean variable to check if zone 3 is armed
unsigned int all_zones_set = 0;       // boolean variable to check if all zones are set
unsigned int cancel_arming = 0;       // boolean variable to check if cancel arming is true
unsigned int one_zone_set = 0;        // boolean variable to check if at least one zone is set
unsigned int countdown = 10;          // countdown variable to count down from 10
unsigned int ongoing_countdown = 0;   // boolean variable to check if countdown is ongoing
unsigned int q_start = 0;             // boolean variable to check if statrting question should be asked or not

// ARRAYS FOR CUSTOM MESSAGES

char intro_1[] = "Welcome to Project 7: Synchronous Communication!";                    // 48 chars
char intro_2[] = "This program is a prototype of a home security system.";              // 54 chars

char intro_S[] = "Input 'S' or 's' to go into arming mode.";                            // 40 chars
char intro_A[] = "Input 'A' or 'a' to arm the alarms.";                                 // 35 chars

char case_S_1[] = "Input 'S' or 's' detected. Master will now go into Arming mode.";    // 63 chars
char case_S_2[] = "Which zone would you like to set? (1, 2, or 3)";                     // 46 chars
char case_S_3[] = "Zone ";                                                              // 5 chars
char case_S_4[] = " has been set.";                                                     // 14 chars

char case_C_1[] = "Input 'C' or 'c' to cancel.";                // 27 chars

char all_3_zones_set[] = "All zones have been set.";            // 24 chars
char arming_cancelled[] = "Input 'C' or 'c' detected. Arming has been cancelled";       // 52 chars

char try_again[] = "That wasn't an option. Try again.";         // 33 chars

char case_A_1[] = "At least one zone needs to be set before arming!";   // 48 chars

char start_countdown[] = "Arming zones in 10 seconds... (timer displayed on PORTE)";    // 56 chars
char press_PA0[] = "Press PA0 to cancel countdown and stop arming.";                    // 46 chars
char cancelled[] = "Countdown has been cancelled.";             // 29 chars

char get_input[] = "Input: ";                                   // 7 chars

char zone_1_arm[] = "Zone 1 has been armed.";                   // 22 chars
char zone_2_arm[] = "Zone 2 has been armed.";                   //
char zone_3_arm[] = "Zone 3 has been armed.";                   //

char all_zones_armed[] = "All 3 zones have been armed.";        // 28 chars
char one_zone_armed[] = "At least one zone has been armed.";    // 33 chars

char zone_1_breached[] = "Zone 1 has been breached!";           // 25 chars
char zone_2_breached[] = "Zone 2 has been breached!";           //
char zone_3_breached[] = "Zone 3 has been breached!";           //

// CUSTOM CHARACTER COUNTS FOR CUSTOM MESSAGES
unsigned int count_intro_1 = 48;
unsigned int count_intro_2 = 54;

unsigned int count_intro_S = 40;
unsigned int count_intro_A = 35;

unsigned int count_S_1 = 63;
unsigned int count_S_2 = 46;
unsigned int count_S_3 = 5;
unsigned int count_S_4 = 14;

unsigned int count_C_1 = 27;

unsigned int count_all_3_zones_set = 24;
unsigned int count_arming_cancelled = 52;
unsigned int count_try_again = 33;

unsigned int count_A_1 = 48;

unsigned int count_start_countdown = 56;
unsigned int count_press_PA0 = 46;
unsigned int count_cancelled = 29;

unsigned int count_get_input = 7;

unsigned int count_zone_arm = 22;

unsigned int count_all_zones_armed = 28;
unsigned int count_one_zone_armed = 33;

unsigned int count_zone_breached = 25;

// FUNCTION DECLARATIONS
void InitializeUSART1();              // Sub function declaration which initializes the registers to enable USART1 - Project 4
void Timer2Configuration();           // Sub function declaration which enables the use of timer 2 - Project 5
void ExternalIntConfig();             // Sub function declaration which enables the use of PA0 and PB6 as external interrupts - Project 7
void Print(char message[], unsigned int count_message);    // Sub function declaration which quickens the use of printing custom messages
void Newline();                       // Sub function declaration which prints newline in USART
void zone_1_breach();                 // Sub function declaration for zone 1 breach procedure
void zone_2_breach();                 // Sub function declaration for zone 2 breach procedure
void zone_3_breach();                 // Sub function declaration for zone 3 breach procedure

//INTERRUPT SERVICE ROUTINES
void EXTIPA0 () iv IVT_INT_EXTI0 {
    EXTI_PR.B0 = 1;                   // Clear pending interrupt flag for PA0

    if (ongoing_countdown == 1) {
    
        ongoing_countdown = 0;
    
        Newline();

        Print(cancelled, count_cancelled);      // "Countdown has been cancelled."
        Newline();
        Newline();
        
        GPIOE_ODR = 0x0;                        // reset PORTE LEDs

        TIM2_CR1 = 0x0000;                      // Disable timer
    
    } // end of if
} // end of PA0 ISR

// ----------------------------------------------------------------------------------------------------------------------------------------

// MAIN FUNCTION
void main() {

// INITIALIZATIONS
    InitializeUSART1();         // Call sub function to initialize USART1 - Project 4
    Timer2Configuration();      // Call sub function to enable the use of timer 2 - Project 5
    TIM2_CR1 = 0x0000;          // keep timer disabled until needed
    ExternalIntConfig();        // Call sub function to enable the use of PA4 and PB6 as external interrupts - Project 7
    
    RCC_APB2ENR |= 1 << 5;      // Enable GPIOD clock - necessary to use GPIOD
    RCC_APB2ENR |= 1 << 6;      // Enable GPIOE clock - necessary to use GPIOE
    
    GPIOA_CRL = 0x33333334;     // Configure GPIOA as output for LEDs (PA0 as input) - Project 2

    GPIOD_CRL = 0X44444444;     // Configure GPIOD as input
    
    GPIOE_CRH = 0x33333333;     // Configure GPIOE as output for LEDs - Project 2
    
    GPIOA_ODR = 0X0000;         // Initialize GPIOA LEDs as off - Project 7
    GPIOE_ODR = 0X0000;         // Initialize GPIOE LEDs as off - Project 7
    
    Delay_ms(100);              // ensure all initializations have occured before running main loop

    Print(intro_1, count_intro_1);      // "Welcome to Project 7: Synchronous Communication!"
    Newline();
    Print(intro_2, count_intro_2);      // "This program is a prototype of a home security system."

// MAIN LOOP
    for(;;) {
        
        zone_1_breach();        // check for zone 1 breach
        zone_2_breach();        // check for zone 2 breach
        zone_3_breach();        // check for zone 3 breach
        
        if (q_start == 0) {                     // ask starting question if q_start = 0
        
            q_start = 1;                        // ensure starting question won't loop forever
        
            Newline();
            Newline();

            Print(intro_S, count_intro_S);      // "Input 'S' or 's' to go into arming mode."
            Newline();
            Print(intro_A, count_intro_A);      // "Input 'A' or 'a' to arm the alarms."
            Newline();
            Newline();

            Print(get_input, count_get_input);  // "Input: "
            
        } // end of if
        
        if (((USART1_SR & (1<<5))== 0x20)) {           // get user input - Project 6
            
            input = USART1_DR;                         // store USART user input into varaible 'input'

            switch (input) {
              
              case 'S':
              case 's':

                q_start = 0;                               // ensure starting question will be asked again afterwards

                while(!((USART1_SR & (1<<7)) == 0x80)){}   // reprint user input to USART
                USART1_DR = input;

                Newline();
                Newline();

                Print(case_S_1, count_S_1);                // "Input 'S' or 's' detected. Master will now go into Arming mode."
                Newline();
                Newline();

                for (;;) {    // run until all zones are set or if 'C' or 'c' is inputted

                    if ((zone_1_set && zone_2_set && zone_3_set) == 1) {    // if all zones have been set, break out of this for loop

                        Newline();
                        Print(all_3_zones_set, count_all_3_zones_set);      // "All zones have been set."
                        Newline();

                        all_zones_set = 1;

                        while (!((USART1_SR & (1<<5))== 0x20)){}   // needed to refreash USART input
                        input = USART1_DR;                         // don't know why this works, but it works

                        break;
                    } // end of if

                    if (cancel_arming == 1) {                      // if 'C' or 'c' was input
                    
                        cancel_arming = 0;                         // ensure that this case won't run only once
                    
                        while (!((USART1_SR & (1<<5)) == 0x20)){}  // needed to refreash USART input
                        input = USART1_DR;                         // dont' know why this works, but it works

                        break;                                     // break out of for loop
                    
                    }

                    Print(case_S_2, count_S_2);                // "Which zone would you like to set? (1, 2, or 3)"
                    Newline();
                    Print(case_C_1, count_C_1);                // "Input 'C' or 'c' to cancel."
                    Newline();
                    Print(intro_A, count_intro_A);             // "Input 'A' or 'a' to arm the alarms."
                    Newline();
                    Newline();
                    Print(get_input, count_get_input);         // "Input: "

                    while (!((USART1_SR & (1<<5)) == 0x20)){}  // needed to refresh USART input
                    input = USART1_DR;                         // don't know why this works, but it works

                    while (!((USART1_SR & (1<<5))== 0x20)){}   // Wait for user input - Project 4
                    input = USART1_DR;                         // store USART user input into varaible 'input'

                    switch(input) {

                      case '1':

                        while(!((USART1_SR & (1<<7)) == 0x80)){}   // reprint user input to USART
                        USART1_DR = input;

                        Newline();
                        Newline();

                        GPIOA_ODR = 0x10;                      // PA4 LED on

                        if ((zone_2_set == 1) && (zone_3_set == 1)) { GPIOA_ODR = 0x70; }      // if zones 2 and 3 are also set
                        else if (zone_2_set == 1) { GPIOA_ODR = 0x30; }                        // if zone 2 is also set
                        else if (zone_3_set == 1) { GPIOA_ODR = 0x50; }                        // if zone 3 is also set

                        Print(case_S_3, count_S_3);            // "Zone "

                        while(!((USART1_SR & (1<<7)) == 0x80)){}   // reprint user input to USART
                        USART1_DR = input;

                        Print(case_S_4, count_S_4);            // " has been set."

                        Newline();
                        Newline();

                        zone_1_set = 1;
                        one_zone_set = 1;

                        break;

                      case '2':

                        while(!((USART1_SR & (1<<7)) == 0x80)){}   // reprint user input to USART
                        USART1_DR = input;

                        Newline();
                        Newline();

                        GPIOA_ODR = 0x20;                      // PA5 LED on

                        if ((zone_1_set == 1) && (zone_3_set == 1)) { GPIOA_ODR = 0x70; }      // if zones 1 and 3 are also set
                        else if (zone_1_set == 1) { GPIOA_ODR = 0x30; }                        // if zone 1 is also set
                        else if (zone_3_set == 1) { GPIOA_ODR = 0x60; }                        // if zone 3 is also set

                        Print(case_S_3, count_S_3);            // "Zone "

                        while(!((USART1_SR & (1<<7)) == 0x80)){}   // reprint user input to USART
                        USART1_DR = input;

                        Print(case_S_4, count_S_4);            // " has been set."

                        Newline();
                        Newline();

                        zone_2_set = 1;
                        one_zone_set = 1;

                        break;

                      case '3':

                        while(!((USART1_SR & (1<<7)) == 0x80)){}   // reprint user input to USART
                        USART1_DR = input;

                        Newline();
                        Newline();

                        GPIOA_ODR = 0x40;                      // PA6 LED on

                        if ((zone_1_set == 1) && (zone_2_set == 1)) { GPIOA_ODR = 0x70; }      // if zones 1 and 2 are also set
                        else if (zone_1_set == 1) { GPIOA_ODR = 0x50; }                        // if zone 1 is also set
                        else if (zone_2_set == 1) { GPIOA_ODR = 0x60; }                        // if zone 2 is also set

                        Print(case_S_3, count_S_3);            // "Zone "

                        while(!((USART1_SR & (1<<7)) == 0x80)){}   // reprint user input to USART
                        USART1_DR = input;

                        Print(case_S_4, count_S_4);            // " has been set."

                        Newline();
                        Newline();

                        zone_3_set = 1;
                        one_zone_set = 1;

                        break;

                      case 'C':
                      case 'c':

                        while(!((USART1_SR & (1<<7)) == 0x80)){}   // reprint user input to USART
                        USART1_DR = input;

                        Newline();
                        Newline();

                        Print(arming_cancelled, count_arming_cancelled);    // "Input 'C' or 'c' detected. Arming has been cancelled"

                        cancel_arming = 1;

                        break;

                      case 'A':
                      case 'a':

                        while(!((USART1_SR & (1<<7)) == 0x80)){}   // reprint user input to USART
                        USART1_DR = input;

                        Newline();
                        Newline();

                        if (one_zone_set == 1) {                   // if at least one zone is set, start countdown to arm the alarms

                            Print(start_countdown, count_start_countdown);    // "Arming zones in 10 seconds... (timer displayed on PORTE)"
                            Newline();
                            Print(press_PA0, count_press_PA0);                // "Press PA0 to cancel countdown and stop arming."
                            Newline();

                            TIM2_CR1 = 0x0001;          // enable timer2
                            countdown = 12;             // needs to start at 12 to start at binary 10 on PORTE LEDs for some reason
                            ongoing_countdown = 1;      // set ongoing countdown condition for PA0 ISR
                            
                            for(;;){                    // run until countdown = 0 or until PA0 is pressed

                                if (countdown == 0) {                       // if countdown reaches 0

                                    TIM2_CR1 = 0x0000;                      // Disable timer
                                    
                                    Newline();
                                    Newline();

                                    if (zone_1_set == 1) {                  // if zone 1 was set
                                        zone_1_armed = 1;
                                        Print(zone_1_arm, count_zone_arm);  // "Zone 1 has been armed."
                                        Newline();
                                    }
                                    
                                    if (zone_2_set == 1) {                  // if zone 2 was set
                                       zone_2_armed = 1;
                                       Print(zone_2_arm, count_zone_arm);   // "Zone 2 has been armed."
                                       Newline();
                                    }
                                    
                                    if (zone_3_set == 1) {                  // if zone 3 was set
                                       zone_3_armed = 1;
                                       Print(zone_3_arm, count_zone_arm);   // "Zone 3 has been armed."
                                       Newline();
                                    }

                                    if ((zone_1_armed == 1) && (zone_2_armed == 1) && (zone_3_armed == 1)) {
                                        Newline();
                                        Print(all_zones_armed, count_all_zones_armed);    // "All 3 zones have been armed."
                                        Newline();
                                    }
                                    
                                    Newline();
                                    
                                    break;
                                }

                                if (TIM2_SR.UIF == 1) {                     // If you have multiple timers, can use IF to check status register

                                    TIM2_SR.UIF = 0;                        // Clear this flag which will reset the timer

                                    countdown = countdown - 1;              // decrement PORTE counter
                                    GPIOE_ODR = 0x00FF * countdown;         // decrement PORTE LEDs
                                    
                                } // end of if

                                if (GPIOA_IDR.B0 == 1) { break; }           // if PA0 is pressed, call PA0 ISR and break out of for loop
                                
                            } // end of for loop
                        } // end of if
                        else {

                            while(!((USART1_SR & (1<<7)) == 0x80)){}   // reprint user input to USART
                            USART1_DR = input;

                            Newline();
                            Newline();

                            Print(case_A_1, count_A_1);            // "At least one zone needs to be set before arming!"

                        } // end of else

                        while(!((USART1_SR & (1<<7)) == 0x80)){}   // needed to refresh USART input
                        input = USART1_DR;                         // don't know why this works, but it does

                        break;

                      default:

                        while(!((USART1_SR & (1<<7)) == 0x80)){}   // reprint user input to USART
                        USART1_DR = input;

                        Newline();
                        Newline();

                        Print(try_again, count_try_again);         // "That wasn't an option. Try again."
                        Newline();
                        Newline();

                        break;
                    } // end of switch
                } // end of for loop

                break;

              case 'A':
              case 'a':

                q_start = 0;                               // ensure starting question will be asked again afterwards

                while(!((USART1_SR & (1<<7)) == 0x80)){}   // reprint user input to USART
                USART1_DR = input;

                Newline();
                Newline();

                if ((all_zones_set == 1) || (one_zone_set == 1)) {    // if all zones or ar least one zone is set

                    Print(start_countdown, count_start_countdown);    // "Arming zones in 10 seconds... (timer displayed on PORTE)"
                    Newline();
                    Print(press_PA0, count_press_PA0);                // "Press PA0 to cancel countdown and stop arming."
                    Newline();

                    TIM2_CR1 = 0x0001;     // enable timer2
                    countdown = 12;        // needs to start at 12 to start at binary 10 on PORTE LEDs for some reason
                    ongoing_countdown = 1; // set ongoing countdown condition for PA0 ISR

                    for(;;){               // run until countdown = 0 or until PA0 is pressed

                        if (countdown == 0) {                       // if countdown reaches 0

                            TIM2_CR1 = 0x0000;                      // Disable timer

                            Newline();
                            Newline();

                            if (zone_1_set == 1) {                  // if zone 1 was set
                                zone_1_armed = 1;
                                Print(zone_1_arm, count_zone_arm);  // "Zone 1 has been armed."
                                Newline();
                            }

                            if (zone_2_set == 1) {                  // if zone 2 was set
                               zone_2_armed = 1;
                               Print(zone_2_arm, count_zone_arm);   // "Zone 2 has been armed."
                               Newline();
                            }

                            if (zone_3_set == 1) {                  // if zone 3 was set
                               zone_3_armed = 1;
                               Print(zone_3_arm, count_zone_arm);   // "Zone 3 has been armed."
                               Newline();
                            }

                            if ((zone_1_armed == 1) && (zone_2_armed == 1) && (zone_3_armed == 1)) {
                                Newline();
                                Print(all_zones_armed, count_all_zones_armed);    // "All 3 zones have been armed."
                                Newline();
                            }

                            break;
                        }

                        if (TIM2_SR.UIF == 1) {                     // If you have multiple timers, can use IF to check status register

                            TIM2_SR.UIF = 0;                        // Clear this flag which will reset the timer

                            countdown = countdown - 1;              // decrement PORTE counter
                            GPIOE_ODR = 0x00FF * countdown;         // decrement PORTE LEDs

                        } // end of if

                        if (GPIOA_IDR.B0 == 1) { break; }           // if PA0 is pressed, call PA0 ISR and break out of for loop

                    } // end of for loop
                } // end of if
                else {

                    while(!((USART1_SR & (1<<7)) == 0x80)){}   // reprint user input to USART
                    USART1_DR = input;

                    Newline();
                    Newline();

                    Print(case_A_1, count_A_1);            // "At least one zone needs to be set before arming!"

                } // end of else

                while (!((USART1_SR & (1<<5))== 0x20)){}   // needed to refresh USART input
                input = USART1_DR;                         // don't know why this works, but it does

                break;

              default:
                
                q_start = 0;                               // ensure starting question will be asked again afterwards

                while(!((USART1_SR & (1<<7)) == 0x80)){}   // reprint user input to USART
                USART1_DR = input;

                Newline();
                Newline();

                Print(try_again, count_try_again);         // "That wasn't an option. Try again."

                while (!((USART1_SR & (1<<5))== 0x20)){}   // needed to refresh USART input
                input = USART1_DR;                         // don't know why this works, but it does

                break;

            } // end of switch
        } // end if
    } // end of main for loop
} // end of main program
// ----------------------------------------------------------------------------------------------------------------------------------------

// SUB FUNCTIONS
void InitializeUSART1() {             // Sub function which initializes the registers to enable USART1 (given)
    RCC_APB2ENR |= 1;                 // Enable clock for Alt. Function. USART1 uses AF for PA9/PA10
    AFIO_MAPR = 0X0F000000;           // Do not mask PA9 and PA10 (because we are using for USART)
    RCC_APB2ENR |= 1<<2;              // Enable clock for GPIOA
    GPIOA_CRH &= ~(0xFF << 4);        // Clear PA9, PA10
    GPIOA_CRH |= (0x0B << 4);         // USART1 Tx (PA9) output push-pull
    GPIOA_CRH |= (0x04 << 8);         // USART1 Rx (PA10) input floating
    RCC_APB2ENR |= 1<<14;             // enable clock for USART1
    USART1_BRR = 0X00000506;          // Set baud rate to 56000
    USART1_CR1 &= ~(1<<12);           // Force 8 data bits. M bit is set to 0.
    USART1_CR2 &= ~(3<<12);           // Force 1 stop bit
    USART1_CR3 &= ~(3<<8);            // Force no flow control and no DMA for USART1
    USART1_CR1 &= ~(3<<9);            // Force no parity and no parity control
    USART1_CR1 |= 3<<2;               // RX, TX enable
    USART1_CR1 |= 1<<13;              // USART1 enable. This is done after configuration is complete
    Delay_ms(100);                    // Wait for USART to complete configuration and enable (not always necessary, but good practice)
} // end of sub function

void Timer2Configuration() {          // Sub function which enables the use of timer 2 (given)
        RCC_APB1ENR.TIM2EN = 1;       // Enable TIMER2 clock
        TIM2_CR1 = 0x0000;            // Disable timer until configuration is complete
        TIM2_PSC = 7999;              // Clock to TIMx_CNT = 72000000
        TIM2_ARR = 9000;              // Reload timer count register with this value when count register resets
        TIM2_CR1 = 0x0001;            // Enable TIMER2
} // end of sub function

void ExternalIntConfig(){           // Sub function to use PA0 and PB6 as external interrupts (given)
        RCC_APB2ENR.AFIOEN = 1;     // Enable clock for alternate pin function
        AFIO_EXTICR1  = 0x00000000; // PA0 as External interrupt
        AFIO_EXTICR2 |= 0x00000100; // PB6 as External interrupt
        EXTI_RTSR |= 0x00000041;    // Set interrupt on rising edge for PA0 and PB6
        EXTI_IMR |= 0x00000041;     // Interrupt on PA0 and PB6 are non-maskable
        NVIC_ISER0 |= 1<<6;         // Enable NVIC interrupt for EXTI line zero (PA0)
                                    // with position 6 in NVIC table
//        NVIC_ISER0 |= 1<<23;        // Enable NVIC interrupt for EXTI9_% (PB6) position 23 in NVIC table
        NVIC_ISER0.B23 = 1;         // * this fixes issues from using the above line
}

void Print(char message[], unsigned int count_message){    // Sub function to Print messages
    for(i=0; i<count_message; i++){
        while (! ((USART1_SR & (1<<7)) == 0x80)) {}
        USART1_DR = message[i];
    }
} // end of sub function

void Newline(){                     // Sub function to print Newline for USART
    while(USART1_SR.TC == 0) {}     // essentially newline (Carriage Return + Line Feed)
    USART1_DR = 0x0D;               // Carriage Return
    while(USART1_SR.TC == 0) {}
    USART1_DR = 0x0A;               // Line Feed
} // end of sub function

void zone_1_breach(){               // Sub function for zone 1 breach procedure

    if (GPIOD_IDR.B4 == 1) {        // if PD4 is pressed, trigger breach for zone 1

        q_start = 0;

        TIM2_CR1 = 0x0001;      // enable timer2

        if (zone_1_armed == 1) {    // if zone 1 is armed

            countdown = 17;

            for(;;){

                if (countdown == 0) {                       // if countdown reaches 0

                    Newline();
                    Newline();
                    Print(zone_1_breached, count_zone_breached);    // "Zone 1 has been breached!"
                    Newline();

                    TIM2_CR1 = 0x0000;                      // Disable timer

                    break;
                } // end of if

                if (TIM2_SR.UIF == 1) {                     // If you have multiple timers, can use IF to check status register

                    TIM2_SR.UIF = 0;                        // Clear this flag which will reset the timer

                    countdown = countdown - 1;              // decrement PORTE counter
                    GPIOE_ODR = 0x00FF * countdown;         // decrement PORTE LEDs
                    // (PORTE only has high, no low, so initial LED value is technically 0x00FF, not 0x0000)
                } // end of if
            } // end of for loop
        } // end of if
    } // end of if
} // end of sub function

void zone_2_breach(){               // Sub function for zone 2 breach procedure

    if (GPIOD_IDR.B5 == 1) {        // if PD4 is pressed, trigger breach for zone 2

        q_start = 0;

        if (zone_2_armed == 1) {    // if zone 2 is armed

            TIM2_CR1 = 0x0001;      // enable timer2
            countdown = 17;

            for(;;){

                if (countdown == 0) {                       // if countdown reaches 0

                    Newline();
                    Newline();
                    Print(zone_2_breached, count_zone_breached);    // "Zone 2 has been breached!"
                    Newline();

                    TIM2_CR1 = 0x0000;                      // Disable timer

                    break;
                } // end of if

                if (TIM2_SR.UIF == 1) {                     // If you have multiple timers, can use IF to check status register

                    TIM2_SR.UIF = 0;                        // Clear this flag which will reset the timer

                    countdown = countdown - 1;              // decrement PORTE counter
                    GPIOE_ODR = 0x00FF * countdown;         // decrement PORTE LEDs
                    // (PORTE only has high, no low, so initial LED value is technically 0x00FF, not 0x0000)
                } // end of if
            } // end of for loop
        } // end of if
    } // end of if
} // end of sub function

void zone_3_breach(){               // Sub function for zone 3 breach procedure

    if (GPIOD_IDR.B6 == 1) {        // if PD6 is pressed, trigger breach for zone 3

        q_start = 0;

        if (zone_3_armed == 1) {    // if zone 3 is armed

            TIM2_CR1 = 0x0001;      // enable timer2
            countdown = 17;

            for(;;){

                if (countdown == 0) {                       // if countdown reaches 0

                    Newline();
                    Newline();
                    Print(zone_3_breached, count_zone_breached);    // "Zone 3 has been breached!"
                    Newline();

                    TIM2_CR1 = 0x0000;                      // Disable timer

                    break;
                } // end of if

                if (TIM2_SR.UIF == 1) {                     // If you have multiple timers, can use IF to check status register

                    TIM2_SR.UIF = 0;                        // Clear this flag which will reset the timer

                    countdown = countdown - 1;              // decrement PORTE counter
                    GPIOE_ODR = 0x00FF * countdown;         // decrement PORTE LEDs
                    // (PORTE only has high, no low, so initial LED value is technically 0x00FF, not 0x0000)
                } // end of if
            } // end of for loop
        } // end of if
    } // end of if
} // end of sub function