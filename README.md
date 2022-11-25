# tp_actionneur_et_automatique_appliquee

La documentation peut être trouvée [ici](https://lucas-vallery.github.io/tp_actionneur_et_automatique_appliquee/html/)

## Table des matières

- [Introduction](#introduction)
- [Configuration du microcontrôleur](#Configuration-du-microcontrôleur)
- [TP1 - Commande MCC Classique](#TP1---commande-mcc-classique)
- [TP2 - Mesure de vitesse et de courant](#tp2---mesure-de-vitesse-et-de-courant)
- [Conclusion](#conclusion)

## Introduction

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

## TP1 - Commande MCC Classique

## TP2 - Mesure de vitesse et de courant

## Conclusion
