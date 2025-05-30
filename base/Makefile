#
SHELL = /bin/sh

ifndef BIL_PATH
#CURDIR = ${shell pwd}
#BIL_PATH = ${word 1,${subst /base, ,${CURDIR}}}
BIL_PATH = ${shell realpath ../}
endif

BASEDIR = ${BIL_PATH}/base

#ALL_BASE_DIRS = $(wildcard m* M*)
ALL_BASE_DIRS = ${subst /,,${shell ls -d */}}
#ALL_BASE_DIRS = ${notdir ${shell ls -d */}}
#ALL_BASE_DIRS = ${notdir ${shell find . -type d}}
#ALL_BASE_DIRS = ${shell find . -type d}


include ${BIL_PATH}/src/Models/ListOfModels.inc

COMMA := ,
EMPTY :=
SPACE := ${EMPTY} ${EMPTY}
MODELS := ${SELECTEDMODELS} 
MODELS += ${addsuffix -%,${SELECTEDMODELS}}
BASE_DIRS := ${filter ${MODELS},${ALL_BASE_DIRS}}

DIFF_FILE = diff

# Diff command
#LLENGTH := ${word 1,${shell wc -L ${MODELS} | tail -1}}
#LLENGTH := 400
#DIFF := diff --suppress-common-lines -E -b -y --width=${LLENGTH}
DIFF := diff --suppress-common-lines -E -b



all: exec diff




#=======================================================================
# Target rules for diff
#=======================================================================

diff: diff1 diff2



.IGNORE: diff1
diff1: 
	rm -f ${DIFF_FILE}.t1
	for i in ${BASE_DIRS} ; do (\
	cd $$i &&\
	echo "-- diff on $$i.t1 --" &&\
	echo "-- diff on $$i.t1 --" >> ${BASEDIR}/${DIFF_FILE}.t1 &&\
	${DIFF} $$i.t1 toto.t1 >> ${BASEDIR}/${DIFF_FILE}.t1 \
	); done


.IGNORE: diff2
diff2:
	rm -f ${DIFF_FILE}.t2
	for i in ${BASE_DIRS} ; do (\
	cd $$i &&\
	echo "-- diff on $$i.t2 --" &&\
	echo "-- diff on $$i.t2 --" >> ${BASEDIR}/${DIFF_FILE}.t2 &&\
	${DIFF} $$i.t2 toto.t2 >> ${BASEDIR}/${DIFF_FILE}.t2 \
	); done




#=======================================================================
# Target rules for exec
# To pass the variable "-r" to make, use: make exec opt=-r
#=======================================================================

exec: toto iperm
	for i in ${BASE_DIRS}; do (\
	cd $$i && \
	echo "\n\nExecution of $$i:" && \
	echo "-----------------------------------------" && \
	bil ${opt} toto \
	); done ; echo "\n\n\n";



iperm: toto
	for i in ${BASE_DIRS}; do (\
	cd $$i && \
	echo "\n\nExecution of $$i:" && \
	echo "-----------------------------------------" && \
	bil -i toto \
	); done ; echo "\n\n\n";



ref:
	for i in ${BASE_DIRS}; do (\
	cd $$i && \
	echo "Execution of $$i" && \
	bil $$i \
	); done



toto: clean
	for i in ${BASE_DIRS}; do (\
	cd $$i && \
	echo "\n\nCopy $$i in toto:" && \
	echo "-----------------------------------------" && \
	cp $$i toto \
	); done ; echo "\n\n\n";

#=======================================================================
# Target rules for execfem
# To pass the variable "-r" to make, use: make exec opt=-r
#=======================================================================



BASEFEM_DIRS := Elast M10-2 M10-3 MechaMic Niko_1 Niko_2 Plast Poroplast usoil
execfem: iperm
	for i in ${BASEFEM_DIRS}; do (\
	cd $$i && \
	echo "\n\nExecution of $$i:" && \
	echo "-----------------------------------------" && \
	bil ${opt} toto \
	); done ; echo "\n\n\n";



plotdifft1fem:
	for i in ${BASEFEM_DIRS}; do (\
	echo "Plot $$i.t1 and toto.t1" && \
	cd $$i && \
	../../scripts/bilplotdiff.sh $$i.t1 toto.t1 ${opt} \
	); done




#=======================================================================
# Target rules for clean
#=======================================================================

