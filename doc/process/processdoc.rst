

Dokumentacja procesowa
======================

Motywacja
---------

..  
    TODO Laboratorium Robotów - tak to się zwie?

W Laboratorium Robotów znajduje się boisko i zestaw robotów 
umożliwiających rozgrywanie meczy piłki robotycznej zgodnie z regułami ligi 
FIRA MiroSot. Niemal wszystkie elementy niezbędne do rozegrania meczu znajdują
się na miejscu. Jedyne brakujące elementy to:

* zgodne z regułami FIRA kolorowe nakładki na roboty
* program rozpoznający drużyny robotów i piłkę na obrazie z kamery
* program decydujący o rozkazach dla robotów (AI)

Nasz projekt dostarcza pierwsze dwa z tych elementów. Głównym celem
jest skonstruowanie aplikacji rozpoznającej roboty na boisku, jednak dobór barw
na wierzchu robota ma bardzo duży wpływ na algorytm rozpoznawania. W związku z 
tym konieczne jest również znalezienie wzoru koszulek dla zawodników.

W Laboratorium Robotów wykorzystywany był już system wizyjny oparty na kamerze 
znajdującej się nad boiskiem. Wykorzystywał on jednak koszulki niezgodne z 
regulaminem ligi i był bardzo podatny na wszelkie zmiany oświetlenia i 
otoczenie boiska. System ten wykorzystywał własną bibliotekę komunikacji z 
kamerą i komunikował się z elementami sterującymi robotami 
za pomocą pakietów multicastowych UDP. Sposób komunikacji z kamerą i programami
sterującymi powinny zostać zachowane.

Głównym celem projektu jest skonstruowanie aplikacji rozpoznającej pozycję
robotów zgodnie z regułami ligi FIRA MiroSot i w warunkach dostępnych w
Laboratorium Robotów w sposób umożliwiający użycie jej w rzeczywistych
rozgrywkach.

Zakres funcjonalności
---------------------

Użytkownikiem produktu będzie operator, którego zadaniem po uruchomieniu
programu jest ustawienie parametrów rozpoznawania. W skład parametrów algorytmu
wchodzą informacje na temat ilości i typu robotów w drużynach i informacje na 
temat warunków oświetleniowych. 

System współpracuje z:

* biblioteką do współpracy z kamerą internetową --- jest to biblioteka napisana 
  na potrzeby poprzedniego systemu lokalizacji robotów. Biblioteka jest
  wkompilowywana do aplikacji.
* programami sterującymi robotami, które są konsumentami informacji. Protokół 
  komunikacyjny jest oparty na UDP i jest odziedziczony po poprzednim systemie
  lokalizacji robotów. 


Wymaganiami funkcjonalnymi systemu są:

* Współpraca z kamerami internetowymi w możliwie ogólny sposób
* Możliwość łatwej kalibracji parametrów algorytmu w czasie działania
* Możliwość współpracy z istniejącymi systemami sterującymi robotami
* Możliwość przekazywania danych o pozycji robotów do dwóch programów
  sterujących drużynami jednocześnie
* Możliwość użycia aplikacji w warunkach oświetleniowych niezgodnych z regułami
  gry, w tym w zmiennych warunkach oświetleniowych
* Możliwość użycia aplikacji na boisku o wymiarach niezgodnych z regułami gry 
  i z kamerą w odległości od boiska niezgodnej z regułami gry



Wymaganiami niefunkcjonalnymi są:

* Praca z szybkością 30fps przy możliwie niewielkim wykorzystaniu procesora


Weryfikacja
-----------

..
    Identyfikator       Nazwa przypadku użycia .
    Opis    Opis słowny działań podejmowanych przez przypadek testowych.
    Warunki wstępne Warunki wstępne konieczne do przeprowadzenia testu.
    Procedura testowa       1. Krok pierwszy testu
    2. Krok drugi testu
    3. Nie należy wdawać się w szczegóły kodowania.
    Oczekiwane rezultaty    Wystąpienie wyjątku lub realizacja celu opisanego w Opisie. 

