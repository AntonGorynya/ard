//#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h> 

#define BUTTON_PIN1 4
#define BUTTON_PIN2 2
#define BUTTON_PIN3 3
#define LIGHT1 9
#define LIGHT2 7
#define LIGHT3 8
#define LED_0_1 5
#define LED_0_2 6
#define LED_pi_1 5
#define LED_pi_2 6
#define ARRAYSIZE 3

String modes[ARRAYSIZE] = { "normal >", "< manual >", "< delayed" };
int tdelay = 0;
bool fix = false;
int mode = 0;
int phase = 0;
int x = 0;
int hz = 1;
int hz2 = 1;
int long i = 0;
unsigned long time_m = 0;
unsigned long time_f = 0;
// в дальнейшем приравниаем к одному из LED
int LED1;
int LED2;

char myStr1[15] = {'S', 't', 'a', 'r', 't'};
char myStr2[15] = {'S', 't', 'a', 'r', 't'};


class Flasher
{
  // Переменные - члены класса
  // Инициализируются при запуске
  int ledPin; // номер пина со светодиодом
  long OnOffTime; // время включения и выключения в миллисекундах
  int phase; // сдвиг по фазе 

  // Текущее состояние
  int ledState; // состояние ВКЛ/ВЫКЛ
  unsigned long previousMillis; // последний момент смены состояния

  // Конструктор создает экземпляр Flasher и инициализирует 
  // переменные-члены класса и состояние
  public:
  Flasher(int pin, long onoff, int phase)
  {
     ledPin = pin;
     pinMode(ledPin, OUTPUT);    
     OnOffTime = onoff;
     phase = phase; 
     ledState = LOW;
     if (phase == 2){
      ledState = HIGH;  
     }  
    previousMillis = 0;
  }
  void Update()
  {
    // выясняем не настал ли момент сменить состояние светодиода
    unsigned long currentMillis = micros(); // текущее время в миллисекундах
    if((currentMillis - previousMillis >= OnOffTime*1000))
    {
       ledState = !ledState; // выключаем
       previousMillis = currentMillis; // запоминаем момент времени
       digitalWrite(ledPin, ledState); // реализуем новое состояние  
    }
   
  }
};

//  не стирать иначе, не компилируется!
Flasher led1(LED_0_1, 1000, phase);
Flasher led2(LED_0_2, 1000, phase);

//LiquidCrystal lcd(8, 9, 10, 11, 12, 13); 
//3F or 27
LiquidCrystal_I2C lcd(0x27,16,2); 


void setup()
{
  //устанавливаем размер (количество столбцов и строк) экрана
  //lcd.begin(16, 2);  //if not i2c  
  // I2C  
  lcd.begin();  

  pinMode(BUTTON_PIN1, INPUT_PULLUP);
  pinMode(BUTTON_PIN2, INPUT_PULLUP);
  Serial.begin(9600); 
  lcd.print("Hello dreamer!");
  // устанавливаем курсор в колонку 0, строку 1. То есть на
  // самом деле это вторая строка, т.к. нумерация начинается с нуля
  lcd.setCursor(0, 1);    
  lcd.print("choose mode");   

}
 
void loop()
{  
  //подсветка
 // Flasher light1(LIGHT1, 1000, phase);
//  Flasher light2(LIGHT2, 3000, phase);
 // Flasher light3(LIGHT3, 4000, phase); 
 // light1.Update();
 // light2.Update();
 // light3.Update();
  //выбор режима
  if (mode == 0 ){  
    delay(2000);
    mode = menu_zero(BUTTON_PIN1, BUTTON_PIN2, BUTTON_PIN3);
   // light1.Update();
   // light2.Update();
   // light3.Update();     
  }
  // обычный режим
  if (mode == 1 ){
    fix = true;
    phase = 2;
    lcd.clear(); 
    lcd.print("default");   
    x =1 ;
    for (x; x < 8; x = x+1){ 
      hz =11 - x; // нижний придел 
      lcd.clear();  
      print_hz(hz,hz);
      // /1000 /60       
      generate(hz, 5); 
    } 
    hz =0;
    lcd.print("delay");
    delay(3600000); 
    x = 1 ;
    for (x; x < 5; x = x+1){  
      // 24000 = 4 минуты на епждк. частоту. Всего частот 5, на каждую 4 минуты
      hz = x +4; // нижний придел 
      lcd.clear();  
      print_hz(hz,hz);       
      generate(hz, 5);      
    } 
    hz = 40;       
    print_hz(hz,hz);
    generate(hz, 120);  
  }
  // режим с установкой частоты
  if (mode == 2){   
    if (phase == 0){ 
      lcd.clear();
      lcd.print("choose phase");
      lcd.setCursor(0, 1);    
      lcd.print("Pi/2 or 0");
      delay(100);
      phase = menu_phase(BUTTON_PIN1, BUTTON_PIN2);
      Serial.print("phase =");
      Serial.println(phase);      
    }  
    else { 
      if (digitalRead(BUTTON_PIN1)){ 
        hz = hz + 1; 
        hz2 = hz; 
        lcd.clear();         
        print_hz(hz, hz2);
        delay(100);         
        generate(hz, 120);
      } 
      if (digitalRead(BUTTON_PIN2) && hz > 0){ 
        hz = hz - 1; 
        hz2 = hz; 
        lcd.clear();
        print_hz(hz, hz2);
        delay(100);          
        generate(hz, 120);
      } 
      if (digitalRead(BUTTON_PIN3) && hz > 0) {       
        if (fix == false) {
          fix = true; 
          lcd.clear(); 
          sprintf(myStr2,"fix f: %d hz",hz2 ); 
          lcd.print(myStr2);
          delay(tdelay*60*1000);
          generate(hz, 120);
        }     

      } 
    }

  } 


  if (mode == 3 && tdelay == 0){
    lcd.clear();
    lcd.print("choose delay");
    delay(2000);
    tdelay = setupdelay();  
  }


  
} 


