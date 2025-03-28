#!/usr/bin/gnuplot -persist
#
reset
set term postscript eps enhanced color
# Options
set termoption solid
set termoption lw 2
set termoption font "Times-Roman,26"


# 1st plot
set output "Yuan2vol.eps"


# Main file
mainfile = 'Yuan2b'

# Data files
file1 = mainfile.'.t1'



# x-axis
set xlabel "Distance to the surface (mm)" 
set xrange [ * : 10] noreverse nowriteback  # (currently[0.151196:0.304308] )
set xtics 
set mxtics 2


# y-axis
set ylabel "Solid volume (L)" offset 2.5,0
set y2label "Ca/Si"  offset -2,0
set yrange [ * : 2 ] noreverse nowriteback    # (currently [0.411707:0.459901] )
set y2range [ * : * ] noreverse nowriteback   # (currently [0.411707:0.459901] )
set ytics 
set ytics nomirror
set y2tics nomirror 
set y2tics
set mytics 2
set my2tics 2



set lmargin 5
set bmargin 3.5
set rmargin 6
set tmargin 1


set title ""
set title  offset character 0, 0, 0 font "" norotate
set key title "Method 1 (10 days)"

set key samplen 2
set key font ",24" at first 9.5, first 1.8

#set label front textcolor rgb "white" font ",22" "Gypsum" at first 5, first 0.2
#set key spacing 1.5
set border  lw 2

set label "Attack direction" font ",22" at first 1.3, first 1.94
set arrow from 1.3,1.88 to 3.9,1.88  lt 1 lc 0 lw 2

set arrow front from 1.25,0 to 1.25,2 nohead lt 1 lc 0 lw 2
set label "D_{Simulation}" at first 1.8, first 1.3
set arrow from 1.6,1.25 to 1.3,1.15  lt 10 lc 0 lw 2


plot file1 us (100-$1*100):($23*$31) with filledcurves above y1=0 linecolor rgbcolor "green" title 'C-S-H'\
    ,file1 us (100-$1*100):($23*$31):($23*$31+0.033*$21) with filledcurves linecolor rgbcolor "red" title 'Portlandite'\
    ,file1 us (100-$1*100):($23*$31+0.033*$21):($23*$31+0.033*$21+0.075*$22) with filledcurves linecolor rgbcolor "gray" title 'Gypsum'\
    ,file1 us (100-$1*100):32 title "Ca/Si ratio" w l lt 1 lw 2 lc rgb "blue" axis x1y2 
    
    
reset
#    EOF