W toku prac zostały przeprowadzone następujące testy:

.. tabularcolumns:: |l|J|

======  ========================================================================
Nazwa   Test efektywności lokalizacji
======  ========================================================================
Opis    Na zebranych 40 zdjęciach z kamery można sprawdzić jakość lokalizacji
        robotów i piłki. Na każdym zdjęciu znajdowało się 10 robotów.

Wyniki  Istniały ustawienia parametrów, dla których rozpoznanie pozycji i
        kąta robotów wyniosło 99.75%, a rozpoznanie piłki 100%,
        bez widocznych błędów w identyfikacji poszczególnych robotów w drużynie.
======  ========================================================================

.. tabularcolumns:: |l|J|

======  ========================================================================
Nazwa   Syntetyczny test wydajności
======  ========================================================================
Opis    Algorytm lokalizacji został uruchomiony na przykładowym obrazku w pętli
        1000 razy.

Wyniki  Podczas ostatniego testu, przeprowadzanego na komputerze z procesorem 
        ``Intel Core i5-2410M 2.30GHz`` wydajność wyniosła  około 9ms na 
        pojedynczą klatkę obrazu.
======  ========================================================================

.. tabularcolumns:: |l|J|

======  ========================================================================
Nazwa   Kompatybilność z kamerą
======  ========================================================================
Opis    Aplikacja powinna współpracować bezoroblemowo z kamerą dostępną w 
        Laboratorium Robotów (Phillips spc1300NC)

Wyniki  Program połączył się z kamerą natychmiast po włączeniu, nie było 
        konieczne przeprowadzanie żadnej dodatkowej konfiguracji
======  ========================================================================

.. tabularcolumns:: |l|J|

======  ========================================================================
Nazwa   Odporność na zmianę oświetlenia
======  ========================================================================
Opis    Algorytm powinien działać poprawnie w różnych warunkach oświetleniowych
        (przy różnej kombinacji zapalonych lamp).

Wyniki  Zmiana jasności i barwy oświetlenia w niewielkim zakresie nie wpływa 
        na zdolność lokalizacji robotów. Zmiana w zakresie wywołującym 
        prześwietlenie lub niedoświetlenie obrazu wymaga interwencji w postaci
        zmiany długości czasu naświetlania w sterowniku kamery.
======  ========================================================================

.. tabularcolumns:: |l|J|

======  ========================================================================
Nazwa   Odporność na lokalne zmiany oświetlenia
======  ========================================================================
Opis    Osoby przechodzące obok boiska mogą rzucać cienie, które mogą wpływać na
        przebieg lokalizacji. Należy sprawdzić ten wpływ.

Wyniki  Osoby znajdujące się przy krawędzi boiska nie mają wpływu na wynik 
        lokalizacji.
======  ========================================================================

.. tabularcolumns:: |l|J|

======  ========================================================================
Nazwa   Test wydajności w warunkach rzeczywistych
======  ========================================================================
Opis    Wydajność aplikacji została sprawdzona w Laboratorium Robotów, przy 
        użyciu znajdującego się tam komputera.

Wyniki  Czas przetwarzania pojedynczej klatki wynosi około 50ms (co daje 20 
        klatek na sekundę). 
======  ========================================================================


.. tabularcolumns:: |l|J|

======  ========================================================================
Nazwa   Test efektywności w warunkach rzeczywistych
======  ========================================================================
Opis    Sprawdzono jakość lokalizacji w Laboratorium Robotów.

Wyniki  Poprawność rozpoznawania pozycji robotów jest praktycznie stuprocentowa.
        Wyznaczanie kąta obrotu jest mniej efektywne -- zdarzają się niepoprawne
        rozpoznania. Błędy nie występują zawsze, ale mogą pojawiać się roboty, 
        które będąc w specyficznej pozycji co pewną liczbę klatek otrzymują 
        błędny kąt obrotu. Błędy w rozpoznaniu można redukować dokładniejszym 
        doborem parametrów algorytmu.
======  ========================================================================
