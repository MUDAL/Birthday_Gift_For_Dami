#include "app.h"

//Scrolling (navigating the screen)
typedef enum
{
  SCROLL_UP = 0,
  SCROLL_DOWN
}scroll_t;

static void Scroll(scroll_t dir,int& param,int limit)
{
  if(param != limit)
  {
    switch(dir)
    {
      case SCROLL_UP:
        param--;
        break;
      case SCROLL_DOWN:
        param++;
        break;
    }
  }  
}

//Setting time
static void SetHour(int& time,LiquidCrystal& lcd,RTC_DS3231& rtc,IRrecv& irReceiver)
{
  lcd.setCursor(1,0);
  lcd.print('>');  
  while(1)
  {
    CheckAlarms(lcd,rtc); 
    DateTime dateTime = rtc.now();
    irRecv_t irValue = GetIRRemoteVal(irReceiver);
    time %= 24; //24-hour format (00-23)
    lcd.setCursor(8,0);
    DisplayAlignedValue(lcd,time);
    if(LongPress(UP_BUTTON) || (irValue == KEY_UP))
    {
      delay(150);
      time++;
    }
    if(LongPress(DOWN_BUTTON) || (irValue == KEY_DOWN))
    {
      delay(150);
      time--;
      if(time < 0)
      {
        time = 23;
      }
    }
    if(IsPressed(SEL_BUTTON) || (irValue == KEY_OK))
    {
      //set new hour (current value of 'time')
      rtc.adjust(DateTime(dateTime.year(),dateTime.month(),dateTime.day(),
                          time,dateTime.minute(),dateTime.second()));
      break; //exit the loop
    }  
  }  
}

static void SetMinute(int& time,LiquidCrystal& lcd,RTC_DS3231& rtc,IRrecv& irReceiver)
{
  lcd.setCursor(1,1);
  lcd.print('>');  
  while(1)
  {
    CheckAlarms(lcd,rtc);
    DateTime dateTime = rtc.now();
    irRecv_t irValue = GetIRRemoteVal(irReceiver);
    time %= 60; //60-minutes (00-59)
    lcd.setCursor(10,1);
    DisplayAlignedValue(lcd,time);
    if(LongPress(UP_BUTTON) || (irValue == KEY_UP))
    {
      delay(150);
      time++;
    }
    if(LongPress(DOWN_BUTTON) || (irValue == KEY_DOWN))
    {
      delay(150);
      time--;
      if(time < 0)
      {
        time = 59;
      }
    }
    if(IsPressed(SEL_BUTTON) || (irValue == KEY_OK))
    {
      //set new minute (current value of 'time')
      rtc.adjust(DateTime(dateTime.year(),dateTime.month(),dateTime.day(),
                          dateTime.hour(),time,dateTime.second()));
      break; //exit the loop
    }  
  }    
}

static void LoopSong(void(*PlaySong)())
{
  while(1)
  {//loop is broken when reset button is pressed
    PlaySong(); 
  }
}

//Extern functions
void StateFunc_MainMenu(int& state,irRecv_t& irValue,LiquidCrystal& lcd,
                        RTC_DS3231& rtc,int& hour,int& minute)
{
  const int minRow = 0;
  const int maxRow = 3;
  static int currentRow;
  
  DateTime dateTime = rtc.now();
  hour = dateTime.hour();
  minute = dateTime.minute();
  
  DisplayMainMenu(lcd,currentRow,rtc);
  if(IsPressed(UP_BUTTON) || (irValue == KEY_UP))
  {
    Scroll(SCROLL_UP,currentRow,minRow);
  }
  if(IsPressed(DOWN_BUTTON) || (irValue == KEY_DOWN))
  {
    Scroll(SCROLL_DOWN,currentRow,maxRow); 
  }
  if(IsPressed(SEL_BUTTON) || (irValue == KEY_OK))
  {
    switch(currentRow)
    {
      case 0:
        state = STATE_TIMEMENU;
        break;
      case 1:
        state = STATE_ALARMMENU;
        break;
      case 2:
        state = STATE_GAMEMENU;
        break;
      case 3:
        state = STATE_SONGMENU;
        break;
    }
    lcd.clear(); 
  }  
}

