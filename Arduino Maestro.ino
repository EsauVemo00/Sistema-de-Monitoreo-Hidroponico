//PROGRAMA PARA EL ARDUINO MAESTRO

//Angel Parra Arianna - Vela Mosqueda Esaú
//S18003537 - S18003542

//Ingeniería Mecatrónica

//Librerías, pines y variables
//RTC
#include<virtuabotixRTC.h>
virtuabotixRTC myRTC(5, 4, 2);                                //Pines digitales en arduino: 5-Clock, 4-Data, 2-SCR
int day, month, year, hours, minutes, seconds;                //Variables de la fecha

//DHT11 (Temperatura y Húmedad)
#include<DHT.h>
#define DHTPIN 8                                              //Pin digital de arduino del DHT
#define DHTTYPE DHT11                                         //Tipo de módulo ocupado
DHT dht(DHTPIN, DHTTYPE);                                     //Define el pin 8 y DHT11 como módulo
int humedad, temperatura;                                     //Variables del DHT11

//HCSR04 (Sensor ultrasónico para volumen)
int TRIG = 10;                                                //Pin digital usado de trigger
int ECO = 9;                                                  //Pin digital usado de Echo
int TIEMPO, DISTANCIA, Volumen;                               //Variables del sensor ultrasónico
int AlturaTotal = 20;                                         //Altura total del contenedor en cm
float Radio = 13.5 / 2;                                       //Radio obtenido del diametro del contenedor
float Altura;


//  (Módulo de pH)
int pH = A1;                                                  //Pin analógico módulo pH
float Volt, pHVolt;                                           //Variables para modulo de pH
int pHValor;

void setup() {
  Serial.begin(4800);                                         //Inicializa la velocidad del Serial en Baudios
  /*
     Para ajustar la fecha actual se tiene que seguir el siguiente formato
     Segundos, minutos, hora, dia de la semana, día del mes, mes, año
     myRTC.setDS1302Time(00,56,19,7,2,1,2022);
  */
  dht.begin();                                                //Inicialización del modulo DHT11
  pinMode(TRIG, OUTPUT);                                      //Pin inicializado como salida (HCSR04)
  pinMode(ECO, INPUT);                                        //Pin inicializado como entrada (HCSR04)
  pinMode(pH, INPUT);                                         //Pin inicializado como entrada (Módulo pH)
}

void loop() {
  DHT11F();                                                   //Llama a la función del sensor DHT11
  HCSR04F();                                                  //Llama a la función del sensor HCSR04
  pHSensorF();                                                //Llama a la función del sensor de pH
  ActualDateF();                                              //Llama a la función del módulo RTC1302
  delay(1000);
}

//Función creada para el sensor de humedad y temperatura
void DHT11F()
{
  humedad = dht.readHumidity();                               //Lee la humedad
  temperatura = dht.readTemperature();                        //Lee la temperatura en °C
  if (isnan(humedad) || isnan(temperatura))                   //Pregunta si la lectura es correcta para continuar
  {
    return;
  }
  Serial.write(humedad + 10);                                 //Envia los valores al Arduino esclavo
  Serial.write(temperatura - 4);                              //Envia los valores al Arduino esclavo
}

//Función creada para calcular el volumen del la sustancia
void HCSR04F()
{
  digitalWrite(TRIG, HIGH);                                   //Realiza un pulso para el sensor ultrasonico
  digitalWrite(TRIG, LOW);
  TIEMPO = pulseIn(ECO, HIGH);                                //Lee el tiempo en que regresa la onda sonara al sensor
  DISTANCIA = TIEMPO / 58.2;                                  //Calcula la distancia del objeto mediante una formula proporcionada por el proveedor
  Altura = AlturaTotal - DISTANCIA;                           //Restando la altura total con la distancia que medidmos obtenemos la altura actual del liquido
  if (Altura > 0)                                             //Verifica que no envie datos erroneos
  {
    Volumen = PI * Radio * Radio * Altura-250;                //Calcula el volumen con la altura obtenida
  }
  else
  {
    Volumen = Volumen;                                        //Si llegará a mandar una señal erronea se mantendrá con el valor anterior
  }
  Serial.write(Volumen / 10);                                 //Envia los valores al Arduino esclavo
}

//Función para el sensor de pH
void pHSensorF()
{
  Volt = analogRead(pH);                                      //Calcula el voltaje recibido por el sensor entre 0 y 5
  pHVolt = Volt * 5.0 / 1024;                                 //Aqui conseguimoms que nuestro valor sea entre 0 y 1023
  pHValor = -5.70 * pHVolt + 22.34;                           //Formula proporcionada por el fabricante
  Serial.write(pHValor);                                      //Envia los valores al Arduino esclavo
}

//Función para la hora y fecha actual
void ActualDateF()
{
  myRTC.updateTime();                                         //Aqui el modulo RTC se actualiza
  day = myRTC.dayofmonth;                                     //En las sigueintes 6 lineas se lee y almacena los valores de fecha y hora actual
  month = myRTC.month;
  year = myRTC.year;
  hours = myRTC.hours;
  minutes = myRTC.minutes;
  seconds = myRTC.seconds;
  Serial.write(day);                                          //Envia los valores al Arduino esclavo
  Serial.write(month);                                        //Envia los valores al Arduino esclavo
  Serial.write(year-2000);                                    //Envia los valores al Arduino esclavo
  Serial.write(hours);                                        //Envia los valores al Arduino esclavo
  Serial.write(minutes);                                      //Envia los valores al Arduino esclavo
  Serial.write(seconds);                                      //Envia los valores al Arduino esclavo
}