int hz_to_ms(int hz){
  return 1000/hz;
}


void print_hz(int hz, int hz2){              
    sprintf(myStr1,"HZ1:= %d",hz);
    lcd.print(myStr1);    
    Serial.print(myStr1);
    lcd.setCursor(0,1);
    sprintf(myStr2,"HZ2:= %d",hz);
    printf(myStr2,"HZ2:= %d",hz2);
    Serial.println(myStr2);
} 

int menu_zero(int pin1, int pin2, int pin3){
  i = 0;
  lcd.clear();
  lcd.print("choose mode:");
  lcd.setCursor(0, 1);    
  lcd.print(modes[i]);
  while(true){
    if (digitalRead(pin1) && i < ARRAYSIZE - 1) {
    i++; 
    Serial.println(i);
    Serial.println(modes[i]);
    lcd.clear();
    lcd.print("choose mode:");
    lcd.setCursor(0, 1);   
    //lcd.print(i); 
    lcd.print(modes[i]);
    delay(500); 
    }
    if (digitalRead(pin2) && i > 0) {
    i--; 
    Serial.println(i);
    Serial.println(modes[i]);
    lcd.clear();
    lcd.print("choose mode:");
    lcd.setCursor(0, 1); 
    //lcd.print(i); 
    lcd.print(modes[i]);   
    delay(500);    
    }
    if (digitalRead(pin3)){
      i++;
      return i;  
    }
  }   
}

int menu_phase(int pin1, int pin2){
  while(true){    
      if (digitalRead(pin1)){
      lcd.clear();          
      lcd.print("phase 0 on ");
      lcd.print(LED_0_1);
      phase = 1;
      delay(300); 
      LED1 = LED_0_1;
      LED2 = LED_0_2;
      return phase;
      }
      if (digitalRead(pin2)) {
      phase = 2; 
      lcd.clear();         
      lcd.print("phase Pi/2 on"); 
      lcd.print(LED_pi_1);
      delay(300);       
      LED1 = LED_pi_1;
      LED2 = LED_pi_2; 
            return phase; 
      }
  }  
  }



void generate(int hz, int time_max) {  
  printf(myStr2,"hz_to_ms(hz)/2:= %d",hz_to_ms(hz)/2);
  Serial.println(myStr2);    
  Flasher led1(LED2, hz_to_ms(hz)/2,0);  
  Flasher led2(LED1, hz_to_ms(hz)/2,phase);
  time_f=micros()/1000/1000/60;
  time_m = 0;
  while(time_m < time_max){ 
    led1.Update(); 
    led2.Update();
    if (not fix){
      if (digitalRead(BUTTON_PIN1) or digitalRead(BUTTON_PIN2) or digitalRead(BUTTON_PIN3)) {
        break;
      }
    }
        time_m=micros()/1000/1000/60-time_f;
  }
} 

int setupdelay() {
  while (true){
    if (digitalRead(BUTTON_PIN1)){ 
      tdelay = tdelay + 10; 
      lcd.clear(); 
      lcd.print("delay is:");
      lcd.setCursor(0,1);
      sprintf(myStr2,"%d min",tdelay ); 
      lcd.print(myStr2);
      delay(100);
    } 
    if (digitalRead(BUTTON_PIN2) && tdelay > 0){ 
      tdelay = tdelay - 10; 
      lcd.clear(); 
      lcd.print("delay is:");
      lcd.setCursor(0,1);
      sprintf(myStr2,"%d min",tdelay ); 
      lcd.print(myStr2);
      delay(100);
    } 
    if (digitalRead(BUTTON_PIN3) && tdelay > 0) {  
      mode = 2;
      lcd.clear(); 
      sprintf(myStr2," fix %d min",tdelay ); 
      lcd.print(myStr2); 
      return tdelay;     
    }
  }
}
