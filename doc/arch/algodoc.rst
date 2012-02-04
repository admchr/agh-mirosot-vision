
Dokładny opis algorytmu
-----------------------

Algorytm lokalizacji składa się z sekwencji następujących po sobie operacji 
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
********************************

Dla podanych przez użytkownika punktów na obrazku wyznaczane są wartości 
RGB mediany ich otoczenia :math:`3\times 3`. Ta wartość jest zapisywana jako
punkt bieli w danym miejscu na obrazku.

Jeśli do dyspozycji jest kilka punktów bieli w różnych miejscach, biel w 
punktach pośrednich wyliczana jest jako ich średnia ważona  wagą :math:`\frac{1}{r^2}`, 
gdzie :math:`r` to odległość euklidesowa pozycji piksela z bielą do badanego punktu.

Mając do dyspozycji punkt bieli :math:`(w_r, w_g, w_b)` w danym miejscu wyliczany jest zbalansowany 
kolor dla piksela :math:`p`:

.. math::
    (p'_r, p'_g, p'_b) = (p_r\cdot\frac{128}{w_r}, p_g\cdot\frac{128}{w_g}, p_b\cdot\frac{128}{w_b})



Maskowanie
**********

Podany przez użytkownika ciąg punktów na obrazku określa
pojedynczy wielokąt, który ogranicza piksele istotne dla rozpoznania.
Wielokąt nie musi być wypukły.
Piksele z poza obszaru ograniczonego tym  wielokątem są zamieniane w kolor czarny (zerowane).
Zadaniem maskowania jest usuwanie części obrazu poza kontrolą reguł gry, zawierającego
kolorowe obiekty (np. ludzi), które mogą skutecznie destabilizować algorytm.
Poza tym elementem maska nie posiada żadnego innego znaczenia w algorytmie.


Transformacja do HSL
********************

Dla każdej klatki obrazu jest przygotowywana jej kopia przetransformowana do
reprezentacji HSL. Jako, że ilość operacji arytmetycznych koniecznych do wykonania
transformacji jest bardzo duża, konieczne jest korzystanie ze ztablicowanych 
wyników. Ilość pamięci potrzebnej do skonstruowania tablicy wynosi 
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
``0..179``, jasność i nasycenie wypełniają wszystkie wartości ``0..255``.
    
.. math::

        H &= \frac{h}{2} \\
        H &\in [0, 180[ \\
        S &= 256 \cdot s \\
        L &= 256 \cdot l


Przynależność do klas kolorów
*****************************

Żeby rozpoznać pozycję robotów na boisku, przy założeniu obecności robotów przeciwnika (koszulki przeciwnika są nieprzewidywalne) możemy polegać jedynie 
na rozpoznawaniu kolorów żółtego i niebieskiego, które to zgodnie z regulaminem
muszą zawierać się w obszarze każdej koszulki robota z drużyny.

Piksele na tym etapie są dzielone na klasy:

* żółty
* niebieski
* pomarańczowy (piłka)
* żaden z powyższych

Przyporządkowanie koloru piksela do klasy jest wykonywane na podstawie jego 
współrzędnych w przestrzeni HSL. Najpierw eliminowane są piksele o zbyt małej
jasności (Lightness) lub nasyceniu (Saturation). 
Pozostałe są klasyfikowane na podstawie barwy (Hue) do jednej z powyższych klas.


Spójne obszary
**************

Piksele tej samej klasy muszą zostać podzielone na spójne obszary.
Piksele sąsiadujące ze sobą bokami zostają przydzielone do tych samych spójnych
składowych. Każdy z obszarów ma na tym etapie wyliczane statystyki: liczność 
obszaru i średnią wartość RGB obszaru.

Po skompletowaniu listy obszarów danego koloru ustala się je w listę rankingową
na podstawie ilości pikseli i średniej wartości RGB obszaru.
Z góry listy odcinanych jest tyle obszarów ile robotów powinno znajdować się 
na boisku.
Za piłkę przyjmuje się najlepszy pomarańczowy obszar.

Kąt obrotu robota i identyfikacja
*********************************

..
    TODO make use of home_team

Jeśli drużyna robotów jest uznawana za drużynę przeciwnika, to poprzednie kroki
kończą rozpoznawanie. Do sterowania robotami niezbędne są informacje na temat 
orientacji i identyfikacji robotów w drużynie. 

Na drużynowych obszarach wyznaczana jest linia minimalizująca
sumę kwadratów odległości pikseli od linii (regresja Deminga). Za jej pomocą
wyznaczamy kąt obrotu robota z dokładnością do  :math:`180^\circ`. 
Zdeterminowanie, która z dwóch orientacji jest prawdziwa polega na policzeniu 
różnicy ilości pikseli po jednej i drugiej stronie prostej regresji.

Następnie należy rozpoznać dwa pola barwne znajdujące się obok koloru drużynowego.
Mając pozycję i kąt obrotu robota wiemy, gdzie się one znajdują. Należy tylko 
stwierdzić, jakie kolory się na nich znajdują. Przyporządkowanie klas kolorów 
pikselom odbywa się wyłącznie na podstawie ich barwy (Hue).

Żeby jak najlepiej dopasować identyfikacje robotów, dla każdej pary
(robot, identyfikacja) tworzy się liczbową jakość, odpowiadającą na pytanie (przykładowo):

    Jak bardzo rozpatrywany robot pasuje do identyfikatora czerwony-zielony?

Mając jakości wszystkich dopasowań identyfikacji do robotów na boisku chcemy 
znaleźć dopasowanie o jak najlepszej sumie jakości. Problem ten odpowiada
skojarzeniu o maksymalnej wadze w pełnym grafie dwudzielnym. Rozwiązywany
jest on przy pomocy algorytmu zachłannego: dodawane jest za każdym 
razem dopasowanie o największej jakości, którego wierzchołki są 
jeszcze nieprzyporządkowane. Rozwiązanie nie jest teoretycznie optymalne, ale
w praktyce wystarczające.
