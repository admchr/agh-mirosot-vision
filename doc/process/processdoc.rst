

Dokumentacja procesowa
======================

..
    Tu:

    * wizja
    * koncepcja

    to wszystko jest mało zrozumiałe, bo co tam ma być?

Założenia
---------

..  
    TODO factual accuracy

W Laboratorium Robotów znajduje się boisko i zestaw ponad 10 robotów 
umożliwiających rozgrywanie meczy piłki robotycznej zgodnie z regułami ligi 
FIRA. Niemal wszystkie elementy niezbędne do rozegrania meczu znajdują się na 
miejscu. Jedyne brakujące elementy to:

* zgodne z regułami FIRA kolorowe nakładki na roboty
* program rozpoznający drużyny robotów na obrazie z kamery
* program decydujący o rozkazach dla robotów (AI)

Nasz projekt dostarcza pierwsze dwa z tych elementów. Głównym celem
jest skonstruowanie metody rozpoznawania robotów na boisku, jednak dobór barw
na wierzchu robota ma bardzo duży wpływ na algorytm rozpoznawania. Dlatego 
algorytm rozpoznawania i projekt barw drużyny są w zasadzie nierozerwalne - 
efektywy algorytm będzie wykorzystywał własności obiektów, które rozpoznaje.

