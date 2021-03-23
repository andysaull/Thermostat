//Pantalla de 320x240


//#include <registers.h>
//#include <pin_magic.h>
#include <TimeLib.h>
//#include <Adafruit_TFTLCD.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <Adafruit_GFX.h>
#include <DHT.h>
#include "Termostato.h"

#include "Fonts/Org_01.h"
#include "Fonts/FreeSansBold18pt7b.h"
#include "Fonts/FreeSansBold12pt7b.h"

void setup()
{
	pinMode(13, OUTPUT);
	pinMode(31, OUTPUT);  //definir pin 31 como salida de rele
	tft.reset();
	tft.begin(0x9341); //Inicializar el LCD con el driver adecuado
	tft.setRotation(3); //Rotacion horizontal
	dht.begin();
	temaEnum = TEMA_NEGRO;
	ajustarTema(temaEnum, tema);
	time_t t = now();
	ajustarHora(t);
	inicializarArrayGuardado();

	temp = dht.readTemperature();
	tempMin = temp - 5;
	tempMax = temp + 5;
}

void loop()
{
	ajustarTema(temaEnum, tema);
	time_t t = now();
	lecturaSensor();
	registrarTempMaxMin();
	if (temp != 99)
	{
		guardarGraficaIncrementarGrafX();
	}

	if (horario)
	{
		funcionCalendario(t);
	}

	if (termostato)
	{
		funcionTermostato();
	}
	else
	{
		apagarCaldera();
	}

	if (diaAnterior != day(t))
	{
		cambioPantalla(DIA_IMPORTANTE);
	}
	diaAnterior = day(t);

	if (joke)
	{
		tft.setRotation(1);
	}

	switch (pantallaEst)
	{
		case INICIAL:
		{
			pantallaInicial(t);
		}
		break;

		case GRAFICA:
		{
			pantallaGrafica(t);
		}
		break;

		case TEMPERATURA:
		{
			pantallaTemperatura(t);
		}
		break;

		case AJUSTES:
		{
			pantallaAjustes(t);
		}
		break;

		case DIA_IMPORTANTE:
		{
			diaImportante(t);
		}
		break;
	}
}

void ajustarHora(time_t &t)
{
	apagarCaldera();
	tft.fillScreen(tema.fondo);
	byte hora = hour(t);
	byte minuto = minute(t);
	byte dia, mes;
	int ano;
	if (day(t) == 0)
	{
		dia = 1;
	}
	else
	{
		dia = day(t);
	}

	if (month(t) == 0)
	{
		mes = 1;
	}
	else
	{
		mes = month(t);
	}

	if (year(t) == 1970)
	{
		ano = 2018;
	}
	else
	{
		ano = year(t);
	}

	tft.setTextColor(tema.texto, tema.cuadro);
	tft.setTextSize(2);
	tft.setCursor(35, 0);
	tft.print("Ajusta la hora actual");
	tft.setCursor(85, 100);
	tft.print("Hora");
	tft.setCursor(190, 100);
	tft.print("Min");

	while (!botonOk())
	{
		tft.setTextSize(5);
		tft.setTextColor(tema.texto, tema.cuadro);
		tft.setCursor(80, 120);
		numeroCero(hora);
		tft.setCursor(180, 120);
		numeroCero(minuto);

		if (botonSubeBaja(110, 60, true))
		{
			hora++;
			if (hora > 23)
			{
				hora = 0;
			}
		}
		if (botonSubeBaja(110, 200, false))
		{
			hora--;
			if (hora > 23)
			{
				hora = 23;
			}
		}
		if (botonSubeBaja(210, 60, true))
		{
			minuto++;
			if (minuto > 59)
			{
				minuto = 0;
			}
		}
		if (botonSubeBaja(210, 200, false))
		{
			minuto--;
			if (minuto > 59)
			{
				minuto = 59;
			}
		}
	}

	flagTexto = false;

	tft.fillScreen(tema.fondo);
	setTime(hora, minuto, 0, dia, mes, ano);
	delay(100);

	tft.setTextColor(tema.texto, tema.cuadro);
	tft.setTextSize(2);
	tft.setCursor(32, 0);
	tft.print("Ajusta la fecha actual");
	tft.setCursor(10, 100);
	tft.print("Dia");
	tft.setCursor(113, 100);
	tft.print("Mes");
	tft.setCursor(240, 100);
	tft.print("Ano");
	tft.drawLine(252, 102, 261, 102, tema.texto);

	while (!botonOk())
	{
		tft.setTextSize(5);
		tft.setTextColor(tema.texto, tema.cuadro);
		tft.setCursor(0, 120);
		numeroCero(dia);
		tft.setCursor(100, 120);
		numeroCero(mes);
		tft.setCursor(199, 120);
		numeroCero(ano);

		if (botonSubeBaja(30, 60, true))
		{
			dia++;
			if (dia > 31)
			{
				dia = 1;
			}
		}
		if (botonSubeBaja(30, 200, false))
		{
			dia--;
			if (dia < 1)
			{
				dia = 31;
			}
		}
		if (botonSubeBaja(130, 60, true))
		{
			mes++;
			if (mes > 12)
			{
				mes = 1;
			}
		}
		if (botonSubeBaja(130, 200, false))
		{
			mes--;
			if (mes < 1)
			{
				mes = 12;
			}
		}
		if (botonSubeBaja(230, 60, true))
		{
			ano++;
		}
		if (botonSubeBaja(230, 200, false))
		{
			ano--;
		}
	}
	flagTexto = false;
	t = now();
	setTime(hora, minuto, second(t), dia, mes, ano);
}

void ajustarTema(const tColorTema temaEnum, tTema &tema)
{
	switch (temaEnum)
	{
		case TEMA_NEGRO:
		{
			tema.fondo = NEGRO;
			tema.texto = BLANCO;
			tema.cuadro = ROJO;
			tema.grafica = AZULPLAN;
			tema.grafica2 = VERDE;
			tema.graficaAjusteTemp = GRIS;
		}
		break;

		case TEMA_ROJO:
		{
			tema.fondo = ROJO;
			tema.texto = BLANCO;
			tema.cuadro = NEGRO;
			tema.grafica = AZUL;
			tema.grafica2 = VERDE;
			tema.graficaAjusteTemp = tft.color565(30, 0, 0);
		}
		break;

		case TEMA_AZUL:
		{
			tema.fondo = tft.color565(0, 165, 255);
			tema.texto = NEGRO;
			tema.cuadro = BLANCO;
			tema.grafica = ROJOSCURO;
			tema.grafica2 = VERDE;
			tema.graficaAjusteTemp = AZUL;
		}
		break;

		case TEMA_ROSA:
		{
			tema.fondo = tft.color565(255, 108, 160);
			tema.texto = NEGRO;
			tema.cuadro = BLANCO;
			tema.grafica = AZULPLAN;
			tema.grafica2 = VERDE;
			tema.graficaAjusteTemp = MORADO;
		}
		break;

		case TEMA_BLANCO:
		{
			tema.fondo = BLANCO;
			tema.texto = NEGRO;
			tema.cuadro = AZUL;
			tema.grafica = ROJO;
			tema.grafica2 = VERDE;
			tema.graficaAjusteTemp = GRIS;
		}
		break;
	}
}

