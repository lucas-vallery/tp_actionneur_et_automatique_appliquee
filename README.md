# tp_actionneur_et_automatique_appliquee

La documentation peut être trouvée [ici](https://lucas-vallery.github.io/tp_actionneur_et_automatique_appliquee/html/)

## Table des matières

- [Introduction](#introduction)
- [Configuration du microcontrôleur](#Configuration-du-microcontrôleur)
- [Console UART](#Console-UART)
- [TP1 - Commande MCC Classique](#TP1---commande-mcc-classique)
- [TP2 - Mesure de vitesse et de courant](#tp2---mesure-de-vitesse-et-de-courant)
- [Conclusion](#conclusion)

## Introduction

L'objectif de ces 3 séances de TP était de réaliser : 
- un shell pour commander le hâcheur
- la commande des 4 transistors du hâcheur en commande complémentaire décalée
- l'acquisition des différents capteurs

## Configuration du microcontrôleur

Le microncontrôleur est configuré comme suit :

* L'horloge HCLK est configurée à 170 Mhz. Les bus APBx péripheriques et APBx timers sont configuré à 170 Mhz.
* Le *timer* 1 est utilisé pour générer les PWMs de commande :
  * *Prescalar* à 2
  * *Counter Mode* à *Center Aligned mode 1*
  * *Counter Period* à 1750
  * *auto-reload preload* à *Enable*
  * Génére un *Update Event*
  * *Dead Time* à 203
  * Le pulse des deux *channels* des PWMs à 875 (alpha = 50%)
  * Les PWMs sont générées sur les broches suivantes :
    |*Channel*  |Broche   |
    |-----------|---------|
    |CH1        |PA8      |
    |CH2        |PA9      |
    |CH1N       |PA11     |
    |CH2N       |PA12     |
  * Tous les autres paramètres sont laissées par défaut
* Le *timer* 3 est utilisé en *Encode Mode*
  * *Encoder Mode* à *Encoder Mode TI1 and TI2*
  * Tous les autres paramètres sont inchangés
* Le *timer* 4 génére des interruptions qui nous permettent de mesurer la vitesse de rotation du moteur
  * *Prescalar* à 170
  * *Counter Period* à 1000
* L'ADC1 est configuré pour fonctionner avec le DMA
  * *DMA Continuous Request* à *Enable*
  * *External Trigger Conversion Source* en *Timet 1 Trigger Out event*
  * Les autres paramètres ne sont pas changés 
* Enfin, le DMA1 est configuré pour fonctionner en mode *Circular* avec un *Data Width" d'un mots

## Console UART

En amont de notre travail sur le moteur, nous avons dû améliorer le shell déjà existant afin d'implémenter les fonctions de base telles que start, stop, pinout et help. 
Nous avons déclaré en variables globales nos chaînes de caractères.

```c
const uint8_t prompt[]="user@Nucleo-STM32G431>>";
uint8_t started[]=
		"\r\n*-----------------------------*"
		"\r\n| Welcome on Nucleo-STM32G474 |"
		"\r\n*-----------------------------*"
		"\r\n";
const uint8_t newline[]="\r\n";
const uint8_t cmdNotFound[]="Command not found\r\n";
const uint8_t startmsg[] = "Power ON\r\n";
const uint8_t stopmsg[] = "Power OFF\r\n";
const uint8_t restartmsg[] = "Restarting...\r\nPower ON\r\n";

const uint8_t help[6][32]=
{
		"set <pin> <state>\r\n",
		"get <value name>\r\n",
		"start\r\n",
		"stop\r\n",
		"pinout\r\n",
		"restart\r\n"
};

const uint8_t pinoutmsg[7][64]=
{
		"Pinout\r\n",
		" ------------------\r\n"
		"| PA8  | TIM1_CH1  |\r\n",
		"| PA9  | TIM1_CH2  |\r\n",
		"| PA11 | TIM1_CH1N |\r\n",
		"| PA12 | TIM1_CH2N |\r\n",
		" ------------------\r\n"
};
```
Puis on réalise des conditions sur nos commandes dans le while(1). Voici, ci-dessous l'exemple pour l'écriture dans la console de la commande start et stop.

```c
else if(strcmp(argv[0],"start")==0) {
	chopper_start();

	shell.serial.transmit((uint8_t*)startmsg, sizeof(startmsg), HAL_MAX_DELAY);
}
else if(strcmp(argv[0],"stop")==0) {
	chopper_stop();

	shell.serial.transmit((uint8_t*)stopmsg, sizeof(stopmsg), HAL_MAX_DELAY);
}
```


## TP1 - Commande MCC Classique

Dans un premier temps, il a fallu générer 4 PWMs en complémentaire décalée pour contrôler le moteur en boucle ouverte. 
Le cahier des charges est le suivant : 
- Fréquence de la PWM : 16kHz
- Temps mort minimum : 2us
- Résolution minimum : 10bits

Voici ce que nous observons à l'oscilloscope : 



## TP2 - Mesure de vitesse et de courant

## Conclusion
