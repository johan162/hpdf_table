all-local: html-local pdf-local
html-local:  out/html/index.html
pdf-local: out/latex/refman.pdf

DOC_FILES = APIOverview.md \
            CellAndRowApnning.md \
            ErrorHandling.md \
            GettingStarted.md \
            UsingWidgets.md \
            Building.md \
            ContentLabelCallbacks.md \
            FontAndStyle.md \
            Intro.md \
            TablesFromData.md \
            TableDesign.pptx

SRC_FILES = ../src/hpdftbl.c \
            ../src/hpdftbl.h \
            ../src/hpdftbl_grid.c \
            ../src/hpdftbl_widget.c \
            ../src/hpdftbl_errstr.c


if have_doxygen

out/html/index.html out/latex/refman.tex: Doxyfile.in $(SRC_FILES) $(DOC_FILES)
	doxygen

if have_pdflatex

out/latex/refman.pdf: out/latex/refman.tex
	cd out/latex && $(MAKE) $(AM_MAKEFLAGS)

else

out/latex/refman.pdf: out/latex/refman.tex
	touch $@

endif

else

out/html/index.html: Doxyfile.in $(SRC_FILES) $(DOC_FILES)
	touch $@

out/latex/refman.pdf: out/latex/refman.tex
	touch $@

endif

maintainer-clean-local:
	rm -rf out

EXTRA_DIST = out screenshots $(DOC_FILES)