void ajustarHorario(time_t &t)
{
	fondo();

	tft.setTextColor(tema.texto, tema.cuadro);
	tft.setTextSize(2);
	tft.setCursor(35, 0);
	tft.print("Selecciona los dias");
	tft.setCursor(35, 20);
	tft.print("en los que se encendera");
	tft.setCursor(35, 40);
	tft.print("el termostato");

	do
	{
		for (int i = 0; i < 7; i++)		//Pinta los cuadros de la semana
		{
			if (horarioDia[i + 1].activado)
			{
				tft.drawRect((3 + (i * 45)), 80, 45, 45, tema.cuadro);
				tft.drawRect((4 + (i * 45)), 81, 43, 43, tema.cuadro);
			}
			else
			{
				tft.drawRect((4 + (i * 45)), 81, 43, 43, tema.fondo);
				tft.drawRect((3 + (i * 45)), 80, 45, 45, tema.texto);
			}

			tft.setTextColor(tema.texto);
			tft.setCursor((3 + (i * 45) + 15), 93);
			tft.setTextSize(3);

			switch (i + 1)
			{
				case 1:
				{
					tft.setTextColor(tema.cuadro);
					tft.print("D");
				}
				break;

				case 2:
				{
					tft.print("L");
				}
				break;

				case 3:
				{
					tft.print("M");
				}
				break;

				case 4:
				{
					tft.print("X");
				}
				break;

				case 5:
				{
					tft.print("J");
				}
				break;

				case 6:
				{
					tft.print("V");
				}
				break;

				case 7:
				{
					tft.print("S");
				}
				break;
			}
		}

		while (!pantallaTocada() && !flagBotonRebote)		//Crea una pausa hasta que no tocamos la pantalla
		{
		}

		for (int u = 0; u < 7; u++)		//Nos dice que boton ha sido tocado y activa el dia de la semana correspondiente
		{
			if (zonaTocada((3 + (u * 45)), (3 + ((u + 1) * 45)), 80, (80 + 45)) && (!flagBotonRebote))
			{
				if (!horarioDia[u + 1].activado)
				{
					horarioDia[u + 1].activado = true;
				}
				else
				{
					horarioDia[u + 1].activado = false;
				}
			}
		}

		flagBotonRebote = pantallaTocada();
	} while (!botonOk());

	fondo();

	while (!botonOk())
	{
		tft.setTextColor(tema.texto, tema.cuadro);
		tft.setTextSize(2);
		tft.setCursor(50, 0);
		tft.print("Ajusta la temperatura");

		tft.setTextSize(5);
		tft.setTextColor(tema.texto, tema.cuadro);
		tft.setCursor(100, 100);
		String tempAjusteDec;
		tempAjusteDec = String(tempAjuste, 1);		//Al igual que con la variable termostatoPrev, primero guardamos la temperatura seleccionado en la variable tempAjustPrev, para que el termostato todavia no se active hasta que el usuario no le de al OK
		tft.print(tempAjusteDec);

		if (botonSubeBaja(260, 118, true))
		{
			tempAjuste += 0.1;
			if (!flagTiempo2)
			{
				tiempo2 = millis();
				flagTiempo2 = true;
			}

			if ((millis() - tiempo2) > 2000)
			{
				tempAjuste += 0.4;

				if ((millis() - tiempo2) > 5000)
				{
					tempAjuste += 0.5;
				}
			}
			if (tempAjuste > 40)
			{
				tempAjuste = 40;
			}
		}
		else
		{
			flagTiempo2 = false;
		}

		if (botonSubeBaja(60, 118, false))
		{
			tempAjuste -= 0.1;
			if (!flagTiempo3)
			{
				tiempo3 = millis();
				flagTiempo3 = true;
			}

			if ((millis() - tiempo3) > 2000)
			{
				tempAjuste -= 0.4;

				if ((millis() - tiempo3) > 5000)
				{
					tempAjuste -= 0.5;
				}
			}
			if (tempAjuste < 10)
			{
				tempAjuste = 10;
			}
		}
		else
		{
			flagTiempo3 = false;
		}

		flagTexto = true;
	}
	fondo();
	flagTexto = false;

	for (int i = 1; i <= 7; i++)
	{
		bool ok = false;		//Bool para pasar de dia cuando pulsamos OK
		byte hora = 0, minuto = 0;
		if (horarioDia[i].activado)
		{
			while (!ok)		//Hasta que no pulsemos OK no salimos del ajuste de encendido
			{
				tft.setTextColor(tema.texto, tema.cuadro);
				tft.setTextSize(2);
				tft.setCursor(0, 0);
				tft.print("Encendido del ");
				mostrarDiaSemana(i);
				tft.setCursor(85, 100);
				tft.print("Hora");
				tft.setCursor(190, 100);
				tft.print("Min");

				tft.setTextSize(5);
				tft.setTextColor(tema.texto, tema.cuadro);
				tft.setCursor(80, 120);
				numeroCero(hora);
				tft.setCursor(180, 120);
				numeroCero(minuto);

				botonSubeBaja(110, 60, true);
				botonSubeBaja(110, 200, false);
				botonSubeBaja(210, 60, true);
				botonSubeBaja(210, 200, false);

				while (pantallaTocada() && !botonOk())
				{
					tft.setTextSize(5);
					tft.setTextColor(tema.texto, tema.cuadro);
					tft.setCursor(80, 120);
					numeroCero(hora);
					tft.setCursor(180, 120);
					numeroCero(minuto);

					if (botonSubeBaja(110, 60, true))
					{
						hora++;
						if (hora > 23)
						{
							hora = 0;
						}
					}
					else if (botonSubeBaja(110, 200, false))
					{
						hora--;
						if (hora > 23)
						{
							hora = 23;
						}
					}
					else if (botonSubeBaja(210, 60, true))
					{
						minuto++;
						if (minuto > 59)
						{
							minuto = 0;
						}
					}
					else if (botonSubeBaja(210, 200, false))
					{
						minuto--;
						if (minuto > 59)
						{
							minuto = 59;
						}
					}
					flagBotonRebote = pantallaTocada();
				}

				if ((botonOk()) && (!flagBotonRebote))
				{
					ok = true;
					horarioDia[i].encendido.hora = hora;
					horarioDia[i].encendido.minutos = minuto;
					fondo();
					//delay(500);
				}
				flagTexto = true;
			}
			
			hora = 0;
			minuto = 0;
			ok = false;
			botonOk();
			flagTexto = false;

			while (!ok)		//Hasta que no pulsemos OK no salimos del ajuste de encendido
			{
				tft.setTextColor(tema.texto, tema.cuadro);
				tft.setTextSize(2);
				tft.setCursor(0, 0);
				tft.print("Apagado del ");
				mostrarDiaSemana(i);
				tft.setCursor(85, 100);
				tft.print("Hora");
				tft.setCursor(190, 100);
				tft.print("Min");

				tft.setTextSize(5);
				tft.setTextColor(tema.texto, tema.cuadro);
				tft.setCursor(80, 120);
				numeroCero(hora);
				tft.setCursor(180, 120);
				numeroCero(minuto);

				botonSubeBaja(110, 60, true);
				botonSubeBaja(110, 200, false);
				botonSubeBaja(210, 60, true);
				botonSubeBaja(210, 200, false);

				while (pantallaTocada() && !botonOk())
				{
					tft.setTextSize(5);
					tft.setTextColor(tema.texto, tema.cuadro);
					tft.setCursor(80, 120);
					numeroCero(hora);
					tft.setCursor(180, 120);
					numeroCero(minuto);

					if (botonSubeBaja(110, 60, true))
					{
						hora++;
						if (hora > 23)
						{
							hora = 0;
						}
					}
					else if (botonSubeBaja(110, 200, false))
					{
						hora--;
						if (hora > 23)
						{
							hora = 23;
						}
					}
					else if (botonSubeBaja(210, 60, true))
					{
						minuto++;
						if (minuto > 59)
						{
							minuto = 0;
						}
					}
					else if (botonSubeBaja(210, 200, false))
					{
						minuto--;
						if (minuto > 59)
						{
							minuto = 59;
						}
					}
					flagBotonRebote = pantallaTocada();
				}

				if ((botonOk()) && (!flagBotonRebote))
				{
					ok = true;
					horarioDia[i].apagado.hora = hora;
					horarioDia[i].apagado.minutos = minuto;
					fondo();
					//delay(500);
				}
				flagTexto = true;
			}
			flagTexto = false;
		}
	}
}