void StateFunc_TimeMenu(int& state,irRecv_t& irValue,LiquidCrystal& lcd,
                        RTC_DS3231& rtc,IRrecv& irReceiver,
                        int& hour,int& minute)
{
  const int minRow = 0;
  const int maxRow = 2;
  static int currentRow;
  
  DisplayTimeMenu(lcd,currentRow,hour,minute);
  if(IsPressed(UP_BUTTON) || (irValue == KEY_UP))
  {
    Scroll(SCROLL_UP,currentRow,minRow);
  }
  if(IsPressed(DOWN_BUTTON) || (irValue == KEY_DOWN))
  {
    Scroll(SCROLL_DOWN,currentRow,maxRow); 
  }
  if(IsPressed(SEL_BUTTON) || (irValue == KEY_OK))
  {
    switch(currentRow)
    {
      case 0:
        SetHour(hour,lcd,rtc,irReceiver);
        break;
      case 1:
        SetMinute(minute,lcd,rtc,irReceiver);
        break;
      case 2:
        state = STATE_MAINMENU;
        break;
    }
    lcd.clear(); 
  }  
}

void StateFunc_AlarmMenu(int& state,irRecv_t& irValue,LiquidCrystal& lcd)
{
  const int minRow = 0;
  const int maxRow = 2;
  static int currentRow;

  DisplayAlarmMenu(lcd,currentRow);
  if(IsPressed(UP_BUTTON) || (irValue == KEY_UP))
  {
    Scroll(SCROLL_UP,currentRow,minRow);
  }
  if(IsPressed(DOWN_BUTTON) || (irValue == KEY_DOWN))
  {
    Scroll(SCROLL_DOWN,currentRow,maxRow); 
  }
  if(IsPressed(SEL_BUTTON) || (irValue == KEY_OK))
  {
    switch(currentRow)
    {
      case 0:
        state = STATE_SETALARM;
        break;
      case 1:
        state = STATE_DELETEALARM;
        break;
      case 2:
        state = STATE_MAINMENU;
        break;
    }
    lcd.clear();
  }  
}

void StateFunc_SetAlarm(int& state,irRecv_t& irValue,LiquidCrystal& lcd,
                        RTC_DS3231& rtc,IRrecv& irReceiver)
{
  const int minRow = 0;
  const int maxRow = 3;
  static int currentRow;
  static int hour;
  static int minute;  

  DisplayAlarmSetting(lcd,currentRow,hour,minute);
  if(IsPressed(UP_BUTTON) || (irValue == KEY_UP))
  {
    Scroll(SCROLL_UP,currentRow,minRow);
  }
  if(IsPressed(DOWN_BUTTON) || (irValue == KEY_DOWN))
  {
    Scroll(SCROLL_DOWN,currentRow,maxRow); 
  }
  if(IsPressed(SEL_BUTTON) || (irValue == KEY_OK))
  {
    switch(currentRow)
    {
      case 0:
        SetAlarmHour(hour,lcd,rtc,irReceiver);
        break;
      case 1:
        SetAlarmMinute(minute,lcd,rtc,irReceiver);
        break;
      case 2:
        StoreAlarm(hour,minute,lcd);
        break;
      case 3:
        state = STATE_ALARMMENU;
        break;
    }
    lcd.clear(); 
  }      
}

