/////////////////////////////////////////////////
/////////////// Vacuum robot menu ///////////////
/////////////////////////////////////////////////

#include <MenuBackend.h>
#include <LiquidCrystal_I2C.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <EEPROM.h>
#include <Wire.h>

//#include <LCD.h>
//#include <Time.h>
//#include <math.h>
//#include <TimeAlarms.h>

//Initialise the LCD
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address


///////////////////////////////////////////////
// just for reading button resistance values
int A;
///////////////////////////////////////////////

volatile int ZM=-1;
volatile int X=-1;      
volatile int stan_Analog;
char *Line_1;
char *Line_2;
const char *DAYSHORT[] = {"","Sunday","Monday", "Tuesday", "Wednesd", "Thursda", "Friday", "Saturda"};

// sound & light variables
#define REDLEDPIN 10
#define GREENLEDPIN 11
#define BUZZERPIN 12

// time variables
int DAYNR = 0;
int REALDAYNR;
int OLDDAY;
int DAYTIMER = 0;
int RUNONCE = 0;
int HOURTIME = 0;
int MINUTETIME = 0;

//eeprom readadres
int ADRESNR = 0;

//debounce variables
int i;
int SUM;

//lcd BACKLIGHT variables
int IDLECOUNTER; 
byte BACKLIGHT = 0;

//cursor variables
byte CURPOS;
byte CURPOSAD;
byte CURPOSMIN;
byte CURPOSMAX;

//// characters ////
///// up //////
uint8_t arrowUp[8] = {
B00000,
B00000,
B00100,
B00100,
B01110,
B01110,
B11111,
B00000,
};
///// down //////
uint8_t arrowDown[8]  = {
B00000,
B11111,
B01110,
B01110,
B00100,
B00100,
B00000,
B00000,
};
///// right //////
uint8_t arrowRight[8] = {
B00000,
B10000,
B11100,
B11111,
B11100,
B10000,
B00000,
B00000,
};
///// left //////
uint8_t arrowLeft[8] = {
B00000,
B00001,
B00111,
B11111,
B00111,
B00001,
B00000,
B00000,
};
///// select //////
uint8_t arrowSelect[8] = {
B00000,
B00001,
B00001,
B00101,
B01101,
B11111,
B01100,
B00100,
};

///// left bell part //////
uint8_t belll[8] = {
B01001,
B10010,
B10010,
B00100,
B00100,
B01111,
B00000,
B00001,
};

///// right bell part //////
uint8_t bellr[8] = {
B10010,
B01001,
B01001,
B00100,
B00100,
B11110,
B00000,
B10000,
};

///// left clock part //////
uint8_t wekkerl[8] = {
B01100,
B10011,
B10101,
B01001,
B01001,
B01000,
B00100,
B00011,
};
///// right clock part //////
uint8_t wekkerr[8] = {
B01100,
B10010,
B01010,
B00100,
B11100,
B00100,
B01000,
B10000,
};



void menuUseEvent(MenuUseEvent used) 
{
   if (used.item.getName() == " START VACUUM ") 
     {
      Startvacuum();
      menu.moveDown();
     }
   if (used.item.getName() == "  SET ALARM   ")
     {
      menu.moveDown();
     }
   if (used.item.getName() == "  SHOW TIME  ") 
     {
      Clock();
     }
   if (used.item.getName() == "* STOP VACUUM  *") 
     {
      Stopvacuum();
      menu.moveUp();
     }
   if (used.item.getName() == "Monday"){DAYNR = 1; SetAlarm();}     
   if (used.item.getName() == "Monday"){DAYNR = 2;  SetAlarm();}
   if (used.item.getName() == "Tuesday"){DAYNR = 3;  SetAlarm();}
   if (used.item.getName() == "Wednesday"){DAYNR = 4; SetAlarm();}
   if (used.item.getName() == "Thursday"){DAYNR = 5; SetAlarm();}
   if (used.item.getName() == "Friday"){DAYNR = 6; SetAlarm();}
   if (used.item.getName() == "Saturday"){DAYNR = 7; SetAlarm();}

}

