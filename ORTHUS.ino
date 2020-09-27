#include <SoftwareSerial.h>//библиотека для реализации последовательного интерфейса
#include <DFPlayer_Mini_Mp3.h> //библиотека для работы с mp3 модулем
#include "notes.h" //библиотека с нотами
#include "defender.h" //библиотека с основными функциями

int melody_panic[] = {
  NOTE_C4, NOTE_C5, NOTE_G4, NOTE_G5  //массив нот сигнализации
};
int noteDurations[] = { 
  4, 4, 4, 4   //массив продолжительности каждой из нот
};

void panic(){//функция сирены
  for (int thisNote = 0; thisNote < 4; thisNote++) {
    // чтобы рассчитать продолжительность ноты, возьмите одну секунду, разделенную на тип ноты.
    //например, четвертинная нота = 1000 / 4, восьмая нота = 1000/8, и т. д.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(8, melody_panic[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration*1.30; //чтобы различать ноты, устанавливаем минимальное время между ними.
    delay(pauseBetweenNotes);
    noTone(8); //остановить мелодию
  }
}

void setup(){
    servo.attach(2);
    Serial.begin (9600);
    mp3_set_serial (Serial);    
    mp3_set_volume (25);
    delay (100);
    pinMode(TRIG1, OUTPUT);
    pinMode(ECHO1, INPUT);
    pinMode(TRIG2, OUTPUT);
    pinMode(ECHO2, INPUT);
    //два датчика расстояния необходимы для отслеживания на все 360 градусов
    pinMode(LeftAhead, OUTPUT);
    pinMode(LeftBack, OUTPUT);
    pinMode(RightAhead, OUTPUT);
    pinMode(RightBack, OUTPUT);
}

