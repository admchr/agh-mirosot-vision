
.. highlight:: c

Koszulki, założenia
Konwencja H = [0, 180)
S = [0, 255]
L = [0, 255]
kąt 

transformacja współrzędnych

C API 
-----

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

    Struktura jest inicjalizowana za pomocą funkcji :c:func:`init_config()`. 
    Poniżej opisane są poszczególne elementy struktury.


.. c:member:: int amv_config.image_width
.. c:member:: int amv_config.image_height

    Wymiary obrazka w pikselach.

.. c:member:: double amv_config.px_per_cm

    Rozdzielczość obrazu na powierzchni boiska (w pikselach
    na centymetr).

.. c:member:: int amv_config.linearize

    Wartość logiczna określająca, czy przed wszystkimi innymi operacjami ma
    zostać wykonana konwersja kolorów z sRGB do liniowego RGB.

.. c:member:: image_pos *amv_config.white_points
.. c:member:: int amv_config.white_points_len

    Pozycje punktów boiska, które można określić jako białe. Służą do balansu
    bieli na obrazku. ``white_points`` powinno wskazywać na tablicę o 
    długości ``white_points_len``. ``white_points_len`` równe 0 oznacza 
    brak przeprowadzania balansu.

.. c:member:: image_pos *amv_config.mask_points
.. c:member:: int amv_config.mask_points_len

    Punkty opisujące wierzchołki wielokąta regionu zainteresowania. 
    Piksele spoza tego wielokąta są wyłączone z dalszego przetwarzania -
    maskowane kolorem czarnym. ``mask_points`` powinno wskazywać na tablicę o 
    długości ``mask_points_len``. ``mask_points_len`` równe 0 oznacza 
    brak przeprowadzania maskowania.

.. c:member:: int amv_config.meanshift_radius
.. c:member:: int amv_config.meanshift_threshold

    Rozmiar okna algorytmu wygładzania powierzchni meanshift i odległość 
    obcięcia koloru. Rozmiar okna może mieć duży wpływ na wydajność.
    Im większe okno, tym większy obszar jest brany pod uwagę przy wygładzaniu.
    Threshold to odległość w normie euklidesowej pikseli, które są uważane za 
    różne.

.. c:member:: int amv_config.minimum_saturation

    Minimalne nasycenie koloru będącego częścią obszaru żółtego, pomarańczowego
    lub niebieskiego.

.. c:member:: int amv_config.white_cutoff

    Jasność (Lightness), powyżej której piksel uważa się za biały.

.. c:member:: int amv_config.black_cutoff

    Jasność, poniżej której piksel uważa się za zbyt ciemny. Ta wartość jest
    używana do rozpoznawania kolorów drużynowych.
    

.. c:member:: amv_team_info amv_config.blue
.. c:member:: amv_team_info amv_config.yellow

    Struktury określające wygląd i skład poszczególnych drużyn.
    
.. c:type:: struct amv_transform_info
    
    ::
        
        struct amv_transform_info {
            struct amv_image_pos field_top_left;
            struct amv_image_pos field_bottom_right;
            struct amv_point output_scale;
        };
    
    Struktura zawiera informacje potrzebne do przeprowadzania transformacji
    położenia obiektów z współrzędnych obrazka do współrzędnych wynikowych.
    Dwa punkty określają położenie punktów rogów boiska, skala wyjściowa określa
    końcowe skalowanie.

.. c:type:: struct amv_team_info

    ::
        
        struct AMV_EXPORT amv_team_info {
            struct amv_color_info color;
            int team_size;

            int home_team;
            struct amv_robot_info robot_info[AMV_MAX_ROBOTS];
            struct amv_color_info secondary_colors[AMV_MAX_SECONDARY_COLORS];
        };
    
    Struktura opisuje drużynę robotów:
    
    * ``color`` - specyfikacja zakresu barw koloru drużyny robotów. 
    * ``team_size`` - ilość robotów na boisku. Algorytm będzie zwracał dokładnie
        taką ilość pozycji robotów. 
    * `` home_team`` - czy analizie mają być poddawane orientacja robotów i ich
        identyfikacja w ramach drużyny.
    * ``robot_info`` - opis robotów w drużynie (o ile ``home_team`` 
        :math:`\neq 0` ). Indeksy w tej tablicy posłużą za identyfikatory 
        robotów.
    * ``secondary_colors`` - opis dostępnych kolorów pomocniczych, używanych w 
        opisach ``robot_info``.
    
.. c:type:: struct amv_robot_info

    ::
        
        struct AMV_EXPORT amv_robot_info {
            int front_color;
            int back_color;
        };
    
    Opis dwóch kolorów pomocniczych na danym robocie. Liczby wskazują na 
    indeksy w tablicy ``secondary_colors``.

Wynik działania
***************