// --- Response to pressing ----------------------------------------- ------------------------
void menuChangeEvent (MenuChangeEvent changed) // function class MenuBackend
{
   int c = changed.to.getShortkey ();
   lcd.clear ();
   lcd.setCursor (0,0);

   if (c == 1)
     {
     lcd.write (4); // Left arrow
     strcpy (Line_1, changed.to.getName ()); // Create a string in the first line
     lcd.print (Line_1); // Display it
     lcd.setCursor (15,0); lcd.write (2); // Right arrow
     lcd.setCursor (4,1); 
     lcd.print ("Select ");
     lcd.write (5); // Select
     }
     
    if (c == 2)
    {
    lcd.write (" Set alarm for  ");
    lcd.setCursor (0,1);lcd.write (1); //up arrow    
    strcpy (Line_2, changed.to.getName ());
    lcd.setCursor (3,1);
    lcd.print (changed.to.getName()); // Display it
    lcd.setCursor (14,1);lcd.write (4); //left arrow
    lcd.write (2); //right arrow
    lcd.setCursor (12,1); lcd.write (5); // Select
    }
    
   if (c == 3)
     {
     strcpy (Line_1, changed.to.getName ()); // Create a string in the first line
     lcd.print (Line_1); // Display it
     lcd.setCursor (4,1); 
     lcd.print ("Select ");
     lcd.write (5); // Select
     }
}

void Startvacuum ()
{
   digitalWrite(GREENLEDPIN, HIGH); 
   digitalWrite(REDLEDPIN, HIGH); 
   Makesound();
   Wire.beginTransmission(0x50); // transmit to device #50
   Wire.write("Startt");        // send
   Wire.endTransmission();    // stop transmitting
}

void Stopvacuum ()
{
   digitalWrite(GREENLEDPIN, LOW); 
   digitalWrite(REDLEDPIN, LOW); 
   Stopsound();
   Wire.beginTransmission(0x50); // transmit to device #50
   Wire.write("Stopp");        // send
   Wire.endTransmission();    // stop transmitting
}

void SetAlarm()
{
  lcd.clear();
  lcd.setCursor (0,0);lcd.print(DAYSHORT[DAYNR]);
  readeeprom();
  
 ///////////////////////////////////////
 /////////////// no alarm set //////////
 /////////////////////////////////////// 
 
  if (HOURTIME > 23)
  {
    lcd.setCursor (8,0);lcd.print ("no alarm");  
    lcd.setCursor (0,1); 
    lcd.print ("Set y n"); lcd.write (5);//select
    lcd.setCursor (14,1);lcd.write (4); lcd.write (2);
    CURPOS = 6; CURPOSAD = 2; CURPOSMIN = 4; CURPOSMAX = 6;
    lcd.setCursor (CURPOS,1); lcd.cursor ();
    do
    {
       X=Read_1(0);delay(20);
         if(ZM != X)   // if it was the change in the state are:
         {
            switch(X) // check to see what was pressed
            {
               case 2: movecursorRight (); break;
               case 4: movecursorLeft (); break; 
               case 5: break;
            }
         } ZM = X;
   }
   while(X != 5);
   if (CURPOS == 4) {setalarmtime();}
   if (CURPOS == 6) {lcd.noCursor(); menu.moveRight();}
  }
  
////////////////////////////////////////////  
/////////////// alarm already set //////////
//////////////////////////////////////////// 

   else if (HOURTIME < 24){
   lcd.setCursor (8,0); lcd.write (6);; lcd.write (7);// bell
   lcd.setCursor (11,0); printtime();
   lcd.setCursor (0,1);
   lcd.print ("Change y n cl");
   lcd.write (5);//select
   lcd.setCursor (14,1);lcd.write (4); lcd.write (2);
   CURPOS = 9; CURPOSAD = 2; CURPOSMIN = 7; CURPOSMAX = 11;
   lcd.setCursor (CURPOS,1); lcd.cursor ();
   do
   {
    X=Read_1(0);delay(20);
         if(ZM != X)   // if it was the change in the state are:
         {
           switch(X) // check to see what was pressed
           {
              case 2: movecursorRight (); break;
              case 4: movecursorLeft (); break; 
              case 5: break;
           }
         } ZM = X;
   }
   while(X != 5);
   if (CURPOS == 7) {setalarmtime();}
   if (CURPOS == 9) {lcd.noCursor(); menu.moveRight();}
   if (CURPOS == 11) {clearalarm();}
  }
}
void clearalarm(){
  HOURTIME = 24; //clear alarm
  writeeeprom(); lcd.noCursor();
  lcd.clear(); lcd.setCursor (0,0); lcd.print (DAYSHORT[DAYNR]);
  lcd.setCursor (8,0);lcd.print ("no alarm");  
  lcd.setCursor (0,1); lcd.print ("Alarm is cleared.");
  delay (2000); menu.moveRight();
}

