set term postscript eps enhanced "helvetica"
set output 'sig_tt.eps'
set size square 7.5/10.,1.
set xlabel 'r (m)' "helvetica,20"
set ylabel "{/Symbol s_{qq}}' (Pa)" "helvetica,20"
set key bottom right
set nologscale  
plot 't1.3.t1' us 1:($19+0.8*$4) title 't = 1.5e6 s' w l 1\
    ,'t1.3.t2' us 1:($19+0.8*$4) title 't = 50.e6 s' w l 1\
    ,'t1.3.t3' us 1:($19+0.8*$4) title 't = 300.e6 s' w l 1