clean:
	rm -f ${DIFF_FILE}.t*
	for i in ${ALL_BASE_DIRS}; do (\
	cd $$i && \
	rm -f *~ *.eps toto* *.sto \
	); done



#=======================================================================
# Target rules for plotdiffti
# Usage: make plotdifft1 opt=pause
#=======================================================================

plotdifft1:
	for i in ${BASE_DIRS}; do (\
	echo "Plot $$i.t1 and toto.t1" && \
	cd $$i && \
	../../scripts/bilplotdiff.sh $$i.t1 toto.t1 ${opt} \
	); done



plotdifft2:
	for i in ${BASE_DIRS}; do (\
	echo "Plot $$i.t1 and toto.t1" && \
	cd $$i && \
	../../scripts/bilplotdiff.sh $$i.t2 toto.t2 ${opt} \
	); done



plotdiffp:
	for i in ${BASE_DIRS}; do (\
	echo "Plot $$i.p1 and toto.p1" && \
	cd $$i && \
	../../scripts/bilplotdiff.sh $$i.p1 toto.p1 ${opt} \
	); done



BASE1 = MTD1 Shen
plotdifft11:
	for i in ${BASE1}; do (\
	echo "Plot $$i.t1 and toto.t1" && \
	cd $$i && \
	gnuplot -e "call '${BASEDIR}/diff.gp' \"$$i.t1\" \"toto.t1\" " \
	); done



plotdifft21:
	for i in ${BASE1}; do (\
	echo "Plot $$i.t1 and toto.t1" && \
	cd ${BASEDIR}/$$i && \
	gnuplot -e "call '${BASEDIR}/diff.gp' \"$$i.t2\" \"toto.t2\" " \
	); done



plotdiffp1:
	for i in ${BASE1}; do (\
	echo "Plot $$i.p1 and toto.p1" && \
	cd $$i && \
	gnuplot -e "call '${BASEDIR}/diffp.gp' \"$$i.p1\" \"toto.p1\" " \
	); done





#=======================================================================
# Target rules for readonly
#=======================================================================

readonly: toto
	for i in ${BASE_DIRS}; do (\
	cd $$i && \
	echo "\n\nReading of $$i:" && \
	echo "-----------------------------------------" && \
	bil -r toto \
	); done ; echo "\n\n\n";







#=======================================================================
# Other target rules
#=======================================================================

list:
	for i in ${BASE_DIRS}; do (\
	echo "\n\n\n" && \
	echo "List $$i" && \
	echo "--------\n" && \
	cat $$i/$$i \
	); done



work1:
	for i in ${BASE_DIRS}; do (\
	cd $$i && \
	sed 's/Time Steps$^$/Time Steps/g' $$i > $$i.bak && \
	mv -f $$i.bak toto \
	); done



grep:
	for i in ${BASE_DIRS}; do (\
	cd $$i && \
	echo "$$i" && \
	grep "Geometry" $$i \
	); done



test:
	@echo "ALL_BASE_DIRS = " ${ALL_BASE_DIRS}
	@echo ""
	@echo "BASE_DIRS = " ${BASE_DIRS}
	@echo ""
	@echo "SELECTEDMODELS = " ${SELECTEDMODELS}
	@echo ""
	@echo "MODELS = " ${MODELS}
	@echo ""
	@echo "Maximum line length = " ${LLENGTH}
	for i in ${BASE_DIRS} ; do (\
	cd $$i && \
	echo "folder = " $$i &&\
	${eval LL := ${word 1,${shell wc -L $$i.t1}}} \
	echo "Maximum line length = " $${LL} &&\
	echo "Maximum line length = " ${shell wc -L $$i.t1} \
	); done





TEST_DIRS = Yuan1 Elast
test1: 
	rm -f ${DIFF_FILE}.t1
	for i in ${TEST_DIRS} ; do (\
	cd $$i &&\
	echo "-- diff on $$i.t1 --" &&\
	echo "-- diff on $$i.t1 --" >> ${BASEDIR}/${DIFF_FILE}.t1 &&\
	${DIFF} $$i.t1 toto.t1 >> ${BASEDIR}/${DIFF_FILE}.t1 \
	); done



F2 = Elast
test2: 
	rm -f ${DIFF_FILE}.t1
	cd ${F2} &&\
	${DIFF} ${F2}.t1 toto.t1

