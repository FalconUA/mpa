TARGET = demo
CC = g++
CFLAGS =
HEADERS = defines.hpp mpz.hpp mpr.hpp mpmod.hpp mpa.hpp modules.hpp
OBJECTS = \
	mpz.o mpr.o mpmod.o mpmod_gpu.o algorithm.o inverse.o \
	mul_schoolgrade.o mul_karatsuba.o mul_toom3way.o mul_fft.o mul_fft_gpu.o mul_schonhage.o \
	pt_lemer.o pt_rabin_miller.o pt_lehmann.o pt_agrawal.o pt_solovey_strassen.o \

# custom main code
DEMO = main.o
MULCMP = mulcmp.o
PTCMP = ptcmp.o
	
demo : $(OBJECTS) $(DEMO)
	$(CC) $(CFLAGS) $(OBJECTS) $(DEMO) -o $(TARGET)

mulcmp : $(OBJECTS) $(MULCMP)
	$(CC) $(CFLAGS) $(OBJECTS) $(MULCMP) -o $(TARGET)

ptcmp : $(OBJECTS) $(PTCMP)
	$(CC) $(CFLAGS) $(OBJECTS) $(PTCMP) -o $(TARGET)

%.o : %.c
	$(CC) $(CFLAGS) $(HEADERS) -c $<

clean :
	rm -r *o $(TARGET)
