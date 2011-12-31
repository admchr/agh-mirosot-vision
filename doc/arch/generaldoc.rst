
Wprowadzenie
------------

Opis rozwiązania
****************

Z rozwiązania wydzielony został element odpowiedzialny za faktyczne obliczenia.
Jest nim biblioteka z interfejsem w C, w której znajduje się funkcja 
rozpoznająca zawartość obrazu. Funkcja ta korzysta wyłącznie z bitmapy obrazu
i pewnych danych na temat jego zawartości (ilość robotów, brzegi boiska, ...)
wprowadzanych na samym początku przez użytkownika. Operację taką można 
nazwać funkcją w matematycznym sensie.

.. math:: 

    f_{\mathrm{config}}: M_{n\times m} \to (\mathrm{\vec{Pos}_{blue}},\mathrm{\vec{Pos}_{yellow}})

Istnieje możliwość używania dodatkowych informacji na temat przeszłych zdarzeń
do wspomagania rozpoznawania, ale w obecnym rozwiązaniu nie są one 
wykorzystywane.

Drużyny przeciwnika
^^^^^^^^^^^^^^^^^^^



Sklejanie się robotów
^^^^^^^^^^^^^^^^^^^^^



Jasność i balans bieli
^^^^^^^^^^^^^^^^^^^^^^





Zależności
**********

Zależności kodu obliczeniowego skompilowanej biblioteki ograniczają się do 
linkowania z biblioteką OpenCV (http://opencv.willowgarage.com/wiki/). Nagłówki
OpenCV są potrzebne tylko do kompilacji biblioteki, nagłówek ``amv.h`` nie 
zawiera żadnych odniesień do zewnętrznych symboli.

Z OpenCV są wykorzystywane następujące elementy:

* struktura ``cv::Mat``, będąca kontenerem na bitmapy, wraz z operacjami dostępu
  do pikseli, skalowaniem 
* konwersja współrzędnych przestrzeni kolorów z ``BGR`` do ``HSV`` i z powrotem.
* pomocnicze struktury danych jak punkty 2D, dane koloru piksela, prostokątne 
  obszary itp.

Widać, że stopień użycia biblioteki jest dość niewielki i nie stanowiłoby zbyt
wielkiego trudu zastąpienie funkcjonalności OpenCV własnymi implementacjami.
Powodem wykorzystania biblioteki była możliwość wykorzystania 