void lecturaPanel()
{
	digitalWrite(13, HIGH);
	TSPoint p = ts.getPoint();
	digitalWrite(13, LOW);

	pinMode(XM, OUTPUT);
	pinMode(YP, OUTPUT);

	X = map(p.y, LEFT, RT, 0, 320);	//X e Y invertidos por la rotacion de la pantalla
	Y = map(p.x, BOT, TOP, 0, 240);
	Z = p.z;
}

void lecturaSensor()
{
	hum = dht.readHumidity();
	temp = dht.readTemperature();
	if (hum == 0)
	{
		temp = 99;
	}
}

bool zonaTocada(int Xizq, int Xder, int Ysup, int Yinf)
{
	bool tocado = false;

	lecturaPanel();
	if ((X > Xizq && X < Xder) && (Y > Ysup && Y < Yinf) && (Z > MINPRESION))
	{
		tocado = true;
	}

	return tocado;
}

bool pantallaTocada()
{
	bool panTocado;

	lecturaPanel();

	if (Z > MINPRESION)
	{
		panTocado = true;
	}
	else
	{
		panTocado = false;
	}

	return panTocado;
}

void numeroCero(int numero)		//Saca por pantalla un 0 antes de un numero si es menor que 10
{
	if (numero < 10)
	{
		tft.print("0");
		tft.print(numero);
	}
	else
	{
		tft.print(numero);
	}
}

bool botonSubeBaja(int Xcir, int Ycir, bool signo)		//Dibuja un boton circular en las coordenadas y si signo es 0 dibuja -, 1 dibuja +
{
	bool botonSubeBajaTocado = false;

	botonSubeBajaTocado = zonaTocada((Xcir - 30), (Xcir + 30), (Ycir - 30), (Ycir + 30));

	tft.drawCircle(Xcir, Ycir, 30, tema.cuadro);

	tft.setCursor((Xcir - 12), (Ycir - 17));
	tft.setTextColor(tema.texto);
	tft.setTextSize(5);
	if (signo)
	{
		tft.print("+");
	}
	else
	{
		tft.print("-");
	}

	return botonSubeBajaTocado;
}

bool boton(int Xcua, int Ycua, const char texto[])
{
	bool botonTocado = false;

	tft.drawRoundRect(Xcua, Ycua, 100, 50, 5, tema.cuadro);
	tft.setTextSize(2);
	tft.setTextColor(tema.texto);
	tft.setCursor((Xcua + 10), (Ycua + 18));
	tft.print(texto);

	botonTocado = zonaTocada(Xcua, (Xcua + 100), Ycua, (Ycua + 50));

	return botonTocado;
}

bool botonHor(byte fila, const char texto[])		//Maximo de 4 botones por pantalla
{
	/*Linea dibujada cada 57 pixeles contando desde abajo (La 4 fila se dibuja en 180, la 3 se dibuja en (180 - 57))*/
	bool botonTocado = false;
	tft.setFont(&FreeSansBold18pt7b);
	switch (fila)
	{
		case 1:
		{
			if (!flagTexto)
			{
				tft.drawLine(0, 66, 320, 66, tema.cuadro);
				tft.setTextSize(1);
				tft.setTextColor(tema.texto);
				tft.setCursor(45, 50);
				tft.print(texto);
			}

			botonTocado = zonaTocada(0, 320, 17, 66);
			if (botonTocado)
			{
				tft.fillRect(0, 17, 320, 49, tema.cuadro);
			}
		}
		break;

		case 2:
		{
			if (!flagTexto)
			{
				tft.drawLine(0, 123, 320, 123, tema.cuadro);
				tft.setTextSize(1);
				tft.setTextColor(tema.texto);
				tft.setCursor(45, 107);
				tft.print(texto);
			}

			botonTocado = zonaTocada(0, 320, 67, 123);
			if (botonTocado)
			{
				tft.fillRect(0, 66, 320, 57, tema.cuadro);
			}
		}
		break;

		case 3:
		{
			if (!flagTexto)
			{
				tft.drawLine(0, 180, 320, 180, tema.cuadro);
				tft.setTextSize(1);
				tft.setTextColor(tema.texto);
				tft.setCursor(45, 164);
				tft.print(texto);
			}
			
			botonTocado = zonaTocada(0, 320, 124, 180);
			if (botonTocado)
			{
				tft.fillRect(0, 123, 320, 57, tema.cuadro);
			}
		}
		break;

		case 4:
		{
			if (!flagTexto)
			{
				tft.setTextSize(1);
				tft.setTextColor(tema.texto);
				tft.setCursor(45, 221);
				tft.print(texto);
			}
			
			botonTocado = zonaTocada(61, 320, 181, 240);
			if (botonTocado)
			{
				tft.fillRect(0, 180, 320, 60, tema.cuadro);
			}
		}
		break;
	}
	tft.setFont();
	return botonTocado;
}

void botonAct(byte fila, bool &activacion, bool &flag, const char textoDes[], const char textoAct[])
{
	/*
	El boton funciona pidiendo primero la fila donde va a ir colocado (del 1 al 4), la variable a activar, y un booleano llamado flag que se va a activar cuando presionemos el boton, y hay que devolverlo a false cuando salgamos de esa pantalla
	Este flag sirve para pintar el fondo del cuadro de un color SOLO UNA VEZ, para que el texto que pongamos encima no parpadee
	*/
	tft.setFont(&FreeSansBold12pt7b);
	switch (fila)
	{
		case 1:
		{
			if (activacion && !flag)
			{
				tft.fillRect(0, 17, 320, 49, tema.cuadro);
				tft.drawLine(0, 66, 320, 66, tema.cuadro);
				tft.setTextSize(1);
				tft.setTextColor(tema.texto, tema.cuadro);
				tft.setCursor(45, 50);
				tft.print(textoAct);
				flag = true;
			}
			else if (!activacion && !flag)
			{
				tft.fillRect(0, 17, 320, 49, tema.fondo);
				tft.drawLine(0, 66, 320, 66, tema.cuadro);
				tft.setTextSize(1);
				tft.setTextColor(tema.texto, tema.fondo);
				tft.setCursor(45, 50);
				tft.print(textoDes);
				flag = true;
			}

			if ((zonaTocada(0, 320, 17, 65)) && (!activacion) && (!flagBotonRebote))
			{
				flag = false;
				activacion = true;
				flagBotonRebote = pantallaTocada();
			}
			else if ((zonaTocada(0, 320, 17, 65)) && (activacion) && (!flagBotonRebote))
			{
				flag = false;
				activacion = false;
				flagBotonRebote = pantallaTocada();
			}
			else if (!pantallaTocada())
			{
				flagBotonRebote = false;
			}
		}
		break;

		case 2:
		{
			if (activacion && !flag)
			{
				tft.fillRect(0, 66, 320, 57, tema.cuadro);
				tft.setTextSize(1);
				tft.setTextColor(tema.texto, tema.cuadro);
				tft.setCursor(45, 107);
				tft.print(textoAct);
				flag = true;
			}
			else if (!activacion && !flag)
			{
				tft.fillRect(0, 67, 320, 57, tema.fondo);
				tft.drawLine(0, 123, 320, 123, tema.cuadro);
				tft.setTextSize(1);
				tft.setTextColor(tema.texto, tema.fondo);
				tft.setCursor(45, 107);
				tft.print(textoDes);
				flag = true;
			}

			if ((zonaTocada(0, 320, 66, 123)) && (!activacion))
			{
				if (!flagBotonRebote)
				{
					flag = false;
					activacion = true;
					flagBotonRebote = pantallaTocada();
				}
			}
			else if ((zonaTocada(0, 320, 66, 123)) && (activacion))
			{
				if (!flagBotonRebote)
				{
					flag = false;
					activacion = false;
					flagBotonRebote = pantallaTocada();
				}
			}
			else if (!pantallaTocada())
			{
				flagBotonRebote = false;
			}
		}
		break;

		case 3:
		{
			if (activacion && !flag)
			{
				tft.fillRect(0, 123, 320, 57, tema.cuadro);
				tft.setTextSize(1);
				tft.setTextColor(tema.texto, tema.cuadro);
				tft.setCursor(45, 164);
				tft.print(textoAct);
				flag = true;
			}
			else if (!activacion && !flag)
			{
				tft.fillRect(0, 123, 320, 57, tema.fondo);
				tft.drawLine(0, 180, 320, 180, tema.cuadro);
				tft.setTextSize(1);
				tft.setTextColor(tema.texto, tema.fondo);
				tft.setCursor(45, 164);
				tft.print(textoDes);
				flag = true;
			}

			if ((zonaTocada(0, 320, 131, 180)) && (!activacion) && (!flagBotonRebote))
			{
				flag = false;
				activacion = true;
			}
			else if ((zonaTocada(0, 320, 131, 180)) && (activacion) && (!flagBotonRebote))
			{
				flag = false;
				activacion = false;
			}
		}
		break;

		case 4:
		{
			if (activacion && !flag)
			{
				tft.fillRect(0, 180, 320, 60, tema.cuadro);
				tft.setTextSize(1);
				tft.setTextColor(tema.texto, tema.cuadro);
				tft.setCursor(45, 221);
				tft.print(textoAct);
				flag = true;
			}
			else if (!activacion && !flag)
			{
				tft.fillRect(0, 180, 320, 60, tema.fondo);
				tft.setTextSize(1);
				tft.setTextColor(tema.texto, tema.fondo);
				tft.setCursor(45, 221);
				tft.print(textoDes);
				flag = true;
			}

			if ((zonaTocada(61, 320, 181, 240)) && (!activacion) && (!flagBotonRebote))
			{
				flag = false;
				activacion = true;
			}
			else if ((zonaTocada(61, 320, 181, 240)) && (activacion) && (!flagBotonRebote))
			{
				flag = false;
				activacion = false;
			}
		}
		break;
	}

	tft.setFont();
	//flagBotonRebote = pantallaTocada();
}

