
Struktura systemu
*****************

TODO, zależy od tego jak wygląda z tym UDP


Zależności
**********

Zależności kodu obliczeniowego skompilowanej biblioteki ograniczają się do 
linkowania z biblioteką OpenCV (http://opencv.willowgarage.com/wiki/). Nagłówki
OpenCV są potrzebne tylko do kompilacji biblioteki, nagłówek ``amv.h`` nie 
zawiera żadnych odniesień do zewnętrznych symboli.

Z OpenCV są wykorzystywane następujące elementy:

* struktura ``cv::Mat``, będąca kontenerem na bitmapy
* konwersja współrzędnych przestrzeni kolorów z ``BGR`` do ``HSV`` i z powrotem.
* pomocnicze struktury danych jak punkty 2D, dane koloru piksela, prostokątne 
  obszary itp.

