
Dokładny opis algorytmu
-----------------------

Algorytm rozpoznawania składa się z sekwencji następujących po sobie operacji 
na obrazie:

* regulacja jasności/balans bieli
* maskowanie nieistotnych obszarów
* transformacja koloru z reprezentacji RGB do HSL
* klasyfikacja kolorów
* podział klas kolorów na spójne obszary
* wyznaczanie obszarów należących do robotów z największym prawdopodobieństwem
* wyznaczenie kąta obrotu i identyfikacji robota w drużynie (tylko w przypadku
  własnej drużyny)

Regulacja jasności/balansu bieli
***********************

..
    TODO tu krótkie streszczenie samego wyliczenia

Mając do dyspozycji punkt bieli w danym miejscu wyliczany jest zbalansowany 
kolor dla piksela :math:`p`:

.. math::
    (p'_r, p'_g, p'_b) = (p_r\cdot\frac{128}{w_r}, p_g\cdot\frac{128}{w_g}, p_b\cdot\frac{128}{w_b})

Jeśli do dyspozycji jest kilka punktów bieli w różnych miejscach, biel w 
punktach pośrednich jest wyliczana jest jako średnia ważona  wagą :math:`\frac{1}{r^2}`, 
gdzie :math:`r` to odległość piksela z bielą do badanego punktu.


Maskowanie
**********

Podane przez użytkownika punkty :math:`(P_0, P_1, \ldots, P_n)` określają
pojedynczy wielokąt, w który ogranicza pole istotne dla rozpoznania.
Piksele z poza tego wielokąta są zamieniane w kolor czarny (zerowane).
Zadaniem maskowania jest usuwanie tła poza kontrolą reguł gry, zawierającego
kolorowe obiekty i ludzi, którzy mogą skutecznie destabilizować algorytm.
Poza tym elementem maska nie posiada żadnego znaczenia we właściwym algorytmie.


Transformacja do HSL
********************

Dla każdej klatki obrazu jest przygotowywana jej kopia przetransformowana do
reprezentacji HSL. Jako, że ilość operacji arytmetycznych koniecznych do wykonania
transformacji jest zbyt duża, konieczne jest korzystanie ze ztablicowanych 
wyników. Ilość pamięci potrzebnej do skonstruowania takiej tablicy wynosi 
:math:`3\mathrm{B}\times 256^3 = 48 \mathrm{MiB}`, co nie jest dużym kosztem
pamięciowym. Dodatkowo wymusza to narzut kilku sekund na przygotowanie tablicy 
przy starcie aplikacji.

Nazwa "HSL" nie określa jednoznacznie reprezentacji kolorów. Istnieje wiele 
sposobów definiowania jasności, barwy i nasycenia. Sposób wyliczania tych 
wartości w programie wygląda następująco:


1. Najpierw składowe :math:`R, G, B \in \{0, \ldots, 255\}` są konwertowane do przedziału :math:`[0, 1[`:

.. math::

    r &= \frac{R}{256} \\
    g &= \frac{G}{256} \\
    b &= \frac{B}{256} \\
    

2. Z przeskalowanych wartości wyliczana jest wartość maksymalna i chromatyczność:

.. math::
    
        m &= \operatorname{max}(r, g, b) \\
        c &= m - \operatorname{min}(r, g, b)

3. Wyliczana jest barwa, nasycenie i jasność:

.. math::
        
        h &= 60^\circ \cdot 
            \begin{cases}
              0        &\mbox{dla } c = 0 \\
              \frac{g - b}{c} \;\bmod 6 &\mbox{dla } m = r \\
              \frac{b - r}{c} + 2       &\mbox{dla } m = g \\
              \frac{r - g}{c} + 4       &\mbox{dla } m = b
            \end{cases} \\
        h &\in [0, 360^\circ[ \\
        s &= \begin{cases}
              0        &\mbox{dla } m = 0 \\
              \frac{c}{m}&\mbox{dla } m \neq 0 \\
            \end{cases} \\
        s &\in [0, 1[ \\
        l &= \frac{r+g+b}{3} \\
        l &\in [0, 1[

4. Wyniki są skalowane do wartości mieszczących się w typie ``unsigned char``.
Barwa (Hue) jest transformowana z zakresu :math:`[0, 360^\circ[` do wartości
``0..179``, jasność i nasycenie wypełniaja wszystkie wartości ``0..255``.
    
.. math::

        H &= \frac{h}{2} \\
        H &\in [0, 180[ \\
        S &= 256 \cdot s \\
        L &= 256 \cdot l


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

Kąt obrotu robota i identyfikacja
*********************************

..
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