void setalarmtime (){ 
  if (HOURTIME >23){HOURTIME = 0; MINUTETIME = 0;}
  lcd.setCursor (0,1); 
  lcd.print ("                ");
  lcd.setCursor (0,1); 
  lcd.write (1); lcd.write (3); //up & down arrow
  lcd.setCursor (4,1);  printtime();
  lcd.write (5); //select
  lcd.setCursor (14,1);lcd.write (4); lcd.write(2); //left & right arrow
  CURPOS = 5; CURPOSAD = 3; CURPOSMIN = 5; CURPOSMAX = 8;
  lcd.setCursor (CURPOS,1);
  lcd.cursor ();
  do
   {
    X=Read_1(0);delay(20);
       switch(X) // check to see what was pressed
       {
       case 1: increaseTime(); break;
       case 2: movecursorRight (); break;
       case 3: decreaseTime(); break;
       case 4: movecursorLeft (); break; 
       case 5: break;
       }
   }
   while(X != 5);
   writeeeprom (); 
   RUNONCE = 0; //reset runonce per day because alarmtime changed
   lcd.noCursor ();
   lcd.clear(); lcd.setCursor (0,0); lcd.print (DAYSHORT[DAYNR]);
   lcd.setCursor (8,0); lcd.write (6);; lcd.write (7);// bell
   lcd.setCursor (11,0); printtime();
   lcd.setCursor (2,1); lcd.print ("Alarm is set.");
   delay (2000); menu.moveRight();
}

void printtime(){
if (HOURTIME < 10){lcd.print ("0");}
  lcd.print (HOURTIME);
  lcd.print (":");
if (MINUTETIME < 10){lcd.print ("0");}
   lcd.print (MINUTETIME);
}

void movecursorRight()
{
  CURPOS = (CURPOS + CURPOSAD);
  if (CURPOS > CURPOSMAX) {CURPOS = CURPOSMAX;}
  lcd.setCursor (CURPOS,1);
}

void movecursorLeft()
{
  CURPOS = (CURPOS - CURPOSAD);
  if (CURPOS < CURPOSMIN) {CURPOS = CURPOSMIN;}
  lcd.setCursor (CURPOS,1);
}

void increaseTime ()
{
 if (CURPOS == 5){HOURTIME = HOURTIME+1;}
 if (HOURTIME > 23){HOURTIME = 0;}
 if (CURPOS == 8){MINUTETIME = MINUTETIME+1;}
 if (MINUTETIME > 59){MINUTETIME = 0;}
 lcd.setCursor(4,1); printtime();
 lcd.setCursor (CURPOS,1);
}

void decreaseTime ()
{
 if (CURPOS == 5){HOURTIME = HOURTIME-1;}
 if (HOURTIME < 0){HOURTIME = 23;}
 if (CURPOS == 8){MINUTETIME = MINUTETIME-1;}
 if (MINUTETIME < 0){MINUTETIME = 59;}
 lcd.setCursor(4,1); printtime();
 lcd.setCursor (CURPOS,1);
}

void eepromadress(){
  if (DAYNR == 1){ADRESNR =0;}//Sunday  
  if (DAYNR == 2){ADRESNR =2;}//Monday
  if (DAYNR == 3){ADRESNR =4;}//Tuesday
  if (DAYNR == 4){ADRESNR =6;}//Wednesday
  if (DAYNR == 5){ADRESNR =8;}//Thursday
  if (DAYNR == 6){ADRESNR =10;}//Friday
  if (DAYNR == 7){ADRESNR =12;}//Saturday
}

void readeeprom(){
  eepromadress();
  HOURTIME = EEPROM.read (ADRESNR);
  MINUTETIME = EEPROM.read (ADRESNR+1);
}

void writeeeprom (){
  eepromadress();
  EEPROM.write (ADRESNR,HOURTIME);
  EEPROM.write ((ADRESNR+1),MINUTETIME);
}

void setrealdaynr(){
REALDAYNR = EEPROM.read (14); 
OLDDAY = EEPROM.read (15);
/////////////////////////////////////////////////////////////
// uncomment next two lines to set date & daynumber once, where sunday = 1
//REALDAYNR = 1; EEPROM.write (14,REALDAYNR);
//OLDDAY = 20; EEPROM.write (15,OLDDAY); //day date
/////////////////////////////////////////////////////////////
 tmElements_t tm;
 if (RTC.read(tm))
   {
     if (OLDDAY != tm.Day)
     {DAYTIMER = DAYTIMER +1;//counter still needs some work
     if (DAYTIMER == 5)
        {
         OLDDAY = tm.Day;
         REALDAYNR = REALDAYNR + 1;
         if (REALDAYNR == 8){REALDAYNR = 1;}
         RUNONCE = 0; //reset run once per day on alarm because realdaynr changed
         DAYTIMER = 0;
         EEPROM.write (14,REALDAYNR);
         EEPROM.write (15,OLDDAY);
        }
     }
   }
}

