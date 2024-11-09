# RAPSBERRY pico

# Zapojení

**Každý senzor na samostatném GPIO.**

Takže mě už nezajímají adresy čidel - prostě první čidlo (GPIO1) je koupelna a je to tak vždy.

Komunikace **1W** využívá aktivní čekání, takže vyčtení teploty, zahájení měření teploty atd. trvá nějakou (ne malou) dobu. Proto je kód s komunikací **1W** umístěn do druhého jádra procesoru.

Vyčtení teploty a počet senzorů připojených na sběrnici (na jedé sběrnici by správně měl být připojen max. 1 senzor) musí být chráněno mutexem protože přistupuji z jiného procesoru.

Skenování senzorů je prováděno na začátku a potom periodicky (10min).

Měření je prováděno každých 10s.


Komunikace modbusem je potom v hlavním vlákně.


## Propojení čidel teplot

| T   | GPIO  |
|-----|:-----:|
| T1  | GPIO28|
| T2  | GPIO27|
| T3  | GPIO26|
| T4  | GPIO18|
| T5  | GPIO17|
| T6  | GPIO16|
| T7  | GPIO13|
| T8  | GPIO12|
| T9  | GPIO06|
| T10 | GPIO05|
| T11 | GPIO04|
| T12 | GPIO03|

## Adresace

| A   | GPIO  |
|-----|:-----:|
| A0  | GPIO15|
| A1  | GPIO14|
| A2  | GPIO19|
| A3  | GPIO20|
| A4  | GPIO21|
| A5  | GPIO22|


## Kominikace

| Fce | GPIO  |
|-----|:-----:|
| RX  | GPIO01|
| TX  | GPIO00|
| EN  | GPIO02|

## Mqtt
Pokud je použit Raspberry pico "W" musí se nastavit v CmakeList.txt **set(PICO_BOARD "pico_w")**

Potom se publikuje do MQTT serveru ve tvaru:

- Topic: *picoTemp*
- Payload: json *{"T_GPIO: temp",..... ,"signal": wifi signal}*


## Schéma desky

![zapojeni](doc/zapojeniRJ45.png)

|Vodič|**modbus**|**Čidla DS18b20**|
|-|-----|-|
|1| VDD | VDD|
|2| VDD | VDD|
|3|  -  |  x |
|4|485-A|Data|
|5|485-B|Data|
|6|  -  |  x |
|7| GND | GND|
|8| GND | GND|

[eagle schéma](flash.sh)


## One wire:
Kod převzat z projektu [zde](https://github.com/adamboardman/pico-onewire/tree/main)



