set terminal png size 800,400
set out "white0.png" 

set size 1.0, 1.0
set origin 0.0, 0.0
set multiplot

set size 0.5, 1.0
set origin 0.0, 0.0

set grid

set title "rzeczywiste odwzorowanie koloru"
set key left top
set xrange [-10:110]
set yrange [-10:170]

set xlabel "jasność[%]"
set ylabel "wartość piksela (R+G+B)/3"

plot "white.txt" with errorbars t "punkty testowe"

set size 0.5, 1.0
set origin 0.5, 0.0

set title "model sRGB"
set key left top
set xrange [-10:266]
set yrange [-10:266]

set xlabel "wartość piksela RGB"
set ylabel "wartość piksela sRGB"

plot (1.055*((x/256)**(1/2.4))-0.055)*256 with line t ""