bool botonOk()
{
	bool botonOkTocado = false;

	if (!flagTexto)
	{
		tft.setFont(&FreeSansBold12pt7b);
		tft.drawRect(260, 180, 60, 60, tema.cuadro);
		tft.setCursor(270, 215);
		tft.setTextSize(1);
		tft.setTextColor(tema.texto);
		tft.print("OK");
		tft.setFont();
	}

	botonOkTocado = zonaTocada(260, 320, 180, 240);

	if ((botonOkTocado) && (!flagBotonRebote))
	{
		tft.fillRect(260, 180, 60, 60, tema.texto);
	}
	return botonOkTocado;
}

bool botonAtras()
{
	bool botonAtrasTocado = false;

	tft.drawRect(0, 180, 60, 60, tema.cuadro);
	tft.setCursor(15, 200);
	tft.setTextSize(3);
	tft.setTextColor(tema.grafica);
	tft.print("<");

	botonAtrasTocado = zonaTocada(0, 60, 180, 240);

	if (botonAtrasTocado)
	{
		tft.fillRect(0, 180, 60, 60, tema.grafica);
	}

	return botonAtrasTocado;
}

void mostrarDiaSemana(byte diaSemana)
{
	switch (diaSemana)
	{
		case 1:
		{
			tft.print("Domingo");
		}
		break;

		case 2:
		{
			tft.print("Lunes");
		}
		break;

		case 3:
		{
			tft.print("Martes");
		}
		break;

		case 4:
		{
			tft.print("Miercoles");
		}
		break;

		case 5:
		{
			tft.print("Jueves");
		}
		break;

		case 6:
		{
			tft.print("Viernes");
		}
		break;

		case 7:
		{
			tft.print("Sabado");
		}
		break;
	}
}

void BarraSup(time_t t, const char titulo[])
{
	if (!flagTexto && termostato)
	{
		tft.fillRect(0, 0, 320, 16, tema.texto);
	}

	tft.setTextSize(2);
	if (termostato)
	{
		tft.setTextColor(tema.fondo, tema.texto);
	}
	else
	{
		tft.setTextColor(tema.texto, tema.fondo);
	}
	tft.setCursor(0, 0);
	numeroCero(hour(t));
	tft.print(":");
	if (termostato)
	{
		tft.setTextColor(tema.cuadro, tema.texto);
	}
	else
	{
		tft.setTextColor(tema.cuadro, tema.fondo);
	}
	numeroCero(minute(t));
	tft.print(" ");

	if (titulo == "")
	{
		if (termostato)
		{
			tft.setTextColor(tema.fondo, tema.texto);
		}
		else
		{
			tft.setTextColor(tema.texto, tema.fondo);
		}
		tft.setTextSize(1);
		mostrarDiaSemana(weekday(t));
		tft.print(" ");

		tft.setTextSize(2);
		numeroCero(day(t));
		tft.print("/");
		numeroCero(month(t));
	}
	else
	{
		tft.print(" ");
		tft.print(titulo);

		if (termostato)
		{
			tft.setTextColor(tema.fondo, tema.texto);
		}
		else
		{
			tft.setTextColor(tema.texto, tema.fondo);
		}
	}

	tft.setCursor(200, 0);
	if (temp != 99)
	{
		String temp1Dec;
		temp1Dec = String(temp, 1);
		if ((temp > 0) && (temp < 10))
		{
			tft.print("0");
			tft.print(temp1Dec);
		}
		else
		{
			tft.print(temp1Dec);
		}
	}
	if (termostato)
	{
		tft.drawCircle((tft.getCursorX() + 1), 2, 2, tema.fondo);
	}
	else
	{
		tft.drawCircle((tft.getCursorX() + 1), 2, 2, tema.texto);
	}
	tft.setCursor((tft.getCursorX() + 5), 0);
	tft.print("C-");
	
	if (termostato)
	{
		tft.setTextColor(tema.cuadro, tema.texto);
	}
	else
	{
		tft.setTextColor(tema.cuadro, tema.fondo);
	}

	tft.print(hum);
	tft.print("%");
	tft.drawLine(0, 16, 320, 16, tema.cuadro);
}

