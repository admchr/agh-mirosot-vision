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



TODO

Wymagania systemowe
*******************

Do poprawnego działania aplikacja wymaga zainstalowanych:

* kamery zgodnej z systemem Microsoft Windows
* Microsoft Visual Studio 2008 Redistributable Package
* Microsoft .NET Framework 2.0

TODO

Koszulki zawodników
*******************

W ogólnym przypadku, dla dowolnych koszulek zgodnych z regułami gry nie jest 
możliwe ustalenie kąta natarcia i identyfikacja poszczególnych robotów. 
Dla uzyskania takich danych konieczne jest zdecydowanie się na wygląd koszulki, 
co z kolei determinuje metody określania kąta i identyfikacji robota. 

Algorytmy rozpoznawania w programie są dostosowane do jednego schematu barwnego 
na robotach. Jest on przedstawiony na schemacie:

.. image:: /team.png
    :width: 320pt
    :height: 240pt

Koszulka składa się z czarnej ramki i 3 kolorowych trójkątów. Największy jest
kolorem drużyny (żółty lub niebieski, zgodnie z regułami). Pozostałe dwa obszary
mogą mieć dowolne dwa różne kolory, ale nie niebieski, żółty ani pomarańczowy.
Względy praktyczne sugerują, że mają to być kolory o jasności zbliżonej do
koloru drużyny, różniące się maksymalnie barwą.


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

Dokładna transformacja z przestrzeni RGB do przestrzeni HSL wymaga kilku kroków:

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
        H &\in [0, 256[ \\
        S &= 256 \cdot s \\
        L &= 256 \cdot l


transformacja współrzędnych i kąta
**********************************


Przed wysłaniem pozycji robota do programu sterującego możliwa jest zmiana
układu współrzędnych. Transformację określa para punktów będących lewym górnym 
i prawym dolnym rogiem boiska :math:`P_1, P_2` i skala wyjściowa 
:math:`(s_x, s_y)`. :math:`P_1` jest punktem :math:`(0, 0)` we współrzędnych 
wyjściowych. :math:`P_2` jest transformowany tak, żeby we współrzędnych 
wyjściowych miał pozycję :math:`(s_x, s_y)`.

.. image:: /transform.png
    :width: 320pt
    :height: 240pt

Kąty zwracane jako ustawienie robota są liczone zgodnie z ruchem wskazówek 
zegara na obrazku, od 0 do :math:`2 \pi`, gdzie 0 to orientacja w prawo.

.. image:: /coords.png
    :width: 150pt
    :height: 150pt
