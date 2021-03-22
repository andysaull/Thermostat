#pragma once

//#include <Adafruit_TFTLCD.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
//Define las patillas de lectura del tactil
#define YP A3  // LCD_CS
#define XM A2  // LCD_RS
#define YM 9   // LCD_D1
#define XP 8   // LCD_D0

//Define el rango donde puede leer el tactil
#define LEFT	890		//190		//150		//907
#define TOP		90		//200		//120		//76
#define RT		90		//950		//920		//90
#define BOT		915		//915		//940		//915
//Define la presion minima y la maxima para saber que estamos tocando el tactil
#define MINPRESION 10
#define MAXPRESION 1000

//Para una mejor precision del tactil medimos la resistencia entre X+ y X- y la medida obtenida la ponemos en el ultimo numero
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

unsigned short X;
unsigned short Y;
unsigned short Z;

//Define las patillas del LCD
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4 //Esta patilla originalmente en el LCD esta en el pin A4. Dado que necesitamos ese pin para leer el otro potenciometro, se ha hecho un puente con wrapping llevandolo hasta el pin 12

//Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// Asigna nombres a los colores en 16-bits para tener facilidad a la hora de usar los colores
#define NEGRO     0x0000    //0
#define AZUL      0x001F    //1
#define ROJO      0xF800    //2
#define VERDE     0x1EE3    //3
#define CYAN      0x07FF    //4
#define MAGENTA   0xF81F    //5
#define AMARILLO  0xFFE0    //6
#define BLANCO    0xFFFF    //7
#define NARANJA   0xFD20    //8
#define MORADO    0xB93F    //9
#define AZULPLAN  0x2D1E    //10
#define MARRON    0x60A2    //11
#define ROJOSCURO 0xE800    //12
#define ROSA	  0xFB53
#define GRIS	  0x94B2


//Definiciones y variables para el sensor de temperatura y humedad

#define DHTTYPE DHT22
const int DHTPin = 26;
DHT dht(DHTPin, DHTTYPE);
byte hum;
float temp;
float tempMin;
float tempMax;
bool cambioTempMinMax = false;

//TEMAS

typedef enum {TEMA_NEGRO, TEMA_ROJO, TEMA_AZUL, TEMA_ROSA, TEMA_BLANCO} tColorTema;
tColorTema temaEnum;

typedef struct
{
	unsigned int fondo;
	unsigned int texto;
	unsigned int cuadro;
	unsigned int grafica;
	unsigned int grafica2;
	unsigned int graficaAjusteTemp;
} tTema;

tTema tema;

//PANTALLAS

typedef enum {INICIAL, GRAFICA, TEMPERATURA, AJUSTES, DIA_IMPORTANTE} tPantalla;

typedef enum {INICIAL_TEMPERATURA, TEMPERATURA_AJUSTE, TEMPERATURA_HORARIO, TEMPERATURA_AJUSTE_CALENDARIO, TEMPERATURA_AVANZADO} tPantallaTemperatura;

typedef enum {INICIAL_AJUSTES, AJUSTE_TEMA, AJUSTE_FECHA, AJUSTE_AVANZADO, AJUSTE_AVANZADO_GRAFICA} tPantallaAjuste;

tPantalla pantallaEst = INICIAL;
tPantallaTemperatura pantallaTemperaturaEst = INICIAL_TEMPERATURA;
tPantallaAjuste pantallaAjusteEst = INICIAL_AJUSTES;

//VARIABLES PARA MOSTRAR UNA FECHA IMPORTANTE

byte diaAnterior = 0;

int colorR = 0;
int colorG = 0;
int colorB = 0;			//Variable que usaremos para incrementarla poco a poco y hacer que cambie el color de fondo de texto
int estadosColor = 0;

bool joke = false;			//xd

//VARIABLES DE GRAFICA

unsigned short grafX = 32;	//Variable que se incrementa desde 32 hasta 320 (pixeles de la pantalla) para ir mostrando la grafica
unsigned short grafY;		//Variable donde mapearemos los datos de temperatura en pixeles para mostrar en la pantalla

bool calderaGuardado[288];	//Array donde guardamos la activacion de la caldera
short int tempGuardado[288];//Array donde guardamos la temperatura (tama�o de 288 porque son los pixeles que se pueden mostrar en la pantalla)

int TIEMPO_GRAFICA = 2083;	//Tiempo que tarda en incrementarse grafX.	[((Minutos de pantalla * 60segundos * 1000ms) / 288pixeles)]

bool fallo = false;

//VARIABLES DE FUNCION TERMOSTATO

bool estadoCaldera = false;
bool estadoCalderaPrevio = false;

bool termostato = false;	//Bool para activar o no la funcion de termostato
float tempAjuste = 25;		//Temperatura que queremos lograr con el termostato
bool histeresisAjuste = true;
float histeresis = 0.5;		//Histeresis para hacer que la caldera no se active muchas veces seguidas
bool ajusteInvertido = false;//Ajuste para que la histeresis este en un punto u otro

//FLAGS Y VARIABLES DE TIEMPO PARA HACER BOTONES VARIABLES CON DELAY

double tiempo = millis();
double tiempo2 = millis();
double tiempo3 = millis();
bool flagTiempo2 = false;
bool flagTiempo3 = false;

//FLAGS DE BOTONES Y VARIABLES TEMPORALES DE PANTALLAS

bool flagBotonRebote = false;

bool flagTexto = false;

//Ajuste de temperatura de termostato
bool termostatoPrev;	//Almacena el estado del termostato antes de darle a OK en la pantalla de ajuste
float tempAjustePrev;	//Almacena la temperatura deseada antes de darle a OK en la pantalla de ajuste
bool flagBotonTermostato = false;	//Almacena el estado del boton para saber si hay que dibujarlo de nuevo o no (para evitar destellos en el boton)

//Ajuste de calendario en pantalla de temperatura
bool horario = false;
bool horarioPrev;
bool flagBotonHorario = false;

//Ajuste de histeresis
bool histeresisAjustePrev = true;
bool flagHisteresisAjuste = false;

bool ajusteInvertidoPrev = false;
bool flagAjusteInvertido = false;

//VARIABLES PARA GUARDAR HORAS DE ACTIVACION DE TERMOSTATO EN FUNCION DE LOS DIAS DE LA SEMANA

typedef struct
{
	byte hora;
	byte minutos;
} tHora;

typedef struct
{
	bool activado;
	tHora encendido;
	tHora apagado;
} tDiaActivacion;

typedef enum {vacio, DOMINGO, LUNES, MARTES, MIERCOLES, JUEVES, VIERNES, SABADO} tDiaSemana;

tDiaActivacion horarioDia[8];		//Array donde, dependiendo del dia de la semana, accedemos a una hora de encendido y apagado	[0 vacio, 1 domingo, 2 lunes, 3 martes...]

int numVel = 0;