void ajusteColores()
{
	tft.drawLine(0, 129, 320, 129, tema.texto);

			//NEGRO
	tft.fillRect(0, 130, 10, 40, NEGRO);
	tft.fillRect(13, 130, 51, 40, NEGRO);
	tft.drawLine(10, 130, 10, 169, ROJO);
	tft.drawLine(11, 130, 11, 169, ROJO);
	tft.drawLine(12, 130, 12, 169, ROJO);

	if (zonaTocada(0, 64, 130, 170))
	{
		temaEnum = TEMA_NEGRO;
		ajustarTema(temaEnum, tema);
		fondo();
		if (termostato)				//La barra superior, si esta el termostato activo hay que pintar el fondo de nuevo.
		{
			tft.fillRect(0, 0, 320, 16, tema.texto);
		}
	}

			//ROJO
	tft.fillRect(64, 130, 10, 40, ROJO);
	tft.fillRect(77, 130, 51, 40, ROJO);
	tft.drawLine(74, 130, 74, 169, tft.color565(255, 128, 0));
	tft.drawLine(75, 130, 75, 169, tft.color565(255, 128, 0));
	tft.drawLine(76, 130, 76, 169, tft.color565(255, 128, 0));

	if (zonaTocada(64, 128, 130, 170))
	{
		temaEnum = TEMA_ROJO;
		ajustarTema(temaEnum, tema);
		fondo();
		if (termostato)				//La barra superior, si esta el termostato activo hay que pintar el fondo de nuevo.
		{
			tft.fillRect(0, 0, 320, 16, tema.texto);
		}
	}

			//AZUL
	tft.fillRect(128, 130, 10, 40, tft.color565(0, 165, 255));
	tft.fillRect(141, 130, 51, 40, tft.color565(0, 165, 255));
	tft.drawLine(138, 130, 138, 169, tft.color565(0, 245, 175));
	tft.drawLine(139, 130, 139, 169, tft.color565(0, 245, 175));
	tft.drawLine(140, 130, 140, 169, tft.color565(0, 245, 175));

	if (zonaTocada(128, 192, 130, 170))
	{
		temaEnum = TEMA_AZUL;
		ajustarTema(temaEnum, tema);
		fondo();
		if (termostato)				//La barra superior, si esta el termostato activo hay que pintar el fondo de nuevo.
		{
			tft.fillRect(0, 0, 320, 16, tema.texto);
		}
	}

			//ROSA
	tft.fillRect(192, 130, 10, 40, tft.color565(255, 108, 160));
	tft.fillRect(205, 130, 51, 40, tft.color565(255, 108, 160));
	tft.drawLine(202, 130, 202, 169, tft.color565(175, 28, 160));
	tft.drawLine(203, 130, 203, 169, tft.color565(175, 28, 160));
	tft.drawLine(204, 130, 204, 169, tft.color565(175, 28, 160));

	if (zonaTocada(192, 256, 130, 170))
	{
		temaEnum = TEMA_ROSA;
		ajustarTema(temaEnum, tema);
		fondo();
		if (termostato)				//La barra superior, si esta el termostato activo hay que pintar el fondo de nuevo.
		{
			tft.fillRect(0, 0, 320, 16, tema.texto);
		}
	}
			//BLANCO
	tft.fillRect(256, 130, 10, 40, tft.color565(255, 255, 255));
	tft.fillRect(269, 130, 51, 40, tft.color565(255, 255, 255));
	tft.drawLine(266, 130, 266, 169, tft.color565(0, 0, 255));
	tft.drawLine(267, 130, 267, 169, tft.color565(0, 0, 255));
	tft.drawLine(268, 130, 268, 169, tft.color565(0, 0, 255));

	if (zonaTocada(256, 320, 130, 170))
	{
		temaEnum = TEMA_BLANCO;
		ajustarTema(temaEnum, tema);
		fondo();
		if (termostato)				//La barra superior, si esta el termostato activo hay que pintar el fondo de nuevo.
		{
			tft.fillRect(0, 0, 320, 16, tema.texto);
		}
	}

	tft.drawLine(0, 170, 320, 170, tema.texto);
}

void nombreColores()
{
	tft.setFont(&Org_01);
	switch (temaEnum)
	{
		case TEMA_NEGRO:
		{
			tft.setTextColor(tema.texto);
			tft.setTextSize(3);
			centerprint("NEGRO", 40);
			centerprint("LAVA", 80);
		}
		break;

		case TEMA_ROJO:
		{
			tft.setTextColor(tema.texto, tema.fondo);
			tft.setTextSize(3);
			centerprint("ROJO", 40);
			centerprint("AMANECER", 80);
		}
		break;

		case TEMA_AZUL:
		{
			tft.setTextColor(tema.texto, tema.fondo);
			tft.setTextSize(3);
			centerprint("AZUL", 40);
			centerprint("PLANETA", 80);
		}
		break;

		case TEMA_ROSA:
		{
			tft.setTextColor(tema.texto, tema.fondo);
			tft.setTextSize(3);
			centerprint("ROSA", 40);
			centerprint("FRANCES", 80);
		}
		break;

		case TEMA_BLANCO:
		{
			tft.setTextColor(tema.texto, tema.fondo);
			tft.setTextSize(3);
			centerprint("BLANCO", 40);
			centerprint("PERLA", 80);
		}
		break;
	}
	tft.setFont();

}

void fondo()
{
	switch (temaEnum)
	{
		case TEMA_NEGRO:
		{
			tft.fillScreen(tema.fondo);
			for (int i = 240; i > 160; i--)
			{
				tft.drawLine(0, i, 320, i, tft.color565((i - 160), 0, 0));
			}
		}
		break;

		case TEMA_ROJO:
		{
			tft.fillScreen(tema.fondo);
			for (int i = 240; i > 160; i--)
			{
				byte u = 240 - i;		//De 0 a 80
				tft.drawLine(0, i, 320, i, tft.color565(255, 80 - u, 0));
			}
		}
		break;

		case TEMA_AZUL:
		{
			tft.fillScreen(tema.fondo);
			for (int i = 240; i > 160; i--)
			{
				byte u = 240 - i;		//De 0 a 80
				tft.drawLine(0, i, 320, i, tft.color565(0, 245 - u, 175 + u));
			}
		}
		break;

		case TEMA_ROSA:
		{
			tft.fillScreen(tema.fondo);
			for (int i = 240; i > 160; i--)
			{
				byte u = 240 - i;		//De 0 a 80
				tft.drawLine(0, i, 320, i, tft.color565(175 + u, 28 + u, 160));
			}
		}
		break;

		case TEMA_BLANCO:
		{
			tft.fillScreen(tema.fondo);
			for (int i = 240; i > 160; i--)
			{
				byte u = 240 - i;		//De 0 a 80
				tft.drawLine(0, i, 320, i, tft.color565(175 + u, 175 + u, 255));
			}
		}
		break;

		default:
		{
			tft.fillScreen(tema.fondo);
			for (int i = 240; i > 180; i--)
			{
				tft.drawLine(0, i, 320, i, tft.color565((i - 180), 0, 0));
			}
		}
		break;
	}
}

void registrarTempMaxMin()
{
	if ((temp > tempMax) && (temp != 99))
	{
		tempMax = temp;
		cambioTempMinMax = true;
	}
	else if (temp < tempMin)
	{
		tempMin = temp;
		cambioTempMinMax = true;
	}
}

void dibujarSaltosGrafica()
{
	/*La grafica dibuja en la vertical, 11 saltos de representacion (es decir, si queremos representar entre 2 y 20, dibuja una raya en 0, 2, 4, 6..., 20
	El valor minimo que representa esta guardado en la variable tempMin, el maximo en tempMax*/

	tft.drawLine(30, 0, 30, 220, tema.cuadro);											//LINEA VERTICAL PRINCIPAL

	for (int i = 0; i <= 220; i = i + 22)												//NUMEROS DE GRAFICA
	{
		float valor2Dec;
		float escalaGrafica;
		String valorRep1Dec;
		tft.drawLine(28, i, 32, i, tema.cuadro);
		tft.setTextSize(1);
		tft.setTextColor(tema.texto, tema.fondo);
		tft.setCursor(0, (220 - i));
		valor2Dec = mapfloat(i, 0, 220, tempMin, tempMax);
		valorRep1Dec = String(valor2Dec, 1);
		tft.print(valorRep1Dec);
		if (i == 198)
		{
			escalaGrafica = valor2Dec;
		}
		else if (i == 220)
		{
			escalaGrafica = valor2Dec - escalaGrafica;
			valorRep1Dec = String(escalaGrafica, 1);
			tft.setCursor(0, 230);
			tft.print("[");
			tft.print(valorRep1Dec);
			tft.print("]");
		}
	}

	tft.drawLine(30, 220, 320, 220, tema.cuadro);										//LINEA HORIZONTAL PRINCIPAL

	if ((tempMax >= tempAjuste) && (tempMin <= tempAjuste) && (termostato))				//LINEA DE AJUSTE
	{
		unsigned short grafAjusteY;
		grafAjusteY = mapfloat(tempAjuste, tempMin, tempMax, 219, 0);

		for (int i = 31; i < 315; i = i + 10)
		{
			tft.drawLine(i, grafAjusteY, i + 5, grafAjusteY, tema.graficaAjusteTemp);
		}
	}

	if (temp == 99)																		//PARA LA GRAFICA SI NO HAY LECTURA CORRECTA
	{
		tft.setCursor(100, 100);
		tft.setTextSize(2);
		tft.setTextColor(tema.texto, tema.fondo);
		tft.print("NO HAY LECTURA");
		fallo = true;		//Sirve para dibujar de nuevo la grafica una vez detecte se�al de sensor
	}

	if (fallo && (temp != 99))
	{
		fondo();
		tft.fillRect(0, 0, 320, 220, tema.fondo);
		cargarGrafica();
		fallo = false;
	}
}