.. c:type:: struct amv_vision_data

    ::
        
        struct AMV_EXPORT amv_vision_data {
            struct amv_team_data blue_team;
            struct amv_team_data yellow_team;
            struct amv_point ball_pos;
        };
    
    Struktura przechowuje wynik rozpoznawania klatki obrazu. 

.. c:type:: struct amv_team_data

    ::
        
        struct AMV_EXPORT amv_team_data {
            int team_len;
            struct amv_robot_data team[AMV_MAX_ROBOTS];
        };
    
    Opis pozycji robotów danej drużyny. Ilość znalezionych robotów
    nie przekracza rozmiaru drużyny podanego na wejściu algorytmu. 

.. c:type:: struct amv_robot_data

    ::
        
        struct AMV_EXPORT amv_robot_data {
            struct amv_point position;
            int identity;
            double angle;
            double certainty;
        };
    
    Opis pozycji znalezionego robota:
    
    * ``position`` - pozycja robota w wyjściowym układzie współrzędnych. 
    * ``identity`` - identyfikator robota w drużynie 
        (o ile roboty są identyfikowane).
    * ``angle`` - kąt 
    
Struktury pomocnicze
********************

.. c:type:: struct amv_image_pos

    ::

        struct amv_image_pos {
            int x;
            int y;
        };

    Punkt dwuwymiarowy, o współrzędnych całkowitych. 
    Używany zwykle do oznaczania pozycji piksela na obrazku.

.. c:type:: struct amv_point

    ::

        struct amv_point {
            double x;
            double y;
        };

    Punkt dwuwymiarowy o współrzędnych zmiennoprzecinkowych. Używany do 
    określenia abstrakcyjnych współrzędnych wyjściowej pozycji robota.

.. c:type:: struct amv_color_info

    ::

        struct amv_color_info {
            int hue_min;
            int hue_max;
        };

    Zakres barw (Hue) w schemacie kolorów HSL. W tej implememtacji składowa barwy 
    ma wartość od 0 do 179 (arytmetyka modulo 180). Przedział barw 
    ``hue_min = 100, hue_max = 50`` jest legalny i oznacza zakres ``0..50,100..179``.

Diagnostyka
***********

.. c:type:: struct amv_debug_info

    ::

        struct amv_debug_info {
            unsigned char *debug_balance;
            unsigned char *debug_prescreen;
            unsigned char *debug_meanshift;
            unsigned char *debug_patches;
            unsigned char *debug_robots;
            unsigned char *debug_results;
        };

    Jeśli któreś z tych pól zostanie ustawione na bufor zaalokowany przez
    użytkownika, zostanie on wypełniony obrazkiem diagnostycznym. 

    Obrazki diagnostyczne mają taki sam rozmiar i format, jak obrazek wejściowy.

.. c:member:: unsigned char *debug_balance.debug_balance

    Ramka obrazu po korekcji jasności i barw.

.. c:member:: unsigned char *debug_balance.debug_prescreen

    Ramka obrazu pokazująca przynależność do zakresów HSV, które definiują 
    poszczególne kolory obszarów.

.. c:member:: unsigned char *debug_balance.debug_meanshift
.. c:member:: unsigned char *debug_balance.debug_patches
.. c:member:: unsigned char *debug_balance.debug_robots

TODO

Funkcje
*******


.. c:function:: amv_vision_data amv_find_teams(
        unsigned char* image, struct amv_state* state, 
        struct amv_debug_info* debug)

    Przyjmuje ona dane wizualne i tworzy opis drużyn robotów.
    
    Parametry:
    
    * ``image`` - bitmapa wejściowej ramki obrazu.
    * ``state`` - stan konfiguracji algorytmu - nie jest zmieniany w przebiegu
        algorytmu.
    * ``debug`` - struktura wskaźników do obrazków diagnostycznych, 
        NULL oznacza brak diagnostyki.


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

.. c:function:: void amv_config_init(struct amv_config* config)

    Inicjalizuje strukturę konfiguracyjną. Pola parametru zostają ustawione na 
    legalne zawartości, wskaźniki do struktur zewnętrznych zostają wyzerowane. 
    Nadpisywane są wszystkie parametry, z wyjątkiem wysokości i szerokości obrazka
     - te muszą zostać podane oddzielnie.
    
.. c:function:: void amv_debug_init(struct amv_debug_info*)
    
    Zeruje wskaźniki do wszystkich ramek diagnostycznych w strukturze.

.. c:function:: void amv_state_new(struct amv_state* st, struct amv_config* config)

    Przepisuje zawartość konfiguracji do zmiennej stanu. Cele pól wskaźnikowych
    nie są kopiowane, więc nie powinny być zwalniane do momentu zwolnienia struktury stanu.

.. c:function:: void amv_state_free(struct amv_state* state)

    Uwalnia pamięć używaną przez zmienną stanu.

