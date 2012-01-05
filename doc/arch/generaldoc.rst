

Wprowadzenie
------------

Opis rozwiązania
****************

Z rozwiązania wydzielony został element odpowiedzialny za faktyczne obliczenia.
Jest nim biblioteka z interfejsem w C, w której znajduje się funkcja 
rozpoznająca zawartość obrazu. Funkcja ta korzysta wyłącznie z bitmapy obrazu
i pewnych danych na temat jego zawartości (ilość robotów, brzegi boiska, ...)
wprowadzanych na samym początku przez użytkownika. Operację taką można 
nazwać funkcją w matematycznym sensie:

.. math:: 

    f_{\mathrm{config}}: M_{n\times m} \to (\mathrm{\vec{Pos}_{blue}},\mathrm{\vec{Pos}_{yellow}})

Istnieje możliwość używania dodatkowych informacji na temat przeszłych zdarzeń
do wspomagania rozpoznawania, ale w obecnym rozwiązaniu nie są one 
wykorzystywane.


Drużyny przeciwnika
^^^^^^^^^^^^^^^^^^^

Robot widziany z góry musi posiadać region koloru drużyny (żółtego lub 
niebieskiego), o wymiarze nie mniejszym, niż
:math:`3.5\mathrm{cm}\times 3.5\mathrm{cm}`. Nie może on przekraczać wymiarów
kwadratu o boku 8cm, zawierać koloru pomarańczowego ani koloru przeciwnej 
drużyny. Taka duża dowolność wyklucza większość podejść, które mogłyby 
wyznaczyć orientację robota przeciwnika. W szczególności koszulki przeciwnika 
mogą:

* posiadać dowolne obszary koloru czarnego, uniemożliwiające znalezienie obrysu
  robota
* nie posiadać żadnych dodatkowych kolorów, niż czarny i kolor drużyny
* posiadać symetrię radialną
* posiadać różne rozmiary koloru drużynowego
* zgodnie z regułami mogą nie być prostokątne

.. figure:: /legal.png
    :width: 160pt
    :height: 40pt
    :align: center

    Patologiczne przykłady legalnych koszulek robotów.

Na zawodach pojawiały się już roboty z koszulkami jedynie w kolorach czarnym i 
drużynowym, różniące się jedynie kształtem żółtej/niebieskiej plamy, więc 
problem niestandardowych układów na koszulkach nie jest tylko teoretyczny. 


Sklejanie się robotów
^^^^^^^^^^^^^^^^^^^^^

Dodatkowe utrudnienie stanowi fakt, że w czasie meczu roboty mogą zbliżać się do
siebie bokami tego samego koloru, zlewając je w jeden obszar. 

.. image:: /collision.png
    :width: 64pt
    :height: 64pt
    :align: center

Można próbować rozwiązywać takie problemy, znając wygląd wszystkich koszulek na
boisku i próbować je przyporządkować do miejsc największego prawdopodobieństwa
ich wystąpienia, nie zważając na ciągłość obszarów na obrazku. Takie podejście
może być zbyt wolne i zbyt skomplikowane w implementacji i użytkowaniu. 

Należy zauważyć pewną różnicę pomiędzy efektami sklejenia się własnych robotów 
a sklejeniem robotów przeciwnika. Jeśli dwa roboty przeciwnika zostaną rozpoznane jako jeden, 
w pewnym miejscu może pojawić się informacja o nieistniejącym robocie przeciwnika.


Jasność i balans bieli
^^^^^^^^^^^^^^^^^^^^^^

Warunki oświetleniowe dzienne i sztuczne mają bardzo różny zakres jasności 
i temperatury barwnej. Ludzki system wizyjny nie ma żadnych problemów z
przetwarzaniem w bardzo różnych warunkach, dlatego efekt ten jest dobrze
widoczny dopiero przy obserwowaniu zdjęć z kamery. Oprogramowanie obsługujące 
kamerę może próbować naprawić obraz zmieniając czas migawki i wartości kanałów 
barwnych, ale musi mieć znać punkt odniesienia. Zazwyczaj kamery próbują 
wykorzystać statystyki całego obrazu, żeby znależć optymalne ustawienia.
W przypadku czarnego boiska do gry zajmującego cały kadr jest to podejście 
skazane ne porażkę, dlatego wszelkie ustawienia automatyczne najlepiej wyłączyć.

Na korekcję jasności i barw wpływ może mieć również specyficzna dla danego 
urządzenia wyjściowa przestrzeń kolorów. Najbardziej popularną ustandaryzowaną
przestrzenią jest sRGB, w której wartość piksela zależy w sposób mocno 
nieliniowy od jego jasności. 

W użytym przez nas modelu kamery nie znaleźliśmy nieliniowości wymuszających 
korekcję (patrz rysunek).

.. _white-figure