void loop(){
  unsigned const short int delta=30;//константа для определения движущихся целей
  unsigned short int arr1[180];// массив данных первого датчика
  unsigned short int arr2[180];// массив данных второго датчика
  delay(100);
AGAIN:  
/////////////////////////Анализ территории//////////////////////////////
  for(angle=90;angle>=0;angle--){//поворот «головы» направо
    //серво привод может поворачиваться максимум на 180 градусов, поэтому начало отсчёта будет из середины
    servo.write(angle);//задаём угол поворота
    checkDistance();//функция библиотеки defender.h, считывающая показания с датчиков
    delay(5);//функция задержки. в качестве параметра указывается длительность задержки в миллисекундах
    arr1[angle]=look1;// записываем полученные данные об окружении в массив (первый датчик)
    arr2[angle]=look2;// записываем полученные данные об окружении в массив (второй датчик)
  }

  delay(100);
  for(angle=0;angle<=180;angle++){//поворот «головы» налево
    servo.write(angle);
    checkDistance();
    delay(5);
    arr1[angle]=look1;
    arr2[angle]=look2;
  }

  delay(100);
  for(angle=180;angle>=90;angle--){//возврат «головы» в исходное положение
    servo.write(angle);
    checkDistance();
    delay(5);
    arr1[angle]=look1;
    arr2[angle]=look2;
    }
////////////////////////Конец анализа территории////////////////////////

////////////////////////Взаимодействие с целью//////////////////////////
/***********************Поиск цели*************************************/
  unsigned short int q=0;//переменная для подсчёта целей
  unsigned short int turn;//переменная для поворота
 
  while(q==0){//сканирование пространства, пока не будет найдена цель
    for(angle=90;angle>=0;angle--){//поворот «головы» направо
      servo.write(angle);
      checkDistance();
      delay(5);
      if(((arr1[angle]-look1)>delta)||((arr2[angle]-look2)>delta)){//проверка на обнаружение движущихся целей
        q++;//если таковые найдены - увеличиваем счётчик на 1
      }
    }
    
    delay(100);
    for(angle=0;angle<=180;angle++){//поворот «головы» налево
      servo.write(angle);
      checkDistance();
      delay(5);
      if(((arr1[angle]-look1)>delta)||((arr2[angle]-look2)>delta)){
        q++;
      }
    }
    
    delay(100);
    for(angle=180;angle>=90;angle--){//возврат «головы» в исходное положение
      servo.write(angle);
      checkDistance();
      delay(5);
      if(((arr1[angle]-look1)>delta)||((arr2[angle]-look2)>delta)){
        q++;
      }
    }
  }
/***********************Несколько целей********************************/
  while(q>1){//Если целей несколько:
    panic();
    Serial.print("ОНИ ПОВСЮДУ\n");
    unsigned short int i;//переменная для возможности выхода из цикла
    for(angle=90;angle>=0;angle--){//поворот «головы» направо
      servo.write(angle);
      checkDistance();
      delay(5);
      if(((arr1[angle]-look1)>delta)||((arr2[angle]-look2)>delta)){
        i++;
      }
    }
  
    delay(100);
    for(angle=0;angle<=180;angle++){//поворот «головы» налево
      servo.write(angle);
     checkDistance();
      delay(5);
      if(((arr1[angle]-look1)>delta)||((arr2[angle]-look2)>delta)){
        i++;
      }
    }
  
    delay(100);
    for(angle=180;angle>=90;angle--){//возврат «головы» в исходное положение
      servo.write(angle);
      checkDistance();
      delay(5);
      if(((arr1[angle]-look1)>delta)||((arr2[angle]-look2)>delta)){
        i++;
      }
    }
    q=i;//датчик повторно сканирует область пока не уменьшится количество целей
    //а именно, пока i не станет меньше 2 (q изменяется вместе с i)
    //если целей стало меньше - выходим из цикла
  }
  goto AGAIN;
/**********************************************************************/

/***********************Одна цель**************************************/
  if(q==1){
    unsigned short moving=0;//переменная для перемещения
    for(angle=90;angle>=0;angle--){//поворот «головы» направо
      servo.write(angle);
      checkDistance();
      delay(5);
      if((arr1[angle]-look1)>delta){//для первого датчика
        turn=angle;
        break;
        Serial.print("ЗАМЕЧЕН НАРУШИТЕЛЬ\n");
        mp3_play();//проигрывание предупреждения
        delay(7000);
        for(turn; turn<=90;turn++){//возврат «головы» в исходное состояния для возможности взаимодействия с целью
          servo.write(turn);
          delay(5);
        }
        goRight();//поворот к цели
        delay((90-turn)*10);//время вращения
        //время вращения для 10 градусов составляет примерно 0.1 сек.(delay(100))
        stopRobot();//останавливаем вращение
        while((moving<=300)||(look1<=300)){//робот будет передвигаться на такое же расстояние к нарушителю, как и нарушитель к роботу
          //пока не выйдет за пределы осматриваемого радиуса или пока нарушитель не выйдет из поля зрения
          checkDistance();
          arr1[90]=look1;
          checkDistance();
          goAhead();
          delay((arr1[90]-look1)*10);
          stopRobot();
          moving+=arr1[90]-look1;
          while((look1<=30)||(look2<=30)){//если цель будет слишком близко, то сработает сирена
            panic();
            Serial.print("ОПАСНОСТЬ НЕМИНУЕМА\n");
          }
        }
        goBack();//возврат в исходное состояние
        delay(moving*10);
        stopRobot();
        goto AGAIN;//возврат к анализу территории
      }
      if((arr2[angle]-look2)>delta){//для второго датчика
        turn=angle;
        break;
        Serial.print("ЗАМЕЧЕН НАРУШИТЕЛЬ\n");
        mp3_play();
        delay(7000);
        for(turn; turn<=90;turn++){
          servo.write(turn);
          delay(5);
        }
        goRight();
        delay((90-turn)*10);
        stopRobot();
        while((moving<=300)||(look2<=300)){
          checkDistance();
          arr2[90]=look2;
          checkDistance();
          goBack();
          delay((arr2[90]-look2)*10);
          stopRobot();
          moving+=arr2[90]-look2;
          while((look1<=30)||(look2<=30)){
            panic();
            Serial.print("ОПАСНОСТЬ НЕМИНУЕМА\n");
          }
        }
        goAhead();
        delay(moving*10);
        stopRobot();
        goto AGAIN;
      }
    }
    
    delay(100);
    for(angle=0;angle<=180;angle++){//поворот «головы» налево
      servo.write(angle);
      checkDistance();
      delay(5);
      if((arr1[angle]-look1)>delta){
        turn=angle;
        break;
        Serial.print("ЗАМЕЧЕН НАРУШИТЕЛЬ\n");
        mp3_play();
        delay(7000);
        if(turn<90){
        for(turn; turn<=90;turn++){
          servo.write(turn);
          delay(5);
        }
        goRight();
        delay((90-turn)*10);
        stopRobot();
        while((moving<=300)||(look1<=300)){
          checkDistance();
          arr1[90]=look1;
          checkDistance();
          goAhead();
          delay((arr1[90]-look1)*10);
          stopRobot();
          moving+=arr1[90]-look1;
          while((look1<=30)||(look2<=30)){
            panic();
            Serial.print("ОПАСНОСТЬ НЕМИНУЕМА\n");
          }
        }
        goBack();
        delay(moving*10);
        stopRobot();
        goto AGAIN;
        }
        else{
          for(turn; turn<=90;turn--){
          servo.write(turn);
          delay(5);
          }
        goLeft();
        delay((turn-90)*10);
        stopRobot();
        while((moving<=300)||(look1<=300)){
          checkDistance();
          arr1[90]=look1;
          checkDistance();
          goAhead();
          delay((arr1[90]-look1)*10);
          stopRobot();
          moving+=arr1[90]-look1;
          while((look1<=30)||(look2<=30)){
            panic();
            Serial.print("ОПАСНОСТЬ НЕМИНУЕМА\n");
          }
        }
        goBack();
        delay(moving*10);
        stopRobot();
        goto AGAIN;
        }
      }
      if((arr2[angle]-look2)>delta){
        turn=angle;
        break;
        Serial.print("ЗАМЕЧЕН НАРУШИТЕЛЬ\n");
        mp3_play();
        delay(7000);
        if(turn<90){
        for(turn; turn<=90;turn++){
          servo.write(turn);
          delay(5);
        }
        goRight();
        delay((90-turn)*10);
        stopRobot();
         while((moving<=300)||(look2<=300)){
          checkDistance();
          arr2[90]=look2;
          checkDistance();
          goBack();
          delay((arr2[90]-look2)*10);
          stopRobot();
          moving+=arr2[90]-look2;
          while((look1<=30)||(look2<=30)){
            panic();
            Serial.print("ОПАСНОСТЬ НЕМИНУЕМА\n");
          }
        }
        goAhead();
        delay(moving*10);
        stopRobot();
        goto AGAIN;
      }
      else{
      for(turn; turn<=90;turn--){
          servo.write(turn);
          delay(5);
        }
        goLeft();
        delay((turn-90)*10);
        stopRobot();
         while((moving<=300)||(look2<=3000)){
          checkDistance();
          arr2[90]=look2;
          checkDistance();
          goBack();
          delay((arr2[90]-look2)*10);
          stopRobot();
          moving+=arr2[90]-look2;
          while((look1<=30)||(look2<=30)){
            panic();
            Serial.print("ОПАСНОСТЬ НЕМИНУЕМА\n");
          }
        }
        goAhead();
        delay(moving*10);
        stopRobot();
        goto AGAIN; 
      }
    }
    }
  }
/**********************************************************************/
}
