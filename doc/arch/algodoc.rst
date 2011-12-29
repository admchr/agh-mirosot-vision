
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

Wstępne zaznaczanie kandydatów
******************************

TODO

Piksele, które wpadają w odpowiedni przedział barwy (H) i nasycenia (S) są 
oznaczane jako kandydaci do dalszego przetwarzania w zbiorach pikseli żółtych i 
niebieskich. 

Meanshift
*********

Algorytm segmentacji meanshift posiada wstępną fazę filtrowania, która wygładza
jednolite obszary, tak że po filtrowaniu stanowią jeden jednolity kolor. 
Istotę tego filtrowania można stosować do każdego piksela z osobna. Dzięki temu,
że po filtrowaniu jednokolorowe obszary są bardzo jednolite, można je wyodrębnić
za pomocą zwykłego algorytmu flood-fill.

Obszary
*******

Następnie obszary są segregowane na podstawie ich rozmiaru. Za duże lub za małe
obszary są odrzucane.

Regresja
********

Na wyodrębnionych obszarach wyznaczana jest linia minimalizująca
sumę kwadratów odległości pikseli od linii (regresja Deminga). Za jej pomocą
wyznaczamy kąt obrotu robota. Proste policzenie ilości pikseli patcha pozwala na
odróżnienie obrotów różniących się od siebie o :math:`180^\circ`.