.. figure :: /white.png
    :width: 400pt
    :height: 400pt

    Za pomocą wydrukowanych wzorów zatrzymujących 25, 50, 75, 100% światła 
    odbijanego od kartki sprawdziliśmy liniowość przestrzeni kolorów dla 
    testowej kamery. 
    Otrzymane wartości pikseli są zbliżone do liniowego odwzorowania.
    Dla porównania w przestrzeni sRGB ciemne obszary mają dużo większą wartość
    składowych, niż przy prostym liniowym RGB.


Przy założeniach dotyczących liniowości przestrzeni kolorów, 
fizycznego modelu absorbcji (np. 50% szary transfomuje kolor światła
:math:`(r, g, b)\to \frac{1}{2}(r, g, b)`) i dodatkowych założeniach 
dotyczących gładkości spektrum długości fali można skonstruować bardzo prosty 
model korekcji barw.

Wiedząc, że pomalowany na biało przedmiot posiada kolor :math:`(w_r, w_g, w_b)`,
bardzo łatwo przekolorować bitmapę, by poprawić odwzorowanie kolorów. Wystarczy 
dla każdego piksela :math:`p` zastosować prostą transformację: 

.. math::
    (p'_r, p'_g, p'_b) = (p_r\cdot\frac{255}{w_r}, p_g\cdot\frac{255}{w_g}, p_b\cdot\frac{255}{w_b})

Taka transformacja przekształca piksel :math:`(w_r, w_g, w_b)` w 
:math:`(255, 255, 255)`. Tu należy zwrócić uwagę, że jeśli przechowujemy 
wartości składowych w pojedyńczym bajcie, to możemy obciąć składowe kolorów, 
które z jakichś względów posiadają składowe większe niż odpowiadające im 
składowe punktu bieli. W takim wypadku lepiej jest przeskalować wynik działania,
tracąc nieznacznie (pomijalnie) na precyzji.

.. math::
    (p'_r, p'_g, p'_b) = (p_r\cdot\frac{128}{w_r}, p_g\cdot\frac{128}{w_g}, p_b\cdot\frac{128}{w_b})

Powyższe wyprowadzenie zakładało, że punkt bieli jest znany i taki sam na całym 
obrazku. Jeśli mamy pomiary bieli w wielu punktach obrazu, lokalny punkt bieli
jest wyliczany jako średnia ważona  wagą :math:`\frac{1}{r^2}`, 
gdzie :math:`r` to odległość położenia piksela z bielą do badanego punktu.


Mieszanie się kolorów
^^^^^^^^^^^^^^^^^^^^^

Piksele znajdujące się na brzegach kolorowych obszarów mogą przyjmować dowolny 
kolor pośredni. W przypadku kolorów zielonego i fioletowego znajdujących się 
obok siebie oznacza to możliwość powstania pomiędzy nimi niebieskiej linii.
Jako, że kolory drużynowe rozpoznaje się po barwie
i łączy na podstawie bliskości (sąsiedztwa), niebieskie kreski mogą zostać
połączone z kolorem drużynowym. Taki artefakt jest w stanie zdestabilizować 
wynik wyznaczania końcowej orientacji robota. Efektem jest odwrócenie 
rozpoznanego robota o :math:`180^\circ`, co dodatkowo pociąga za sobą 
niemożliwość znalezienia kolorów pobocznych i poprawnego zidentyfikowania 
robota.

.. figure:: /colorblur.png
    :width: 200pt
    :height: 200pt

    Piksele na brzegach łączących jednobarwne obszary mogą przyjmować kolory 
    pośrednie. Na pokazanym przykładzie efekt jest pogłębiony przez użytą 
    kamerę, która nadaje tę samą barwę grupom pikseli o rozmiarze 
    :math:`2\times 2`.

Zależności
**********

Zależności kodu obliczeniowego skompilowanej biblioteki ograniczają się do 
linkowania z biblioteką OpenCV (http://opencv.willowgarage.com/wiki/). Nagłówki
OpenCV są potrzebne tylko do kompilacji biblioteki, nagłówek ``amv.h`` nie 
zawiera żadnych odniesień do zewnętrznych symboli.

Z OpenCV są wykorzystywane następujące elementy:

* struktura ``cv::Mat``, będąca kontenerem na bitmapy, wraz z operacjami dostępu
  do pikseli, skalowaniem 
* konwersja współrzędnych przestrzeni kolorów z ``BGR`` do ``HSL`` i z powrotem.
* pomocnicze struktury danych jak punkty 2D, dane koloru piksela, prostokątne 
  obszary itp.

Widać, że stopień użycia biblioteki jest dość niewielki i nie stanowiłoby zbyt
wielkiego trudu zastąpienie funkcjonalności OpenCV własnymi implementacjami.
Powodem wykorzystania biblioteki była możliwość szybkiego prototypowania przy
użyciu ogólnych algorytmów z OpenCV, które w związku z dużymi wymaganiami co do
wydajności musiały zostać zastąpione własnymi implementacjami. Istotną pomocą 
były też funkcje czytające pliki graficzne, co pozwoliło stworzyć proste 
narzędzia testujące.