void dibujarGrafica()
{
	if (cambioTempMinMax)
	{
		fondo();
		tft.fillRect(0, 0, 320, 220, tema.fondo);
		cargarGrafica();
		cambioTempMinMax = false;
	}
	grafY = mapfloat(temp, tempMin, tempMax, 219, 0);
	tft.drawLine(grafX, 0, grafX, 219, tema.fondo);
	tft.drawLine((grafX + 1), 0, (grafX + 1), 219, tema.texto);
	if (grafX == 33)
	{
		tft.drawPixel(grafX, grafY, tema.grafica);
		if (estadoCaldera)
		{
			tft.drawPixel(grafX, 154, tema.grafica2);
		}
		else
		{
			tft.drawPixel(grafX, 219, tema.grafica2);
		}
	}
	else
	{
		float tempGuardadoFloat = tempGuardado[((grafX - 33) - 1)] / 10.0;
		unsigned short grafYPrevio = mapfloat(tempGuardadoFloat, tempMin, tempMax, 219, 0);
		tft.drawLine((grafX - 1), grafYPrevio, grafX, grafY, tema.grafica);

		unsigned short grafEstadoCalderaPrevio;
		if (calderaGuardado[(grafX - 33) - 1])
		{
			grafEstadoCalderaPrevio = 154;
		}
		else
		{
			grafEstadoCalderaPrevio = 219;
		}

		if (estadoCaldera)
		{
			tft.drawLine((grafX - 1), grafEstadoCalderaPrevio, (grafX - 1), 154, tema.grafica2);
		}
		else
		{
			tft.drawLine((grafX - 1), grafEstadoCalderaPrevio, (grafX - 1), 219, tema.grafica2);
		}
	}

	tft.setFont(&Org_01);
	tft.fillCircle(55, 230, 3, tema.grafica);
	tft.setCursor(60, 232);
	tft.setTextSize(1);
	tft.setTextColor(tema.grafica, tema.fondo);
	tft.print("- Temperatura");

	tft.fillCircle((tft.getCursorX() + 22), 230, 3, tema.grafica2);
	tft.setTextColor(tema.grafica2, tema.fondo);
	tft.setCursor((tft.getCursorX() + 27), 232);
	tft.print("- Caldera");
	tft.setFont();

	tft.setTextColor(tema.texto, tema.fondo);
	tft.setCursor(280, 227);
	String temp1Dec;
	temp1Dec = String(temp, 1);
	if ((temp > 0) && (temp < 10))
	{
		tft.print("0");
		tft.print(temp1Dec);
	}
	else
	{
		tft.print(temp1Dec);
	}
	tft.drawCircle((tft.getCursorX() + 2), 229, 2, tema.texto);
	tft.setCursor((tft.getCursorX() + 6), 227);
	tft.print("C");
}

void guardarGraficaIncrementarGrafX()
{
	
	if ((millis() - tiempo) > TIEMPO_GRAFICA)
	{
		grafX++;
		tiempo = millis();
	}
	if (grafX == 321)
	{
		grafX = 33;
	}

	tempGuardado[(grafX - 33)] = temp * 10;
	calderaGuardado[(grafX - 33)] = estadoCaldera;
}

void cargarGrafica()
{
	for (int i = 0; i < 288; i++)
	{
		float tempGuardadoFloat = tempGuardado[i] / 10.0;
		grafY = mapfloat(tempGuardadoFloat, tempMin, tempMax, 219, 0);

		if (tempGuardado[i] != 1000)
		{
			if (i == 0)
			{
				tft.drawPixel((i + 33), grafY, tema.grafica);

				if (calderaGuardado[i])
				{
					tft.drawPixel((i + 33), 154, tema.grafica2);
				}
				else
				{
					tft.drawPixel((i + 33), 219, tema.grafica2);
				}
			}
			else
			{
				tempGuardadoFloat = tempGuardado[i - 1] / 10.0;
				unsigned short grafYPrevio = mapfloat(tempGuardadoFloat, tempMin, tempMax, 219, 0);
				tft.drawLine(((i + 33) - 1), grafYPrevio, (i + 33), grafY, tema.grafica);

				unsigned short grafEstadoCalderaPrevio;
				if (calderaGuardado[i - 1])
				{
					grafEstadoCalderaPrevio = 154;
				}
				else
				{
					grafEstadoCalderaPrevio = 219;
				}

				if (calderaGuardado[i])
				{
					tft.drawLine(((i + 33) - 1), grafEstadoCalderaPrevio, ((i + 33) - 1), 154, tema.grafica2);
				}
				else
				{
					tft.drawLine(((i + 33) - 1), grafEstadoCalderaPrevio, ((i + 33) - 1), 219, tema.grafica2);
				}
			}
		}
	}
}

void inicializarArrayGuardado()
{
	for (int i = 0; i < 288; i++)
	{
		tempGuardado[i] = 1000;
	}
}

void encenderCaldera()
{
	digitalWrite(31, HIGH);
	estadoCaldera = true;
}

void apagarCaldera()
{
	digitalWrite(31, LOW);
	estadoCaldera = false;
}

void funcionTermostato()
{
	if (!ajusteInvertido)
	{
		if ((tempAjuste - histeresis) >= temp)
		{
			encenderCaldera();
			estadoCalderaPrevio = true;
		}
		else if ((tempAjuste + histeresis) <= temp)
		{
			apagarCaldera();
			estadoCalderaPrevio = false;
		}
	}
	else
	{
		if ((tempAjuste - histeresis) >= temp)
		{
			encenderCaldera();
			estadoCalderaPrevio = true;
		}
		else if (((tempAjuste - histeresis) < temp) && estadoCalderaPrevio == true)
		{
			apagarCaldera();
		}
		else if ((tempAjuste + histeresis) <= temp)
		{
			apagarCaldera();
			estadoCalderaPrevio = false;
		}
		else if (((tempAjuste + histeresis) > temp) && estadoCalderaPrevio == false)
		{
			encenderCaldera();
		}
	}
}

void histeresisCalculo()
{
	if (histeresisAjuste)
	{
		histeresis = 0.5;
	}
	else
	{
		histeresis = 0;
	}
}

