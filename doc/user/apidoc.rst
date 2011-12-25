
.. highlight:: c

C API 
-------

Rdzeń obliczeniowy został wyodrębniony w postaci biblioteki w C. Umożliwia to
wykorzystanie go w aplikacjach wykorzystujących dowolny język programowania.
Jedyna zależność kodu obliczeniowego to opencv.

Przykładowy minimalny program wykorzystujący interfejs jest dość prosty::

    struct amv_config config;
    amv_config_init(&config);
    config.image_width = WIDTH;
    config.image_height = HEIGHT;
    load_config(&config, argv[2]);
    
    struct amv_state state;
    amv_state_new(&state, &config);

    while(!stop) {
        unsigned char* frame = grab_frame();
        struct amv_vision_data data = amv_find_teams(frame, &state, NULL);
    }
    amv_state_free(&state);

Po zainicjalizowaniu konfiguracji funkcją :c:func:`init_config()` jedynymi wartościami, 
które bezwzględnie muszą zostać wypełnione są wysokość i szerokość obrazka w 
pikselach. Pozostałe pola zostają wypełnione domyślnymi wartościami, które 
gwarantują legalne działanie algorytmu. Poszczególne elementy struktur 
:c:type:`amv_config` i
:c:type:`amv_vision_data`, a także poprawny format ramki obrazu są opisane poniżej, w 
dokumentacji odpowiednich pól i funkcji.

Struktura :c:type:`amv_state` przechowuje konfigurację algorytmu wraz z wewnętrznymi
strukturami pomocniczymi, które mają za zadanie przyspieszyć działanie funkcji 
:c:type:`amv_find_teams`.

Aby uzyskać obrazki diagnostyczne, należy wykorzystać struturę
:c:type:`amv_debug_info`::
    
    unsigned char* debug_frame0 = malloc(3 * WIDTH * HEIGHT);
    
    amv_debug_info debug;
    amv_debug_init(&debug);
    debug.debug_results = debug_frame0;
    struct amv_vision_data data = amv_find_teams(frame, &state, debug);
    
    view_frame(debug_frame0);
    free(debug_frame0);

Wskażniki do pamięci w której mają być zapisane ramki diagnostyczne są 
przyczepiane do odpowiednich pól struktury.


Konfiguracja
************

.. c:type:: struct amv_config

    ::

        struct amv_config {
            int image_height, image_width;

            struct amv_image_pos* white_points;
            int white_points_len;
            
            struct amv_image_pos* mask_points;
            int mask_points_len;

            double px_per_cm;
            
            int meanshift_radius;
            int meanshift_threshold;
            int same_color_distance;
            int linearize;

            struct amv_transform_info transform;

            int black_cutoff;
            struct amv_team_info blue;
            struct amv_team_info yellow;
            struct amv_color_info orange;
            int white_is_yellow;
            int minimum_saturation;
            int white_cutoff;
        };

    TODO
    
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


.. c:type:: struct image_pos

    ::
        
        struct image_pos {
            int x;
            int y;
        };

    
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


Funkcje
*******

.. c:function:: robot_data find_teams(mirosot_vision_config* config)

    Przyjmuje ona dane wizualne i tworzy opis drużyn robotów.
    
    Struktury z tej funkcji intensywnie korzystają ze struktury opisującej
    położenie punktu na płaszczyźnie bitmapy.

    