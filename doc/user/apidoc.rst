
.. highlight:: c

C API 
-------

Część obliczeniowa programu została wyodrębniona w postaci biblioteki w 
języku C. 



Cała istota algorytmu jest zawarta w jednej funkcji:

.. c:function:: robot_data find_teams(mirosot_vision_config* config)

    Przyjmuje ona dane wizualne i tworzy opis drużyn robotów.
    
    Struktury z tej funkcji intensywnie korzystają ze struktury opisującej
    położenie punktu na płaszczyźnie bitmapy.

.. c:type:: struct image_pos

    ::
        
        struct image_pos {
            int x;
            int y;
        };
    

Konfiguracja
************

.. c:type:: struct mirosot_vision_config

    ::

        struct mirosot_vision_config {
            unsigned char* image;
            int height, width;
            
            image_pos* white_points;
            int white_points_len;
            
            image_pos* mask_points;
            int mask_points_len;

            double px_per_cm;
            double robot_size;
            
            int meanshift_radius;
            int meanshift_threshold;

            int black_cutoff;
            int blue_min;
            int blue_max;
            int yellow_min;
            int yellow_max;
            int minimum_saturation;
            int white_cutoff;
            char linearize;

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

bitmapa
^^^^^^^

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
**Zawartość bitmapy zostaje zamazana po wywołaniu find_teams**

inicjalizacja
^^^^^^^^^^^^^
   Przed pierwszym użyciem należy skonfigurować pewne informacje o świecie
   i warunkach oświetleniowych. Dane na temat samego wyglądu patcha na robocie
   są aktualnie hardcodowane. 

.. c:member:: double mirosot_vision_config.px_per_cm
.. c:member:: double mirosot_vision_config.robot_size

    Podają odpowiednio rozdzielczość obrazu na powierzchni boiska (w pikselach
    na centymetr) i długość boku robota w centymetrach.

.. c:member:: char mirosot_vision_config.linearize

    Wartość logiczna określająca, czy przed wszystkimi innymi operacjami ma 
    zostać wykonana konwersja kolorów z sRGB do liniowego RGB.

.. c:member:: image_pos *mirosot_vision_config.white_points
.. c:member:: int *mirosot_vision_config.white_points_len

    Pozycje punktów boiska, które można określić jako białe. Służą do balansu 
    bieli na obrazku.

.. c:member:: image_pos *mirosot_vision_config.white_points
.. c:member:: int *mirosot_vision_config.white_points_len

    Wierzchołki wielokąta opisującego region zainteresowania. Piksele spoza tego
    wielokąta są maskowane kolorem czarnym. Maskowanie następuje po balansie 
    bieli.

.. c:member:: int mirosot_vision_config.meanshift_radius
.. c:member:: int mirosot_vision_config.meanshift_threshold

    Rozmiar okna algorytmu wygładzania powierzchni meanshift i odległość 
    obcięcia koloru. Rozmiar okna może mieć duży wpływ na wydajność.
    Im większe okno, tym większy obszar jest brany pod uwagę przy wygładzaniu.
    Threshold to odległość w normie euklidesowej pikseli, które są uważane za 
    różne.

.. c:member:: int mirosot_vision_config.yellow_min
.. c:member:: int mirosot_vision_config.yellow_max
.. c:member:: int mirosot_vision_config.blue_min
.. c:member:: int mirosot_vision_config.blue_max

    Zakresy wartości barwy (Hue), w których znajdują się żółte i niebieskie 
    patche robotów.

.. c:member:: int mirosot_vision_config.minimum_saturation

    Minimalne nasycenie koloru będącego częścią obszaru żółtego lub 
    niebieskiego.

.. c:member:: int mirosot_vision_config.black_cutoff

    Minimalna jasność (Lightness) piksela mogącego być przetworzonym. 
    Ciemniejsze piksele są ignorowane.

.. c:member:: int mirosot_vision_config.white_cutoff

    Jasność, przy której piksel uważa się za prześwietlony. Algorytm zakłada, że
    żółte obszary mają tendencję do prześwietlania i traktuje takie obszary 
    jako żółte.

.. c:member:: unsigned char *debug_balance.debug_balance
.. c:member:: unsigned char *debug_balance.debug_prescreen
.. c:member:: unsigned char *debug_balance.debug_meanshift
.. c:member:: unsigned char *debug_balance.debug_patches
.. c:member:: unsigned char *debug_balance.debug_robots

    Jeśli któreś z tych pól zostanie ustawione na bufor zaalokowany przez
    użytkownika, zostanie on wypełniony kopią obrazka z pola image z 
    domalowanymi elementami mogącymi wspomóc diagnozowanie problemów z 
    algorytmem.

Wynik działania
***************

.. c:type:: struct vision_data
    
    Kontener na dane o drużynach i piłce. Zawartość struktur wydaje się być 

    ::
        
        struct vision_data {
            team_data blue_team;
            team_data yellow_team;
            image_pos ball_pos;
        };
        
        struct team_data {
            int team_len;
            robot_data team[MAX_ROBOTS];
        };

        struct robot_data {
            image_pos position;
            double angle;
        };

 
