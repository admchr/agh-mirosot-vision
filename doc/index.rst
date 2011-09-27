.. agh-mirosot-vision documentation master file, created by
   sphinx-quickstart on Tue Sep 27 20:57:37 2011.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

.. highlight:: c

agh-mirosot-vision
==================

Dokumentacja interfejsu
-----------------------

Ze względu na wymagania dotyczące wydajności i konieczność współpracy z 
maksymalnie różnorodnymi środowiskami algorytm rozpoznawania robotów został
zaimplementowany jako program w języku C++ z interfejsem w C. Taka konfiguracja
zapewnia maksymalną kompatybilność z dowolnym językiem programowania.

Cała istota algorytmu jest zawarta w jednej funkcji:

.. c:function:: robot_data find_teams(mirosot_vision_config* config)

Przyjmuje ona dane wizualne i tworzy opis drużyn robotów.  

.. c:type:: struct mirosot_vision_config

::

        
    struct mirosot_vision_config {
        unsigned char* image;
        int height, width;
        
        image_pos* white_points;
        int white_points_len;
        
        double px_per_cm;
        double robot_size;
        
        int meanshift_radius;
        int meanshift_threshold;
        
        unsigned char *debug_balance;
        unsigned char *debug_prescreen;
        unsigned char *debug_meanshift;
        unsigned char *debug_patches;
        unsigned char *debug_robots;
        void* state;
    };

Strukturę należy zainicjalizować funkcją ``init_config``, która wypełnia 
pola wartościami domyślnymi i przygotowuje zawartość tablic pomocniczych 
ukrytych w polu ``state``.

Poniżej opisane są poszczególne elementy struktury.

dane 
^^^^^

Struktura wejściowa zawiera wskaźnik do obrazka, z którego ma zostać wyciągnięta 
informacja o robotach. Dane koloru pikseli bitmapy są zapisane jako 3 bajty w 
formacie BGR, czyli dla wskaźnika ``unsigned char* ptr``::

    B = ptr[0];
    G = ptr[1];
    R = ptr[2];

Wartości pikseli obrazka na współrzędnych x i y są określone wzorem 
``img(x, y) = img_ptr[3*(x + y*width)]``, czyli piksele są upakowane ciasno 
w przestrzeni adresowej i są przechowywane wierszami.

Alokacją obrazka zajmuje się użytkownik. 
**Zawartość bitmapy zostaje nadpisana po wywołaniu find_teams**

