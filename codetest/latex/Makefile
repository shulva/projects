objects := $(wildcard *.tex)

tex: $(objects)
	xelatex $(objects)

cleanall:cleanaux cleanlog cleanpdf
.PHONY:cleanall
cleanaux:
	rm *.aux
cleanlog:
	rm *.log
cleanpdf:
	rm *.pdf
