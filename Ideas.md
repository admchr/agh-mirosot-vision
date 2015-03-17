# O obrazkach #
## z webcama ##
Obrazki są 640x480

(465-485)x(385-390) - rozmiar boiska,
roboty mają bok 15-18px, bok robota może być rozmyty do 3 pikseli, **smużenia nie widać**, poza tym roboty otrzymują od świata lub algorytmu kompresji czarną obwódkę, która się niestety miejscami przebarwia.

Kolor bieli od zera absolutnego do #a2a59a
## z przemysłowej ##
Obrazki są 520x386

(440-460)x(360-380) - rozmiar boiska,
bok robota to ok. 14px, przy czym każdy bok może być rozmyty przy stacjonarnym robocie do 3 pikseli; jeżdżący robot rozciąga się do nawet 20 pikseli (licząc pesymistycznie)

Kolor bieli od #50584b do #abc293 (ohydny zielony)

# Pomysły #

W jednym paperze był taki pomysł przyspieszenia działania rozpoznawania koloru niebieskiego/żółtego poprzez zmniejszenie ilości próbek (czyli zmniejszenie rozmiaru obrazka). Policzmy. Bok robota ma od 14px, więc jeden kolorowy kwadrat ma (14px/2)<sup>2</sup> = 49px<sup>2</sup>. Widać tu pole manewru. Zmniejszając 4x ilość pikseli i tak dostaniemy ponad 10 próbek na pole robota.

# Problemy #


# Algorytm #

## Faza 0 - dostosowanie temperatury koloru ##

Można wykorzystać brzegi boiska(?).

## Faza 1 - rozpoznawanie pomarańczowego, żółtego i niebieskiego ##

Kolory żółty i niebieski na webcamie różnią się od wszystkich innych kolorów dość znacznie (i muszą się różnić zgodnie z zasadami) i są dość podobne niezależnie od temperatury światła:

  * niebieski: hue 8 - 22, sat 182 - 255
  * żółty: hue 85 - 105 (wersja mocniejsza charakteryzuje się całkowitym brakiem składowej niebieskiej, jasny żółty ma nasycenie od 128-255)

Możemy wykorzystać trick z przeglądaniem co 4-ego piksela boiska. Po wykonaniu tej czynności dostajemy małe obszary, w których mogą być roboty. Ich rozmiar to (ostrożnie licząc) `(2*sqrt(2)*18px)^2`, boisko ma `465px * 385px`, wychodzi 50x przyspieszenie. Możemy je zatem traktować trochę wolniejszymi algorytmami.

## Faza 2 - znajdowanie blobów ##

Musimy użyć jakiegoś klasycznego algorytmu podziału obrazka. Trzeba będzie uważać na kolorowe artefakty wokół robotów.


## Faza 3 - rozpoznawanie kolorów & robotów ##

**Zakładamy specyficzny patch dla obu drużyn.** Patch musi mieć właściwość wyznaczania podstawowej (niekoniecznie dokładnej) orientacji robota na podstawie samego koloru teamowego. W ten sposób dostaniemy możliwość samplowania tylko kilku punktów wokół robota, żeby poznać jego pozycję.

W takim układzie kolory dodatkowe również są do pewnego stopnia hardcodowane.

Co by było, gdybyśmy zrezygnowali z tego założenia?

W ogólnym przypadku kolory żółty i niebieski stykających się robotów mogą tworzyć jeden spójny obszar, co mocno utrudnia zrobienie z nimi czegokolwiek. Dodatkowo reguły gry jasno nie zabraniają użycia więcej niż jednego pola kolorowego na jednym robocie.

![http://wiki.agh-mirosot-vision.googlecode.com/hg/problematic-collision.png](http://wiki.agh-mirosot-vision.googlecode.com/hg/problematic-collision.png)

Mając kolor czarny w swoim patchu robot może nie mieć dobrze zdefiniowanej pozycji na czarnym tle, więc wszelkie metody próbujące odnaleźć kontury samych robotów nie mają szans.

![http://wiki.agh-mirosot-vision.googlecode.com/hg/problematic-patch.png](http://wiki.agh-mirosot-vision.googlecode.com/hg/problematic-patch.png)

Czyniąc założenia co rozmiaru i kształtu niebieskich patchy (np. "na wszystkich robotach jest taki sam patch") można prawdopodobnie zajść trochę dalej (na przykład jakoś krojąc za duży blob na kilka części).

Nawet jeśli znajdziemy już środki ciężkości niebieskich patchy, musimy i tak znaleźć ich orientację. Algorytmy przeszukujące całe 360° może być lekko za wolny. Jeśli nie mamy kontroli nad kolorami musimy również wiedzieć, jaki margines błędu stosować do ich obserwacji.

## Faza 4 - poprawianie orientacji i pozycji ##

Kiedy już mamy ogólnie wyznaczoną pozycję robota, możemy próbować dopasować go bardziej.