void StateFunc_DeleteAlarm(int& state,irRecv_t& irValue,LiquidCrystal& lcd,
                           RTC_DS3231& rtc,IRrecv& irReceiver)
{
  const int minRow = 0;
  const int maxRow = 3;
  static int currentRow;
  static int alarmSlot;

  DisplayAlarmDeletion(alarmSlot,lcd,currentRow);
  if(IsPressed(UP_BUTTON) || (irValue == KEY_UP))
  {
    Scroll(SCROLL_UP,currentRow,minRow);
  }
  if(IsPressed(DOWN_BUTTON) || (irValue == KEY_DOWN))
  {
    Scroll(SCROLL_DOWN,currentRow,maxRow); 
  }  
  if(IsPressed(SEL_BUTTON) || (irValue == KEY_OK))
  {
    switch(currentRow)
    {
      case 0:
        SelectAlarmSlot(alarmSlot,lcd,rtc,irReceiver);
        break;
      case 1:
        DeleteAlarmSlot(alarmSlot,lcd,irReceiver);
        break;
      case 2:
        DeleteAllAlarms(lcd);
        break;
      case 3:
        state = STATE_ALARMMENU;
        break;
    }
    lcd.clear(); 
  }      
}

void StateFunc_GameMenu(int& state,irRecv_t& irValue,LiquidCrystal& lcd)
{
  const int minRow = 0;
  const int maxRow = 1;
  static int currentRow;

  DisplayGameMenu(lcd,currentRow);
  if(IsPressed(UP_BUTTON) || (irValue == KEY_UP))
  {
    Scroll(SCROLL_UP,currentRow,minRow);
  }
  if(IsPressed(DOWN_BUTTON) || (irValue == KEY_DOWN))
  {
    Scroll(SCROLL_DOWN,currentRow,maxRow); 
  }
  if(IsPressed(SEL_BUTTON) || (irValue == KEY_OK))
  {
    switch(currentRow)
    {
      case 0:
        state = STATE_PLAYGAME;
        break;
      case 1:
        state = STATE_MAINMENU;
        break;
    }
    lcd.clear(); 
  }    
}

void StateFunc_PlayGame(int& state,irRecv_t& irValue,LiquidCrystal& lcd,
                        RTC_DS3231& rtc,IRrecv& irReceiver)
{
  const int minRow = 0;
  const int maxRow = 2;
  static int currentRow;

  DisplayPlayGame(lcd,currentRow);
  if(IsPressed(UP_BUTTON) || (irValue == KEY_UP))
  {
    Scroll(SCROLL_UP,currentRow,minRow);
  }
  if(IsPressed(DOWN_BUTTON) || (irValue == KEY_DOWN))
  {
    Scroll(SCROLL_DOWN,currentRow,maxRow); 
  }
  if(IsPressed(SEL_BUTTON) || (irValue == KEY_OK))
  {
    switch(currentRow)
    {
      case 0:
        PlayGame(lcd,rtc,irReceiver);
        state = STATE_MAINMENU;
        break;
      case 1:
        DisplayHowToPlay(lcd);
        break;
      case 2:
        state = STATE_GAMEMENU;
        break;
    }
    lcd.clear(); 
  }        
}

void StateFunc_SongMenu(int& state,irRecv_t& irValue,LiquidCrystal& lcd)
{
  const int minRow = 0;
  const int maxRow = 3;
  static int currentRow;

  DisplaySongMenu(lcd,currentRow);
  if(IsPressed(UP_BUTTON) || (irValue == KEY_UP))
  {
    Scroll(SCROLL_UP,currentRow,minRow);
  }
  if(IsPressed(DOWN_BUTTON) || (irValue == KEY_DOWN))
  {
    Scroll(SCROLL_DOWN,currentRow,maxRow); 
  }
  if(IsPressed(SEL_BUTTON) || (irValue == KEY_OK))
  {
    switch(currentRow)
    {
      case 0:
        lcd.setCursor(1,0);
        lcd.print('>');
        LoopSong(PlaySong_TakeOnMe);
        break;
      case 1:
        lcd.setCursor(1,1);
        lcd.print('>');
        LoopSong(PlaySong_Birthday);
        break;
      case 2:
        lcd.setCursor(1,2);
        lcd.print('>');
        LoopSong(PlaySong_Starwars);
        break;
      case 3:
        state = STATE_MAINMENU;
        break;
    }
    lcd.clear(); 
  }         
}

