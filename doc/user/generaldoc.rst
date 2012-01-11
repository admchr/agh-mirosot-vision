.. highlight:: c

Wiadomości wstępne
------------------

Opis systemu
************

FIRA MiroSot to dyscyplina sportu, w której drużyny pięciu robotów nie większych
niż 7.5x7.5cm poruszając się na czarnym boisku mają za zadanie przemieścić
pomarańczową piłkę golfową do bramki przeciwnika. Do sterowania robotami używa
się pojedynczego komputera, który mając do dyspozycji obraz z kamery
zamontowanej centralnie nad boiskiem steruje serwomechanizmami znajdującymi się
w robotach. 

Opisywany system jest odpowiedzialny za przetworzenie strumienia 
wideo do postaci współrzędnych piłki i robotów, które mogą być przekazane
dalej, do programów sterujących. 



..  
    TODO

Wymagania systemowe
*******************

Do poprawnego działania aplikacja wymaga zainstalowanych:

* kamery zgodnej z systemem Microsoft Windows
* Microsoft Visual Studio 2008 Redistributable Package
* Microsoft .NET Framework 2.0


..  
    TODO

..  _koszulki:

    
Koszulki zawodników
*******************

W ogólnym przypadku, dla dowolnych koszulek zgodnych z regułami gry nie jest 
możliwe ustalenie kąta obrotu i identyfikacja poszczególnych robotów. 
Dla uzyskania takich danych konieczne jest zdecydowanie się na wygląd koszulki, 
co z kolei determinuje metody określania kąta i identyfikacji robota. 

Algorytmy rozpoznawania w programie są dostosowane do jednego schematu barwnego 
na robotach. Jest on przedstawiony na rysunku:

.. image:: /team.png
    :align: center
    :width: 320pt
    :height: 240pt

Koszulka składa się z czarnej ramki i 3 kolorowych trójkątów. Największy jest
kolorem drużyny (żółty lub niebieski, zgodnie z regułami). Pozostałe dwa obszary
mogą mieć dowolne dwa różne kolory, ale nie niebieski, żółty ani pomarańczowy.
Względy praktyczne sugerują, że mają to być kolory o jasności zbliżonej do
koloru drużyny, różniące się maksymalnie barwą, np. czerwony, zielony i fioletowy.

.. _hsl:

Przestrzeń kolorów HSL
**********************

Opis koloru w przestrzeni HSL składa się z 3 liczb:

* **H(Hue)** - barwa punktu, określa najbliższy w pełni nasycony kolor, jaki można
  przyporządkować danemu pikselowi. Wartości barwy są z zakresu od 0 do 180 i są
  wartościami cyklicznymi (tzn. :math:`0 = 180`). Poniżej znajduje się wykres 
  barw dla w pełni nasyconych kolorów.

.. image:: /rainbow.png
    :width: 140pt
    :height: 30pt

* **S(Saturation)** - nasycenie. Wartość 0 oznacza brak barwy - kolor 
  biały/szary/czarny. Wartość 255 to kolor w pełni nasycony.

* **L(Lightness)** - jasność. Wartość 0 to kolor czarny, 255 to kolor biały. 
  Kolory w pełni nasycone mają jasność w granicach 85-170.


.. _wsp:

Transformacja współrzędnych i kąta
**********************************

Przed zwróceniem pozycji robotów i piłki z funkcji rozpoznającej możliwa jest zmiana
układu współrzędnych. Transformację określają dwa punkty obrazka (lewy górny i
prawy dolny róg boiska) i skala wyjściowa.

Oznaczmy:

* :math:`(s_x, s_y)` - skala wyjściowa
* :math:`P_1` - pozycja piksela na obrazku będącego lewym górnym rogiem obrazu
* :math:`P_2` - pozycja piksela na obrazku będącego prawym dolnym rogiem obrazu

Wtedy: 

* :math:`P_1` - ma współrzędne :math:`(0, 0)`  w układzie wyjściowym
* :math:`P_2` - ma współrzędne :math:`(s_x, s_y)`  w układzie wyjściowym

Na rysunku widać transformację współrzędnych z współrzędnych obrazka (czerwony)
do współrzędnych wynikowych (niebieski).

.. image:: /transform.png
    :align: center
    :width: 320pt
    :height: 240pt

Kąty zwracane jako ustawienie robota są liczone zgodnie z ruchem wskazówek 
zegara względem układu współrzędnych obrazka, od 0 do :math:`2 \pi`, gdzie 0 to
orientacja w prawo.

.. image:: /coords.png
    :align: center
    :width: 150pt
    :height: 150pt
