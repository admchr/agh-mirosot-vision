
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

Pojęcie koloru rzeczywistego obiektu zależy od czynników, które da się 
modelować wyłącznie w bardzo niedokładnym przybliżeniu. Na finalne odczucie 
koloru wpływa:

1. Natężenie fali przypadające na daną częstotliwość 
2. Ilość odbitego światła danej częstotliwości przez dany punkt obiektu, co 
   zależy nie tylko od materiału, ale i od kąta źródła i obserwatora
3. Transformacja spektrum światła na trzy kanały barwne
4. Skomplikowane przetwarzanie przez obserwatora, zależne między innymi od 
   koloru otoczenia

Widać więc, że próba rozpoznawania barw w różnych warunkach oświetleniowych nie
może bazować na dokładnym modelu psychofizycznym.

Pierwszą rzeczą, którą można zaniedbać jest percepcja koloru. Dana drużyna 
robotów powinna posiadać tylko jeden typ materiału koloru żółtego i 
niebieskiego. Rozpoznawanie kolorów będzie się wtedy sprowadzać do sprawdzania, 
czy materiał jest taki sam, jak ten widziany wcześniej, zamiast do wydawania
ostatecznych sądów na temat tego, który fragment obrazu można określić jako 
"niebieski" w ogólnym tego słowa znaczeniu. 


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


