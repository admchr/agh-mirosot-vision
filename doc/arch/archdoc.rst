

Dokumentacja techniczna
=======================

..
    Tu:

    * Phillips spc1300NC
    * opis przepływu danych w algorytmie
    * można dużo mówić, dlaczego coś zostało zrobione tak, jak zostało
    * problemy z balansem bieli, z kolorami, z jasnością, ze sklejaniem się robotów,
    * jakie są cele i założenia
    możliwe różne podejścia, które zostały odrzucone
    * co było wcześniej, jak wyglądał robotyczny rozpoznawacz wczoraj
    * że obrazki są w BGR i że to wygląda na standardowy sposób zapisu (nie RGB)
    * co można poprawić:
    - można spróbować robić więcej w jednym przebiegu po obrazku, zmniejszyć
        wielkość wszelkich struktur dodatkowych, eliminować alokację
    - poprawić rozpoznawanie orientacji trójkąta, znikające punkty z 3-kąta
    - delayed white balance - napierw wyliczamy balans, potem z niego korzystamy
    - UI... ... ...

    TODO

.. toctree::

    algodoc
    generaldoc

.. [Hao] Cui Hao, Denghua Li, Shuhua Peng, "Research on Designing of Soccer Robot's Color Tag and Identifying Algorithm", Intelligent Human-Machine Systems and Cybernetics, 2009. IHMSC '09. International Conference on 2009

.. [Jiang] Jiang H, Q Peng, HA Lee, EL C Teoh, HL Sng, "Colour Vision and Robot Ball Identification for a Large Field Soccer Robot System", 2nd International Conference on Autonomous Robots and Agents, 2004

