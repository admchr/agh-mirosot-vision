
Poszczególne kroki algorytmu
----------------------------

Regulacja balansu bieli
***********************

Wiedząc, że pomalowany na biało przedmiot posiada kolor :math:`(w_r, w_g, w_b)`,
bardzo łatwo przekolorować bitmapę, by biały wyglądał na biały. Wystarczy 
dla każdego piksela :math:`p` zastosować prostą transformację: 

.. math::
    (p'_r, p'_g, p'_b) = (p_r\cdot\frac{255}{w_r}, p_g\cdot\frac{255}{w_g}, p_b\cdot\frac{255}{w_b})

Jeśli :math:`w`, czyli punkt bieli jest znany i taki sam, na tym można zakończyć
balans. Co jednak, jeśli punkt bieli jest różny w różnych punktach obrazu?
Wtedy w każdym punkcie obrazu (tak naprawdę na każdym małym kafelku obrazu) 
wyznaczamy punkt bieli jako średnią ważoną wagą :math:`\frac{1}{r^2}`, 
gdzie :math:`r` to odległość punktu z bielą do badanego punktu.

Transformacja do HSV
********************

Obok obrazka w formacie RGB tworzony jest jego odpowiednik HSV.

Wstępne zaznaczanie kandydatów
******************************

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

