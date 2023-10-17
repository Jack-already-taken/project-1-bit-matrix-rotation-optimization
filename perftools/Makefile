# Set this to 1 if you are compiling for your native machine
# as opposed to a Haswell compatible build for awsrun
LOCAL := 0

# Updated for HPC2, need to specify clang-16 which was installed on hpc2 and agate-1
CC := clang-16

ifeq ($(DEBUG),1)
	CFLAGS := -Wall -O0 -g -gdwarf-4
else
	CFLAGS := -Wall -O3 -ggdb -gdwarf-4 -DNDEBUG 
endif

# Optimize for AMD Zen2 7002-series 'Rome'
# From https://www.amd.com/content/dam/amd/en/documents/epyc-technical-docs/tuning-guides/amd-epyc-7002-tg-hpc-56827.pdf 
CFLAGS_HPC2 := -march=znver2
CFLAGS_LOCAL := -march=native

ifeq ($(LOCAL), 0)
	CFLAGS := $(CFLAGS_HPC2) $(CFLAGS)
else
	CFLAGS := $(CFLAGS_LOCAL) $(CFLAGS)
endif

ifeq ($(LOCAL), 1)
$(info BE ADVISED: You have selected to build for your native architecture. This might be incompatible with awsrun machines. Make sure to unset the LOCAL flag or use LOCAL=0 before running on awsrun.)
endif

LDFLAGS := -lrt

all: isort sum

isort.o: isort.c 
	$(CC) $(CFLAGS) -c isort.c 

qsort.o: qsort.c 
	$(CC) $(CFLAGS) -c qsort.c 

isort: isort.o qsort.o
	$(CC) -o isort isort.o qsort.o $(LDFLAGS)

sum.o: sum.c 
	$(CC) $(CFLAGS) -c sum.c 

sum: sum.o
	$(CC) -o sum sum.o $(LDFLAGS)

clean:
	rm -f isort sum *.o *.gcda *.gcno *.gcov perf.data */perf.data cachegrind.out.*
