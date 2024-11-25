sep = "\n\n# ------------------------------------------------------------------------------\n\n"

def_frags = """
WARNINGS = -Wall -Wextra
# -Wswitch-enum
ifeq ($(CXX), clang++)
\tWARNINGS += -pedantic -Wconversion -Wdangling -Wdeprecated -Wdocumentation -Wformat -Wfortify-source -Wgcc-compat -Wgnu -Wignored-attributes -Wignored-pragmas -Wimplicit -Wmost -Wshadow-all -Wthread-safety -Wuninitialized -Wunused -Wformat
\tWARNINGS += -Wargument-outside-range -Wassign-enum -Wbitwise-instead-of-logical -Wc23-extensions -Wc11-extensions -Wcast-align -Wcast-function-type -Wcast-qual -Wcomma -Wcomment -Wcompound-token-split -Wconditional-uninitialized -Wduplicate-decl-specifier -Wduplicate-enum -Wduplicate-method-arg -Wduplicate-method-match -Wempty-body -Wempty-init-stmt -Wenum-compare -Wenum-constexpr-conversion -Wextra-tokens -Wfixed-enum-extension -Wfloat-equal -Wloop-analysis -Wframe-address -Wheader-guard -Winfinite-recursion -Wno-gnu-binary-literal -Wint-conversion -Wint-in-bool-context -Wmain -Wmisleading-indentation -Wmissing-braces -Wmissing-prototypes -Wover-aligned -Wundef -Wvla
endif
ifeq ($(CXX), cc)
\tWARNINGS += -pedantic -Waggressive-loop-optimizations -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wopenmp-simd -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wswitch-default -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-varargs -Wstack-usage=8192 -Wstack-protector
endif
ifeq ($(CXX), g++)
\tWARNINGS += -pedantic -Waggressive-loop-optimizations -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wopenmp-simd -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wswitch-default -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-varargs -Wstack-usage=8192 -Wstack-protector
endif

STDCPP_VERSION = -std=c++17
STDC_VERSION = -std=c11 
FLAGS = -fstack-protector-strong -fcheck-new -fstrict-overflow $(WARNINGS)

ASAN_FLAGS = -fsanitize=address,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

O_LEVEL = -O2
MARCH = -march=znver1
LIBRARY = -lm
#-lpng 
# -lglfw -lGL -lX11 -lpthread -lXrandr -lXi

DEBUG_FLAGS = $(FLAGS) $(ASAN_FLAGS) -Og $(MARCH) -DDEBUG -ggdb -D_FORTIFY_SOURCE=2
RELEASE_FLAGS = $(FLAGS) $(O_LEVEL) $(MARCH) -DNDEBUG -fomit-frame-pointer -g3 -flto

LINK_FLAGS_DEBUG = $(LIBRARY) $(ASAN_FLAGS) -g -DDEBUG -ggdb -D_FORTIFY_SOURCE=2
LINK_FLAGS_RELEASE = $(LIBRARY) -DNDEBUG -g3 -flto

# profiling:
# PROFILE = -fprofile-use
# VEC_FLAGS_GCC = -fopt-info-vec-optimized -fopt-info-vec-missed
# VEC_FLAGS_CLANG = -Rpass=loop-vectorize -Rpass-analysis=loop-vectorize -Rpass-missed=loop-vectorize -fsave-optimization-record
# -Rpass=.* -Rpass-analysis=.* -Rpass-missed=.* -fsave-optimization-record

# find . -type f -name "*.<file_ext>"
# find . -type f -name "*.<file_ext>" -printf "./build/%f\\n" | sort"""
