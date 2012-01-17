.. highlight:: c

Aplikacja graficzna
-------------------

Opis programu i wymagania systemowe
***********************************

Aplikacja graficzna do obsługi biblioteki rozpoznającej została zaimplementowana w środowisku Windows(.NET) w języku Visual C++. Oprócz środowiska .NET podczas implementacji aplikacji wykorzystano także biblioteki:

* Xerces ( http://xerces.apache.org/xerces-c/ ) jako biblioteke obsługującą operacje na zapisanej w formacie XML konfiguracji
* OpenCV jako zależność dla biblioteki rozpoznającej oraz wsparcie dla podstawowych operacji na ramkach obrazu (kopiowanie, zapis)
* Odziedziczoną bibliotekę do pobierania obrazu z kamery

oprócz binariów powyższych bibliotek do poprawnego działania aplikacja wymaga zainstalowanych:

* kamery zgodnej z systemem Microsoft Windows
* Microsoft Visual Studio 2010 Redistributable Package
* Microsoft .NET Framework 2.0

Opis działania aplikacji
************************

Uruchamianie
^^^^^^^^^^^^

Pierwszym krokiem po uruchomieniu aplikacji jest wybór kamery. Jeśli w systemie dostepne jest wiecej niż jedno odpowiednie urządzenie pojawia się okno pozwalające na wybór właściwej kamery. Okno to jest pokazane na poniższym rysunku. Jeśli w systemie jest dostępne tylko jedno urządzenie do akwizycji obrazu wyświetlenie okna wyboru kamery zostanie pominięte. Jeśli w systemie nie ma dostępnej kamery, zostanie wyświetlony odpowiedni komunikat a aplikacja zakończy swoje działanie.

.. image:: /cameraChoice.png
    :align: center
    :width: 377pt
    :height: 201pt
    
Po wskazaniu właściwej kamery pojawia się okno wyboru opcji dla wybranego urządzenia. Zalecanymi wartościami do ustawienia na tym etapie są:

* Szybkość klatek: 30.00
* Przestrzeń kolorów / kompresja: RGB 24
* Rozmiar wyjściowy: 640x480

.. image:: /cameraOptions.png
    :align: center
    :width: 394pt
    :height: 302pt
    
Sterowanie wykonaniem programu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. image:: /aplicationWindow.png
    :align: center
    :width: 353pt
    :height: 211pt

Po zatwierdzeniu opcji kamery pojawia się główne okno aplikacji. Na tym etapie można uruchomić algorytm rozpoznawania robotów z użyciem domyślnie załadowanych przez aplikację opcji. Uruchomienie rozpoznawania robotów powoduje równocześnie rozgłaszanie wyników rozpoznawania na adres multicastowy za pomocą pakietów UDP. Do rozpoczęcia/wstrzymania rozpoznawania służy para przycisków "Start" i "Stop". W tej samej grupie przycisków dostępne mamy rownież: "Restet Camera", "Capture Image" oraz "Save Config" i "Load Config". Za ich pomocą możemy odpowiednio zmienić kamerę/opcje kamery, pobrać i zapisać jako plik graficzny pojedynczą ramkę obrazu z aktualnie podłączonego urządzenia, zapisać aktualną konfigurację algorytmu jako plik XML lub wczytać wcześniej przygotowaną/zapisaną konfiguracje z pliku XML.

.. image:: /controlButtons.png
    :align: center
    :width: 237pt
    :height: 52pt
    
Do zmiany adresu multicast i portu, na który rozgłaszane są wyniki działania algorytmu, służą dwa pola tekstowe w prawym górnym rogu okna programu opisane odpowiednio:

* Multicast address
* Multicsat port

Prezentacja wyników
^^^^^^^^^^^^^^^^^^^

Poza rozsyłaniem wyników na adres multicastowy aplikacja wyświetla bieżące wyniki rozpoznawania. Używa do tego celu następującego zestawu kontrolek:

.. image:: /resultsArea.png
    :width: 325pt
    :height: 273pt

* Robots found - oznacza ilość znalezionych robotów w każdej z drużyn.
* Na znajdujących się poniżej listach są wyświetlane pozycje i kąty obrotu kolejnych znalezionych w danej ramce obrazu robotów.
* Ball position - prezentuje aktualną pozycję piłki na boisku.
* Last frame processing time(s) - jest to czas(w sekundach) jaki zabrało przetworzenie ostatniej ramki obrazu przez algorytm rozpoznawania.

Prezentacja wyników częściowych
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Aby obserwować obraz boiska w trakcie działania algorytmu rozpoznawania i oszacować jakość rozpoznawania robotów w okno programu został wkomponowany obszar do wyswietlania zdjęć. Wyboru aktualnie wyświetlanego zdjęcia możemy dokonać zaznaczjąc odpowiedni radiobutton. Poniższa lista przedstawia znaczenie poszczególnych obrazów.

.. image:: /radiobuttons.png
    :width: 89pt
    :height: 173pt

* None -  brak wyświetlania obrazu - w obszarze zdjęcia będzie wyświetlony ostatnio ustawiony obraz
* Original - obszar zdjęcia będzie wyświetlał aktualny, nieprzetworzony obraz z kamery
* White balance - obszar zdjęcia będzie wyświetlał obraz boiska po wykonaniu balansu bieli przez algorytm. Czerwonymi krzyżykami zostały zaznaczone punkty, które algorytm traktował jako bazowe punkty bieli
* Color classes - Na tym obrazie kolorem żółtym, niebieskim, białym oraz pomarańczowym zaznaczone są obszary które zostały zaklasyfikowane do odpowiednich klas kolorów. Podczas konfigurowania algorytmu należy zwrócić szczególną uwagę na jakość(pokrywanie i kształt) obszarów niebieskich i źółtych prezentowanych na tym obrazie.
* Meanshift - obszar zdjęcia będzie wyświetlał obraz boiska po wykonaniu algorytmu meanshift
* Patches - Na tym obrazie zaznaczone będą obszary klasyfikowane jako spójny obszar jednego koloru
* Robots - Na tym obrazie zaznaczone będą kolory drużynowe robotów oraz dla robotów o znanym wzorze koszulki klasyfikacja kolorów pobocznych.
* Results - Obraz będzie przedstawiał wyniki rozpoznawania:

    * Każdy robot będzie reprezentowany za pomocą ramki odpowiednich kolorów, roboty z drużyny o znanych koszulkach będą miały zaznaczone kolory poboczne jako krawędzie ramki
    * piłka zostanie oznaczona jako pomarańczowy krzyżyk
    * za pomocą białego prostokąta zostanie zaznaczony obszar, który wyznacza wyjściowy układ współrzędnych robotów i piłki
    
Dla obrazków: Color classes, Meanshift, Patches, Robots oraz Results obszary poza wielokątem zainteresowania algorytmu są zaczernione.

Zmiana parametrów wykonania algorytmu - dostosowywanie kolorów
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Pozostałe kontrolki okna programu pozwalają na bezpośrednie modyfikowanie opcji algorytmu.

Kluczowymi opcjami algorytmu jest zmiana zakresów klas kolorów klasyfikowanych jako części poszczególnych identyfikowanych obiektów. Klasyfikacja kolorów następuje za pomocą przedziału barwy koloru(Hue). Dla kazdego z kolorów głównych:

* Blue (Niebieski)
* Yellow (Żółty)
* Orange (Pomarańczowy)

oraz kolorów pobocznych:

* Green (Zielony)
* Red (Czerwony)
* Violet (Fioletowy)

mozliwe jest dokładne dopasowanie zakresu barwy klasyfikowanego jako dany kolor. Do tego celu służą kontrolki:

.. image:: /colors.png
    :align: center
    :width: 458pt
    :height: 192pt

Przypomnijmy że skalę kolorów traktujemy cyklicznie modulo 180 stąd poprawnym jest przedział:

* Minimum 150
* Maximum 45

Ważnymi dla klafyfikacji kolorów obrazu są także parametry:

.. image:: /otherControlsColors.png
    :width: 195pt
    :height: 126pt

* Minimum lightness - Jasność, poniżej której piksel uważa się za zbyt ciemny. Ta wartość jest używana do rozpoznawania kolorów drużynowych.
* Maximum lightness - Jasność, powyżej której piksel uważa się za biały.
* Minimum saturation - Minimalne nasycenie koloru, który może być zakwalifikowany jako żółty, pomarańczowy lub niebieskiego.
* Same color distance - odległość (euklidesowa) barwy koloru poniżej której dwa kolory zostaną zaklasyfikowane jako ten sam kolor podczas wyznaczania spójnych obszarów tego samego koloru.

Zmiana parametrów wykonania algorytmu - obszary przetwarzania
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. image:: /chooseButtons.png
    :align: center
    :width: 145pt
    :height: 86pt

Powyższe opcje można zmieniać wyłącznie kiedy algorytm nie jest uruchomiony. Przyciśnięcie któregoś z wyżej wymienionych przycisków spowoduje pobranie i wyświetlenie aktualnej ramki obrazu z kamery i umożliwi zaznaczanie kursorem punktów na obszarze zdjęcia. Aby zakonczyć zaznaczanie punktów na obszarze obrazka należy ponownie wcisnąć wybrany przycisk.

* Choose field scale boundary - wybranie tej opcji pozwoli na zaznaczenie narożników układu współrzednych boiska. Należy zaznaczyć dokładnie dwa punkty: Pierwszy będzie wskazywał wynikową współrzędną (0.0, 0.0) zaś drugi bedzie oznaczał przeciwległy róg boiska, a jego współrzędne zostaną określone zgodnie z wartościami pól:

	* Field scale X
	* Field scale Y
	
.. image:: /fieldScale.png
    :width: 194pt
    :height: 49pt
    
* Choose white points - wybranie tej opcji pozwala na wskazanie punktów które algotytm ma uznać za bazowe punkty bieli.
* Choose mask points - wybranie tej  opcji pozwala na wskazanie wieszchołków wielokąta, którego obszar ma być brany pod uwagę podczas działania algorytmu jako możliwe miejsce występowania robotów.

Zmiana parametrów wykonania algorytmu - pozostałe parametry
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. image:: /otherControlsMeanshift.png
    :width: 197pt
    :height: 126pt
    
* Yellow team size - parametr oznaczający ilośc robotów żółtej drużyny znajdujących się na boisku.
* Blue team size - parametr oznaczający ilośc robotów niebieskiej drużyny znajdujących się na boisku.
* Meanshift radius - Rozmiar okna algorytmu wygładzania powierzchni meanshift. Im większe okno, tym większe otoczenie piksela jest brane pod uwagę przy jego wygładzaniu. Rozmiar okna może mieć duży wpływ na wydajność.
* Meanshift threshold - Odległość w normie euklidesowej kolorów, które są uważane za różne podczas wykonywania algorytmu wygładzania meanshift.
* Pixels/Cm - Rozdzielczość obrazu na powierzchni boiska (w pikselach na centymetr).


