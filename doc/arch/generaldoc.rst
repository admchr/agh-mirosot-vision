

Opis rozwiązania
----------------

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

Opis algorytmu
**************

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
* nie posiadać żadnych dodatkowych kolorów, poza czarnym i kolorem drużyny
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


Koszulki
^^^^^^^^

.. figure:: /team.png
    :width: 320pt
    :height: 240pt

    Projekt koszulek.
    
Wzór koszulki dla robotów miał spełniać zadania:

* posiadać łatwo rozpoznawalne cechy pozwalające na znalezienie kąta obrotu
  robota
* posiadać łatwo rozpoznawalne cechy pozwalające na identyfikację robota
* być odporny na sklejanie się jednokolorowych powierzchni

Najprostszym rozwiązaniem problemu sklejania się robotów jest dodanie czarnego 
marginesu na brzegu robota, co też zostało zrobione.

Problem obrotów i identyfikacji można rozwiązać za pomocą modyfikacji kształtu
figury w kolorze drużynowym lub poprzez dodanie nowych kolorowych powierzchni.

Podejście opierające się wyłacznie na dodatkowych kolorach pociąga za sobą duże
komplikacje: trzeba brać pod uwagę spójnośc dodatkowych kolorowych plam wokół 
obrazu robota i przyporządkowywać plamy do konkretnych robotów. Ta czynność może
stać się bardzo skomplikowana w sytuacji, kiedy kilka robotów styka się ze sobą.


Z drugiej strony opieranie się wyłącznie na kształcie wymagałoby zakodowania 
kąta i identyfikatora robota (liczby od 0 do 4) w jednokolorowym kształcie, 
na co nie znaleźliśmy dobrego (odpornego na zakłócenia) sposobu.

W przypadku większości drużyn dominuje podejście pośrednie. W [Hao]_ znajduje 
się dyskusja wad i zalet różnych rodzajów koszulek, z których wybrany zostaje 
wzór składający się z wydłużonego prostokąta w kolorze drużynowym z dodatkowymi
kolorami po bokach. Ten sam wzór koszulki opisują autorzy [Jiang]_. Metoda 
rozpoznawania takiego kształtu opiera się na znalezieniu prostej minimalizującej
sumę kwadratów odległości
(regresja Deminga http://en.wikipedia.org/wiki/Deming_regression ), która 
znajduje kąt obrotu z dokładnością do :math:`180^\circ`. Ujednoznacznienie kąta 
i identyfikacja następuje na podstawie kolorów pobocznych. 

Nasze rozwiązanie używa regresji do wyznaczenia kąta trójkąta drużynowego, po
czym ujednoznacznia kąt, licząc piksele po obu stronach regresji. 
Mając dane o pozycji robota możemy przewidzieć położenie dodatkowych barwnych
trójkątów. 



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
W przypadku własnych robotów informacje mogą bezpośrednio wpływać na rozkazy 
wydawane robotom. W skrajnych wypadkach programy sterujące mogą przestawać 
wydawać polecenia i zaciąć się w sklejonej pozycji.


Jasność i balans bieli
^^^^^^^^^^^^^^^^^^^^^^

Warunki oświetleniowe dzienne i sztuczne mają bardzo różny zakres jasności 
i temperatury barwnej. Ludzki system wizyjny nie ma żadnych problemów z
przetwarzaniem w bardzo różnych warunkach, dlatego efekt ten jest dobrze
widoczny dopiero przy obserwowaniu zdjęć z kamery. Oprogramowanie obsługujące 
kamerę może próbować naprawić obraz zmieniając czas migawki i wartości kanałów 
barwnych, ale musi znać punkt odniesienia. Zazwyczaj kamery próbują 
wykorzystać statystyki całego obrazu, żeby znależć optymalne ustawienia.
W przypadku czarnego boiska do gry zajmującego cały kadr jest to podejście 
skazane ne porażkę, dlatego wszelkie ustawienia automatyczne najlepiej wyłączyć.

Na korekcję jasności i barw wpływ może mieć również specyficzna dla danego 
urządzenia wyjściowa przestrzeń kolorów. Najbardziej popularną ustandaryzowaną
przestrzenią jest sRGB, w której wartość piksela zależy w sposób mocno 
nieliniowy od jego jasności. W użytym przez nas modelu kamery nie znaleźliśmy 
jednak nieliniowości wymuszających 
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
:math:`(255, 255, 255)`. Tu pojawia się niebezpieczeństwo przekroczenia zakresu 
jednego bajtu: jeśli punkt bieli jest ciemniejszy od danego piksela, dostaniemy 
błędny wynik. W takim wypadku lepiej jest przeskalować wynik działania,
tracąc nieznacznie (pomijalnie) na precyzji.

.. math::
    (p'_r, p'_g, p'_b) = (p_r\cdot\frac{128}{w_r}, p_g\cdot\frac{128}{w_g}, p_b\cdot\frac{128}{w_b})

Powyższe wyprowadzenie zakładało, że punkt bieli jest znany i taki sam na całym 
obrazku. Jeśli mamy pomiary bieli w wielu punktach obrazu, lokalny punkt bieli
jest wyliczany jako średnia ważona  wagą :math:`\frac{1}{r^2}`, 
gdzie :math:`r` to odległość położenia piksela z bielą do badanego punktu.


.. figure:: /balance.png
    :width: 400pt
    :height: 200pt
    
    Obraz przed i po przeprowadzeniu balansu bieli. Czerwonymi plusami oznaczono
    punkty bieli. Zgodnie z zamierzeniami kolor plam został zmieniony na szary 
    50%. Zmiana barwy powierzchni wokół punktów bieli pozwala zobaczyć zakres 
    oddziaływania każdego z nich -- widzimy łagodne przejścia między strefami 
    oddziaływania. 

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

Możliwości poprawy
******************

W wielu aspektach algorytm zachowuje się nieidealnie:

* Na mocno zabarwionych obrazach balans bieli ma tendencję do przedobrzania - 
  zmienia ciemny obraz o odcieniu niebieskim w obraz o odcieniu pomarańczowym
* Algorytm ujednoznaczniania kąta obrotu robota na podstawie ilości pikseli po 
  obu stronach regresji nie zawsze działa ze 100% skutecznością
* Wydajność algorytmu można zwiększyć, łącząc kilka przebiegów po pamięci z
  obrazem w jeden - przykładowo można połączyć liczenie transformacji do HSV
  i klasyfikację pikseli w jedną pętlę.
* Czarne obramowanie wokół kolorów pobocznych na koszulkach nie jest konieczne, gdyż w obecnym
  kształcie algorytm nie potrzebuje izolować obszarów kolorów innych niż 
  drużynowe. Usunięcie ramek zwiększa powierzchnię koloru możliwą do 
  rozpoznania.
* Balans bieli oparty na brzegach boiska jest w stanie dokładnie kontrolować
  jasność jedynie w pobliżu brzegów boiska. Jeśli oświetlenie znajduje się 
  centralnie nad boiskiem, to algorytm nie może zniwelować efektu zwiększenia
  jasności na środku obrazu. Poprawić sytuację mógłoby zmodyfikowanie sposobu normalizacji jasności/balansu 
  bieli, przez wykonanie procedury balansu przed meczem, z wykorzystaniem np.
  białych kartek papieru kładzionych na boisku.
