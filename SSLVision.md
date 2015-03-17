#System wizyjny dla RoboCup

http://code.google.com/p/ssl-vision/

Pluginy tworzące cały pipeline są w src/app/plugins
  * W pierwszym kroku działania jest chyba jakas korekcja barw.
  * Program konwertuje kolory na określoną paletę,
  * komresuje ją Run-Length, plugin\_runlength\_encode
  * oznacza spójne fragmenty. plugin\_find\_blobs
  * Następnie żółte i niebieskie fragmenty są testowane na obecność wokół nich innych kolorowych obiektów, "markerów". Makkery sortuje się po kącie, a następnie jakiś bardziej inteligentny algorytm sprawdza jaki robot ma mieć jaki zestaw markerów. plugin\_find\_robots TeamDetector::findRobotsByModel
  * piłka jest znajdowana
  * i tyle

Konwersja na paletę podpiera się kostką barwną w palecie YCbCr, w której zaznacza się obszary dla danego wzorca koloru. Żeby w GUI dostać się do innych jasności trzeba użyć rolki.

Nieudane były wszelkie próby uruchomienia tego tałatajstwa na jakichś danych testowych. Nie reaguje na obrazki jpeg, png.