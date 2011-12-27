.. highlight:: c

Koszulki, założenia
kąt?

transformacja współrzędnych

Konwencje
---------

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


Kąty na płaszczyźnie
********************

TODO