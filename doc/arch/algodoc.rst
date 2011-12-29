
Dokładny opis algorytmu
-----------------------

Regulacja balansu bieli
***********************

Warunki oświetleniowe dzienne i sztuczne mają bardzo różny zakres jasności 
i temperatury barwnej. Ludzki system wizyjny nie ma żadnych problemów z
przetwarzaniem w bardzo różnych warunkach, dlatego efekt ten jest dobrze
widoczny dopiero przy obserwowaniu zdjęć z kamery. Oprogramowanie obsługujące 
kamerę może próbować naprawić obraz zmieniając czas migawki i wartości kanałów 
barwnych, ale musi mieć znać punkt odniesienia. Zazwyczaj kamery próbują 
wykorzystać statystyki całego obrazu, żeby znależć optymalne ustawienia.
W przypadku czarnego boiska do gry zajmującego cały kadr jest to podejście 
skazane ne porażkę, dlatego wszelkie ustawienia automatyczne najlepiej wyłączyć.

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
gdzie :math:`r` to odległość piksela z bielą do badanego punktu.

Transformacja do HSL
********************

Dla każdej klatki obrazu jest przygotowywana jej kopia przetransformowana do
przestrzeni HSL. Jako, że ilość operacji arytmetycznych koniecznych do wykonania
transformacji jest zbyt duża, konieczne jest korzystanie ze ztablicowanych 
wyników. Ilość pamięci potrzebnej do skonstruowania takiej tablicy wynosi 
:math:`3\mathrm{B}\times 256^3 = 48 \mathrm{MiB}`, co nie jest dużym kosztem
pamięciowym. Dodatkowo wymusza to narzut kilku sekund na przygotowanie tablicy 
przy starcie aplikacji.

Przynależność do klas kolorów
*****************************

Żeby rozpoznać pozycję robotów, w obecności robotów przeciwnika można polegać 
jedynie na obecności kolorów żółtego i niebieskiego - wszelkie inne barwy mogą 
pochodzić z koszulek drużyny przeciwnika, które są nieprzewidywalne.

Piksele na tym etapie są dzielone na klasy:

* żółty
* niebieski
* pomarańczowy
* żaden z pozostałych

Przyporządkowanie koloru piksela do klasy jest wykonywane na podstawie jego 
współrzędnych w przestrzeni HSL. Najpierw eliminowane są piksele o zbyt małej
jasności (L) lub nasyceniu (S). 
Pozostałe są klasyfikowane na podstawie barwy (H) do jednego  z trzech 
przedziałów.


Spójne obszary
**************

Piksele tej samej klasy muszą zostać podzielone na spójne obszary.
Piksele sąsiadujące ze sobą bokami zostają przydzielone do tych samych spójnych
składowych. Każdy z obszarów ma na tym etapie wyliczane statystyki, które będą 
wykorzystane w następnym kroku.

Po skompletowaniu listy obszarów danego koloru ustala się je listę rankingową
na podstawie ilości pikseli i średniej wartości RGB obszaru.
Z góry listy odcinanych jest tyle obszarów ile robotów ma być rozpoznanych.
Za piłkę przyjmuje się najlepszy pomarańczowy obszar.

Kąt natarcia i identyfikacja
****************************

TODO make use of home_team

Jeśli drużyna robotów jest uznawana za drużynę przeciwnika, to poprzednie kroki
kończą rozpoznawanie. Do sterowania drużyną niezbędne są informacje na temat 
orientacji i identyfikacji robotów w drużynie. 

Na drużynowych obszarach wyznaczana jest linia minimalizująca
sumę kwadratów odległości pikseli od linii (regresja Deminga). Za jej pomocą
wyznaczamy kąt obrotu robota z dokładnością do  :math:`180^\circ`. 
Zdeterminowanie, która z dwóch orientacji jest prawdziwa polega na policzeniu 
różnicy ilości pikseli po jednej i drugiej stronie prostej regresji.

Mając pozycję i kąt obrotu koloru drużynowego, można wyznaczyć obszar, w którym
znajdują się kolory poboczne. Przydział obszaru do odpowiadających mu kolorów 
pobocznych następuje wyłącznie na podstawie barwy (H) pikseli. 
Każde z możliwych ustawień kolorów pobocznych próbuje się dopasować do obszaru.
Dopasowaniu odpowiada jakość, która określana jest na podstawie ilości pikseli 
zakwalifikowanych do kolorów tworzących ustawienie i tego, czy środki ciężkości
pikseli są ustawione względem siebie tak, jak kolory z rozpatrywanego 
ustawienia. 

Mając jakości dopasowania dla wszystkich par identyfikacja-robot, algorytm 
zachłannie wybiera dopasowania maksymalizujące jakość aż do dopasowania 
wszystkich robotów.