void funcionCalendario(time_t t)
{
	if (horarioDia[weekday(t)].activado)
	{
		if (hour(t) >= horarioDia[weekday(t)].encendido.hora)
		{
			if (minute(t) >= horarioDia[weekday(t)].encendido.minutos)
			{
				termostato = true;
			}
		}

		if (hour(t) >= horarioDia[weekday(t)].apagado.hora)
		{
			if (minute(t) >= horarioDia[weekday(t)].apagado.minutos)
			{
				termostato = false;
			}
		}
	}
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void reinicio()
{
	asm volatile("  jmp 0");
}

void cambioPantalla(tPantalla pantallaCambiada)
{
	fondo();
	pantallaEst = pantallaCambiada;
	flagTexto = false;
}

void cambioPantalla(tPantallaTemperatura pantallaTemperaturaCambiada)
{
	fondo();
	pantallaTemperaturaEst = pantallaTemperaturaCambiada;
	flagTexto = false;
}

void cambioPantalla(tPantallaAjuste pantallaAjusteCambiada)
{
	fondo();
	pantallaAjusteEst = pantallaAjusteCambiada;
	flagTexto = false;
}

void centerprint(const char *s, int y)
{
	int len = strlen(s) * 18;
	tft.setCursor((320 - len) / 2, y);
	tft.print(s);
}


//PANTALLAS

void pantallaInicial(time_t t)
{
	BarraSup(t, "");

	if (botonHor(1, "Grafica"))
	{
		pantallaEst = GRAFICA;
		tft.fillRect(0, 0, 320, 220, tema.fondo);
		cargarGrafica();
	}
	else if (botonHor(2, "Temperatura"))
	{
		cambioPantalla(TEMPERATURA);
		pantallaTemperaturaEst = INICIAL_TEMPERATURA;
	}
	else if (botonHor(3, "Ajustes"))
	{
		cambioPantalla(AJUSTES);
	}

	if (pantallaEst == INICIAL)
	{
		flagTexto = true;
	}
}

void pantallaGrafica(time_t t)
{
	dibujarSaltosGrafica();

	if (temp != 99)
	{
		dibujarGrafica();
	}

	if (pantallaTocada())
	{
		cambioPantalla(INICIAL);
	}
}

void pantallaTemperatura(time_t t)
{
	switch (pantallaTemperaturaEst)
	{
		case INICIAL_TEMPERATURA:
		{
			pantallaTemperatura_Inicial(t);
		}
		break;

		case TEMPERATURA_AJUSTE:
		{
			pantallaTemperatura_Ajuste(t);
		}
		break;

		case TEMPERATURA_HORARIO:
		{
			pantallaTemperatura_Horario(t);
		}
		break;

		case TEMPERATURA_AJUSTE_CALENDARIO:
		{
			pantallaTemperatura_AjusteCalendario(t);
		}
		break;

		case TEMPERATURA_AVANZADO:
		{
			pantallaTemperatura_Avanzado(t);
		}
		break;

		default:
		{
			pantallaTemperatura_Inicial(t);
		}
		break;
	}
}

void pantallaTemperatura_Inicial(time_t t)
{
	BarraSup(t, "TEMPERAT");
	
	if (botonHor(1, "Ajuste"))
	{
		cambioPantalla(TEMPERATURA_AJUSTE);
		termostatoPrev = termostato;
		tempAjustePrev = tempAjuste;
	}
	else if (botonHor(2, "Horario"))
	{
		cambioPantalla(TEMPERATURA_HORARIO);
		horarioPrev = horario;
	}
	else if (botonHor(3, "Avanzado"))
	{
		cambioPantalla(TEMPERATURA_AVANZADO);
		histeresisAjustePrev = histeresisAjuste;
		ajusteInvertidoPrev = ajusteInvertido;
	}

	else if (botonAtras())
	{
		cambioPantalla(INICIAL);
	}

	if ((pantallaTemperaturaEst == INICIAL_TEMPERATURA) && (pantallaEst == TEMPERATURA))
	{
		flagTexto = true;
	}
}

void pantallaTemperatura_Ajuste(time_t t)
{
	BarraSup(t, "TEMPERAT");
	botonAct(1, termostatoPrev, flagBotonTermostato, "Encender termostato", "Apagar termostato");		//Funcion de boton activable
	if (termostatoPrev)		//Si el boton esta activado, muestra el submenu para ajustar la temperatura [PRIMERO ACTIVAMOS LA VARIABLE termostatoPrev Y LUEGO CUANDO EL USUARIO LE DA A OK EN LA PANTALLA, SE CAMBIA LA VARIABLE termostato]
	{
		tft.setTextSize(5);
		tft.setTextColor(tema.texto, tema.cuadro);
		tft.setCursor(100, 100);
		String tempAjustePrevDec;
		tempAjustePrevDec = String(tempAjustePrev, 1);		//Al igual que con la variable termostatoPrev, primero guardamos la temperatura seleccionado en la variable tempAjustPrev, para que el termostato todavia no se active hasta que el usuario no le de al OK
		tft.print(tempAjustePrevDec);

		tft.setFont(&FreeSansBold12pt7b);
		tft.setTextSize(1);
		tft.setTextColor(tema.texto, tema.fondo);
		tft.setCursor(35, 168);
		tft.print("Ajusta la temperatura");
		tft.setFont();
		if (botonSubeBaja(260, 118, true))
		{
			tempAjustePrev += 0.1;
			if (!flagTiempo2)
			{
				tiempo2 = millis();
				flagTiempo2 = true;
			}

			if ((millis() - tiempo2) > 2000)
			{
				tempAjustePrev += 0.4;

				if ((millis() - tiempo2) > 5000)
				{
					tempAjustePrev += 0.5;
				}
			}
			if (tempAjustePrev > 40)
			{
				tempAjustePrev = 40;
			}
		}
		else
		{
			flagTiempo2 = false;
		}

		if (botonSubeBaja(60, 118, false))
		{
			tempAjustePrev -= 0.1;
			if (!flagTiempo3)
			{
				tiempo3 = millis();
				flagTiempo3 = true;
			}

			if ((millis() - tiempo3) > 2000)
			{
				tempAjustePrev -= 0.4;

				if ((millis() - tiempo3) > 5000)
				{
					tempAjustePrev -= 0.5;
				}
			}
			if (tempAjustePrev < 10)
			{
				tempAjustePrev = 10;
			}
		}
		else
		{
			flagTiempo3 = false;
		}
	}
	else
	{
		tft.fillRect(0, 67, 320, 113, tema.fondo);
	}

	if (botonOk())
	{
		cambioPantalla(INICIAL_TEMPERATURA);
		termostato = termostatoPrev;
		tempAjuste = tempAjustePrev;
		flagBotonTermostato = false;
	}
	else if (botonAtras())
	{
		cambioPantalla(INICIAL_TEMPERATURA);
		flagBotonTermostato = false;
	}

	if ((pantallaTemperaturaEst == TEMPERATURA_AJUSTE) && (pantallaEst == TEMPERATURA))
	{
		flagTexto = true;
	}
}

void pantallaTemperatura_Horario(time_t t)
{
	BarraSup(t, "TEMPERAT");

	botonAct(1, horarioPrev, flagBotonHorario, "Activar calendario", "Desactivar calendario");		//Funcion de boton activable

	if (zonaTocada(0, 320, 17, 65))
	{
		flagTexto = false;
	}

	if (horarioPrev)
	{
		if (botonHor(2, "Editar horario"))
		{
			flagBotonHorario = false;
			horario = horarioPrev;

			cambioPantalla(TEMPERATURA_AJUSTE_CALENDARIO);
		}
	}
	else
	{
		tft.fillRect(0, 67, 320, 113, tema.fondo);
	}

	if (botonOk())
	{
		cambioPantalla(INICIAL_TEMPERATURA);
		horario = horarioPrev;
		flagBotonHorario = false;
	}
	if (botonAtras())
	{
		cambioPantalla(INICIAL_TEMPERATURA);
		flagBotonHorario = false;
	}

	if ((pantallaTemperaturaEst == TEMPERATURA_HORARIO) && (pantallaEst == TEMPERATURA))
	{
		flagTexto = true;
	}
}

void pantallaTemperatura_AjusteCalendario(time_t t)
{
	tft.setTextColor(tema.texto);
	tft.setTextSize(2);
	tft.setCursor(0, 50);
	tft.println("Se parara el termostato");
	tft.println("hasta acabar de ajustar");
	tft.println("el horario de");
	tft.println("funcionamiento");


	if (botonOk())
	{
		ajustarHorario(t);
		cambioPantalla(TEMPERATURA_HORARIO);
	}
	else if (botonAtras())
	{
		cambioPantalla(TEMPERATURA_HORARIO);
	}
}

void pantallaTemperatura_Avanzado(time_t t)
{
	BarraSup(t, "TEMPERAT");

	botonAct(1, histeresisAjustePrev, flagHisteresisAjuste, "Activar histeresis", "Desactivar histeresis");		//Funcion de boton activable
	botonAct(2, ajusteInvertidoPrev, flagAjusteInvertido, "Ajuste invertido", "Ajuste normal");

	if (botonOk())
	{
		flagHisteresisAjuste = false;
		flagAjusteInvertido = false;
		histeresisAjuste = histeresisAjustePrev;
		ajusteInvertido = ajusteInvertidoPrev;
		histeresisCalculo();
		cambioPantalla(INICIAL_TEMPERATURA);
	}

	if (botonAtras())
	{
		flagHisteresisAjuste = false;
		flagAjusteInvertido = false;
		cambioPantalla(INICIAL_TEMPERATURA);
	}

	if ((pantallaTemperaturaEst == TEMPERATURA_AVANZADO) && (pantallaEst == TEMPERATURA))
	{
		flagTexto = true;
	}
}

void pantallaAjustes(time_t t)
{
	switch (pantallaAjusteEst)
	{
		case INICIAL_AJUSTES:
		{
			pantallaAjustes_Inicial(t);
		}
		break;

		case AJUSTE_TEMA:
		{
			pantallaAjustes_Tema(t);
		}
		break;

		case AJUSTE_FECHA:
		{
			pantallaAjustes_Fecha(t);
		}
		break;

		case AJUSTE_AVANZADO:
		{
			pantallaAjustes_Avanzado(t);
		}
		break;
	}
}

void pantallaAjustes_Inicial(time_t t)
{
	BarraSup(t, "AJUSTES");

	if (botonHor(1, "Tema"))
	{
		cambioPantalla(AJUSTE_TEMA);
	}
	else if (botonHor(2, "Fecha y hora"))
	{
		cambioPantalla(AJUSTE_FECHA);
	}
	else if (botonHor(3, "Avanzado"))
	{
		cambioPantalla(AJUSTE_AVANZADO);
	}

	else if (botonAtras())
	{
		cambioPantalla(INICIAL);
	}

	if ((pantallaAjusteEst == INICIAL_AJUSTES) && (pantallaEst == AJUSTES))
	{
		flagTexto = true;
	}
}

void pantallaAjustes_Tema(time_t t)
{
	BarraSup(t, "  TEMA");
	
	nombreColores();

	ajusteColores();

	if (botonAtras())
	{
		cambioPantalla(INICIAL_AJUSTES);
	}
	
	if ((pantallaAjusteEst == AJUSTE_TEMA) && (pantallaEst == AJUSTES))
	{
		flagTexto = true;
	}
}

void pantallaAjustes_Fecha(time_t t)
{
	BarraSup(t, "");
	tft.setTextColor(tema.texto);
	tft.setTextSize(2);
	tft.setCursor(0, 70);
	tft.println("Se parara el termostato");
	tft.println("hasta acabar de ajustar");
	tft.print("la fecha");

	if (botonOk())
	{
		ajustarHora(t);
		cambioPantalla(INICIAL_AJUSTES);
	}
	else if (botonAtras())
	{
		cambioPantalla(INICIAL_AJUSTES);
	}
	
	if ((pantallaAjusteEst == AJUSTE_FECHA) && (pantallaEst == AJUSTES))
	{
		flagTexto = true;
	}
}

void pantallaAjustes_Avanzado(time_t t)
{
	BarraSup(t, "AVANZADO");

	if (botonHor(1, "Reinicio total"))
	{
		reinicio();
	}
	if (botonHor(2, "Reinicio grafica"))
	{
		temp = dht.readTemperature();
		tempMin = temp - 5;
		tempMax = temp + 5;
		inicializarArrayGuardado();
		grafX = 33;
		tft.setCursor(80, 215);
		tft.print("GRAFICA REINICIADA");
	}

	if (botonAtras())
	{
		cambioPantalla(INICIAL_AJUSTES);
	}

	if ((pantallaAjusteEst == AJUSTE_AVANZADO) && (pantallaEst == AJUSTES))
	{
		flagTexto = true;
	}
}

void diaImportante(time_t t)
{
	switch (month(t))
	{
		
		case 1:   //Month
		{
			switch (day(t))
			{
				case 1:   //Day
				{
					tft.setTextSize(4);
					tft.setCursor(0, 100);
					tft.setTextColor(tema.texto, tft.color565(colorR, colorG, colorB));
					calculoColor();

					tft.print(" FELIZ ");
					tft.print(year(t));
					tft.print("!!");

					if (botonOk())
					{
						cambioPantalla(INICIAL);
					}
				}
				break;

				default:
				{
					cambioPantalla(INICIAL);
				}
				break;
			}
		}

    //If you want to add more dates, you need to add a case with the month and inside, another case for the day.
    
		default:
		{
			cambioPantalla(INICIAL);
		}
		break;
	}

	if ((month(t) == 5) && (day(t) < 8) && (weekday(t) == 1))
	{
		tft.setTextSize(4);
		tft.setCursor(0, 100);
		tft.setTextColor(tema.texto, tft.color565(colorR, colorG, colorB));
		calculoColor();

		tft.print("FELIZ DIA DE");
		tft.setCursor(80, 140);
		tft.print("LA MADRE");

		if (botonOk())
		{
			tft.fillScreen(ROSA);
			tft.setTextSize(2);
			tft.setTextColor(tema.texto, ROSA);
			tft.setCursor(10, 20);
			tft.print("Pasa un gran dia que te");
			tft.setCursor(10, 40);
			tft.print("mereces por ser la mejor");
			tft.setCursor(10, 60);
			tft.print("madre del MUNDO");
			
			if (botonOk())
			{
				cambioPantalla(INICIAL);
			}
		}
	}
	else if ((month(t) == 5))
	{
		cambioPantalla(INICIAL);
	}
}

void calculoColor()
{
	switch (estadosColor)
	{
		case 0:
		{
			colorR = 255;
			colorG += 51;
			colorB = 0;

			if (colorG == 255)
			{
				estadosColor = 1;
			}
		}
		break;

		case 1:
		{
			colorR -= 51;
			colorG = 255;
			colorB = 0;

			if (colorR == 0)
			{
				estadosColor = 2;
			}
		}
		break;

		case 2:
		{
			colorR = 0;
			colorG = 255;
			colorB += 51;

			if (colorB == 255)
			{
				estadosColor = 3;
			}
		}
		break;

		case 3:
		{
			colorR = 0;
			colorG -= 51;
			colorB = 255;

			if (colorG == 0)
			{
				estadosColor = 4;
			}
		}
		break;

		case 4:
		{
			colorR += 51;
			colorG = 0;
			colorB = 255;

			if (colorR == 255)
			{
				estadosColor = 5;
			}
		}
		break;

		case 5:
		{
			colorR = 255;
			colorG = 0;
			colorB -= 51;

			if (colorB == 0)
			{
				estadosColor = 0;
			}
		}
		break;
	}
}
