 
.. highlight:: xml

Schemat konfiguracji XML
------------------------

Poniżej znajduje się plik konfiguracyjny, zawierający wszystkie informacje 
potrzebne do rozpoznawania. Informacja zawarta w pliku odpowiada bezpośrednio
strukturze :c:type:`amv_config`.

::
    
    <?xml version="1.0" encoding="UTF-8"?>
    <amv_config>
            <teams>
                    <team>
                            <team_color>blue</team_color>
                            <home_team>true</home_team>
                            <robot>
                                    <secondary_colors>
                                            <first>green</first>
                                            <second>violet</second>
                                    </secondary_colors>
                            </robot>
                            <robot>
                                    <secondary_colors>
                                            <first>green</first>
                                            <second>red</second>
                                    </secondary_colors>
                            </robot>
                            <robot>
                                    <secondary_colors>
                                            <first>red</first>
                                            <second>violet</second>
                                    </secondary_colors>
                            </robot>
                            <robot>
                                    <secondary_colors>
                                            <first>red</first>
                                            <second>green</second>
                                    </secondary_colors>
                            </robot>
                            <robot>
                                    <secondary_colors>
                                            <first>violet</first>
                                            <second>red</second>
                                    </secondary_colors>
                            </robot>
                    </team>

Opis drużyny, której koszulki są zgodne ze wzorem opisanym w sekcji :ref:`koszulki`. Kolor ``first`` znajduje się po lewej 
stronie robota, kolor ``second`` z tyłu. Nazwy kolorów tu występujące są 
zdefiniowane w sekcji ``algorithm_parameters``.

::

    .
                    <team>
                            <team_color>yellow</team_color>
                            <home_team>false</home_team>
                            <robot></robot>
                            <robot></robot>
                            <robot></robot>
                            <robot></robot>
                            <robot></robot>
                    </team>
                    

Opis drużyny posiadającej nieznany wzór koszulek.

::
    
    .
            </teams>
            <algorithm_parameters>
                    <white_points>
                            <point>
                                    <x>1</x>
                                    <y>1</y>
                            </point>
                            <point>
                                    <x>2</x>
                                    <y>2</y>
                            </point>
                    </white_points>
                    
Punkty na obrazku w których znajdują się białe przedmioty.
                    
::

    .
                    <mask_points>
                            <point>
                                    <x>1</x>
                                    <y>1</y>
                            </point>
                            <point>
                                    <x>1</x>
                                    <y>1</y>
                            </point>
                    </mask_points>
                    
Punkty wyznaczające wielokąt w którym znajduje się boisko. Używany do maskowania
obiektów spoza boiska.
                    
::

    .
                    <px_per_cm>2.133333</px_per_cm>
                    <meanshift_radius>0</meanshift_radius>
                    <meanshift_threshold>40</meanshift_threshold>
                    <same_color_distance>50</same_color_distance>
                    <black_cutoff>55</black_cutoff>
                    <white_cutoff>250</white_cutoff>
                    <minimum_saturation>150</minimum_saturation>
                    
Parametry rozpoznawania - analogiczne do pól w :c:type:`amv_config`.
                    
::

    .
                    <colors>
                            <color>
                                    <name>blue</name>
                                    <min>85</min>
                                    <max>110</max>
                            </color>
                            <color>
                                    <name>yellow</name>
                                    <min>10</min>
                                    <max>40</max>
                            </color>
                            <color>
                                    <name>orange</name>
                                    <min>0</min>
                                    <max>20</max>
                            </color>
                            <color>
                                    <name>violet</name>
                                    <min>115</min>
                                    <max>140</max>
                            </color>
                            <color>
                                    <name>green</name>
                                    <min>69</min>
                                    <max>100</max>
                            </color>
                            <color>
                                    <name>red</name>
                                    <min>140</min>
                                    <max>53</max>
                            </color>
                    </colors>
                    
Definicje nazw kolorów używanych w sekcji ``teams``. Wartości ``min`` i ``max``
oznaczają minimalną i maksymalną wartość barwy (Hue).
                    
::

    .
            </algorithm_parameters>
    </amv_config>

