# TP Actionneur et Automatique Appliquée

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
  * *Prescaler* à 2
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
  * Tous les autres paramètres sont laissés par défaut
* Le *timer* 3 est utilisé en *Encode Mode*
  * *Encoder Mode* à *Encoder Mode TI1 and TI2*
  * Tous les autres paramètres sont inchangés
* Le *timer* 4 génére des interruptions qui nous permettent de mesurer la vitesse de rotation du moteur
  * *Prescaler* à 170
  * *Counter Period* à 1000
* L'ADC1 est configuré pour fonctionner avec le DMA
  * *DMA Continuous Request* à *Enable*
  * *External Trigger Conversion Source* en *Timet 1 Trigger Out event*
  * Les autres paramètres ne sont pas changés 
* Enfin, le DMA1 est configuré pour fonctionner en mode *Circular* avec un *Data Width* d'un mot

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
Puis on réalise des conditions sur nos commandes dans le *while(1)*. Voici, ci-dessous l'exemple pour l'écriture dans la console de la commande start et stop.

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

![Commande complémentaire décalée](images/tek00002.png)

On retrouve bien notre fréquence, notre deadtime et notre commande complémentaire décalée.

Pour demarrer notre moteur, il faut mettre la broche GPIO à 1 pendant *STARTING TIME* puis la repasser à 0.
Puis, il faut activer nos PWMs et nos PWNs complémentaires.
Pour stopper le moteur, il faut mettre la vitesse nulle aux deux channels de notre *Timer 1* puis éteindre nos PWMs.
Ci-dessous, les prototypes des deux fonctions : 

```c
void chopper_start(void);
```

```c
void chopper_stop(void);
```

Pour donner une vitesse au moteur il faut écrire :

```bash 
speed XXXX
```
Lorsque l'on exécute cette commande, le code suivant intervient :

```c
else if(strcmp(argv[0],"speed")==0) {
	if(argv[1] != NULL){
		uint16_t speed;
		sscanf(argv[1], "%hd", &speed);
		chopper_speed(speed);
	}
}
```
*chopper_speed* prend en argument la vitesse que l'on veut atteindre, c'est à dire la valeur *XXXX* que l'on a écrite en ligne de commande.
Cette valeur sera comparé dans la fonction à la vitesse que l'on a effectivement.
On choisit un pas *accelStep* permettant d'accélérer ou de ralentir sans faire décrocher le moteur, ce qui est le principal problème que l'on peut avoir.

De plus, dans le cahier des charges, il est indiqué que la vitesse est majorée par la vitesse maximale du moteur. 

```c
int chopper_speed(uint16_t targetSpeed){
	uint16_t currentSpeed = __HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_1);
	uint16_t period = __HAL_TIM_GET_AUTORELOAD(&htim1);
	uint16_t accelStep = 1;

	if(targetSpeed > MAX_SPEED){
		targetSpeed = MAX_SPEED;
	}
	if(targetSpeed < NO_SPEED){
		targetSpeed = NO_SPEED;
	}

	if(targetSpeed < currentSpeed){
			while(currentSpeed - accelStep > targetSpeed){
				currentSpeed -= accelStep;
				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (uint16_t) currentSpeed);
				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, (uint16_t) period-currentSpeed);
				HAL_Delay(10);
			}
			return 0;
	}else {
		while(currentSpeed + accelStep < targetSpeed){
			currentSpeed += accelStep;
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (uint16_t) currentSpeed);
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, (uint16_t) period-currentSpeed);
			HAL_Delay(10);
		}
		return 0;
	}
}
```


## TP2 - Mesure de vitesse et de courant

Dans le but de réaliser un potentiel asservissement en courrant et en vitesse nous devons acquérir les valeurs de ces deux grandeurs.

### Acquisition du courant 

Nous mesurons le courant à l'aide du capteur à effet Hall intégré dans le hacheur. Pour ce faire nous connectons la sortie correspondante du hacheur sur l'ADC1 de la STM32. 

Afin de décharcher le CPU, nous configurons le DMA afin que les mesures de l'ADC soient directement stockée en mémoire. 

L'ADC converti une tension comprise entre 0 et 3,3 V avec une resolution de 12 bits. De plus, le coeeficient de conversion de capteut à effet Hall est de 12 A/V avec un *offset* de 2,5 V. Nous devons donc appliquer la formule suivante à la valeur $x$ issue de l'ADC :

$I = (x{3.3\over 4096} - 2.5)\times12$

Nous avons implémenté la commande suivant pour lire la valeur de courant dans la console :
```bash
user@Nucleo-STM32G431>> get current
current : 1.92 A
```
## Aquisition de la vitesse

Un codeur incrémental est fixé sur l'arbre du moteur. Avec les paramètres de *timer* (*Encoder Mode TI1 and TI2*) que nous avons défini, le codeur a une résolution de 4096 incréments de *timer* par tour.

Nous utilisons un troisième *timer* générant une interruption toute les millisecondes. Quand une interruption est générée nous mesurons le nombre de *tick* de *timer* générée depuis la dernière interruption. 
```c
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	if(htim->Instance == TIM4){
		previousCNT = currentCNT;
		currentCNT = TIM3->CNT;
		diff = -(currentCNT-previousCNT);
	}
}
```
Ainsi, nous avons le nombre d'incrément de *timer* en une milliseconde. Sachant qu'il y a 4096 incréments de *timer* par tour de moteur, nous pouvons en déduire la vitesse du moteur en tour par minute avec la formule suivante :

$\Omega = x\times {1000 \over 4096}$

Avec $x$ le nombre d'incréments de *timer* en une milliseconde.

Nous avons implémenté une commande pour lire la vitesse dans le *shell* :

```bash 
user@Nucleo-STM32G431>> get speed
speed : 44 rpm
```

## Conclusion