void checkalarm(){
  DAYNR = REALDAYNR;
  eepromadress();
  HOURTIME = EEPROM.read (ADRESNR);
  MINUTETIME = EEPROM.read (ADRESNR+1);
  tmElements_t tm;
  if (RTC.read(tm))
  {
    if ((tm.Hour == HOURTIME)&&(tm.Minute == MINUTETIME))
///////////// start vacuum //////////////////
    {
     RUNONCE = RUNONCE + 1; //run only one time per day on alarm, reset if realdaynr changes
     if (RUNONCE == 1)
     {
     lcd.backlight();delay(500); 
     Startvacuum();
     menu.toRoot();menu.moveDown();menu.moveDown();
     }
    }
  }
}

void Makesound(){
 tone(BUZZERPIN,500); 
 delay (500);
 noTone(BUZZERPIN);
 }

void Stopsound(){
 noTone(BUZZERPIN);
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {lcd.print('0');}
  lcd.print(number);
}

void Clock(){
lcd.clear();
lcd.setCursor(4,0); lcd.print ("Time is:");
lcd.setCursor(13,1);
lcd.write (5);
    tmElements_t tm;
    volatile int  x=-1; 
    while(x !=5)
      {
         if (RTC.read(tm)) 
           {
                lcd.setCursor(0,1);
                lcd.print ("             ");
                lcd.setCursor(4,1);
                print2digits(tm.Hour);
                lcd.print(':');
                print2digits(tm.Minute);
                lcd.print(':');
                print2digits(tm.Second);
                lcd.print(' ');
           }
           delay(100); 
           x=Read_1(0); 
      }
   menu.moveRight();menu.moveLeft();
}

volatile int Read_1(int analog)
{
SUM = 0;
for (i=0; i < 4; i++)
  {
    SUM += analogRead (analog);delay (10);
  } 
  stan_Analog = SUM / 4; 
    if (stan_Analog > 1000) return -1; // For values ​​outside the range
    if (stan_Analog >240 && stan_Analog < 260) return 1; // Up
    if (stan_Analog >705 && stan_Analog < 725) return 2; // Right
    if (stan_Analog < 20) return 3; // Down
    if (stan_Analog >132 && stan_Analog < 152) return 4; // Left
    if (stan_Analog >453 && stan_Analog < 473) return 5; // Select
    return -1; // Nothing pressed
}

void analogread(){
//////// display analog reading values ////////////////////////////////
  A = analogRead(0);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  analogRead() ");
  lcd.setCursor(0,1);
  lcd.print("  value is :");
  lcd.print(A);
  delay(1000);
}

void setup ()
{
   Wire.begin();
   Line_1 = new char [16]; // Initialize a pointer to a dynamic text
   Line_2 = new char [16]; 
   lcd.begin (16, 2); // Initialize the LCD
   lcd.createChar (1, arrowUp);
   lcd.createChar (2, arrowRight);
   lcd.createChar (3, arrowDown);
   lcd.createChar (4, arrowLeft); 
   lcd.createChar (5, arrowSelect);
   lcd.createChar (6, belll);
   lcd.createChar (7, bellr);
   pinMode(GREENLEDPIN, OUTPUT);
   pinMode(REDLEDPIN, OUTPUT);
   menuSetup (); 
   menu.toRoot();
   lcd.noBacklight(); // turn off backlight
}

void loop (){
  setrealdaynr ();
  checkalarm();
   X=Read_1(0); delay(30);   // Read the state of the keyboard:
   if(ZM!=X){
       IDLECOUNTER = 0;lcd.backlight();
       switch(X) // check to see what was pressed
       {
       case 1: menu.moveUp(); break; // Menu up
       case 2: menu.moveRight(); break; // Menu right
       case 3: menu.moveDown(); break; // Menu down
       case 4: menu.moveLeft(); break; // Menu left
       case 5: menu.use(); break; // Select
       }
     } ZM = X;
     IDLECOUNTER = (IDLECOUNTER +1);
     if (IDLECOUNTER == 1500)
     {
     lcd.noBacklight();
     BACKLIGHT = 0;
     menu.toRoot();
     IDLECOUNTER = 0;
     }      
}
////////// END ///////////////////////////////

