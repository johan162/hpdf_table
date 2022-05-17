DOC_FILES = APIOverview.md \
            CellAndRowSpanning.md \
            ErrorHandling.md \
            GettingStarted.md \
            UsingWidgets.md \
            Building.md \
            ContentLabelCallbacks.md \
            FontAndStyle.md \
            Themes.md \
            Intro.md \
            TablesFromData.md \
            TableDesign.pptx \
            Serializing.md

SRC_FILES = ../src/hpdftbl.c \
            ../src/hpdftbl.h \
            ../src/hpdftbl_grid.c \
            ../src/hpdftbl_widget.c \
            ../src/hpdftbl_errstr.c \
            ../src/hpdftbl_callback.c \
            ../src/hpdftbl_theme.c \
            ../src/hpdftbl_load.c \
            ../src/hpdftbl_dump.c \
            ../src/xstr.c \
            ../src/read_file.c \
            ../scripts/bootstrap.sh \
            ../scripts/stdbld.sh \
            ../scripts/dbgbld.sh \
            ../scripts/docupload.sh.in

if have_doxygen

html-local: out/html/index.html

maintainer-clean-local:
	rm -rf out

out/html/index.html: Doxyfile.in $(SRC_FILES) $(DOC_FILES)
	doxygen

if have_pdflatex

# all-local: html-local pdf-local
pdf-local: out/latex/refman.pdf
out/latex/refman.pdf: out/latex/refman.tex
	cd out/latex && $(MAKE) $(AM_MAKEFLAGS)

else

# all-local: html-local

endif

endif

EXTRA_DIST = out screenshots doxygen-awesome-css $(DOC_FILES)
