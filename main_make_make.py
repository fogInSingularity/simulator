# sep_str = "# ------------------------------------------------------------------------------\n"

clean_cmd = "rm build/*"

main_make_str = """
all:
\t@echo 'available options> release debug clean'

release:
\t@echo 'Compiler CXX=$(CXX) CC=$(CC)'
\tmake -f release.mk CXX=$(CXX) CC=$(CC) -j8

debug:
\t@echo 'Compiler CXX=$(CXX) CC=$(CC)'
\tmake -f debug.mk CXX=$(CXX) CC=$(CC) -j8

analyze:
\tmake -f analyze.mk

clean:
\t{}
""".format(clean_cmd)
