CC=gcc#aarch64-linux-gnu-gcc
CFLAGS=-Ih -Iinclude -fopenmp
QEMU=#qemu-aarch64 -L /usr/aarch64-linux-gnu/

all: tests tests_simd benchs benchs_simd

tests_all: tests tests_simd

tests: test_mouvement test_morpho
	$(QEMU) bin/test_mouvement
	$(QEMU) bin/test_morpho

tests_simd: test_mouvement_SIMD test_morpho_SIMD
	$(QEMU) bin/test_mouvement_simd
	$(QEMU) bin/test_morpho_simd

benchs: bench_mouvement bench_morpho bench_chaine
	$(QEMU) bin/bench_mouvement
	$(QEMU) bin/bench_morpho
	$(QEMU) bin/bench_chaine

benchs_simd: bench_mouvement_SIMD bench_morpho_SIMD bench_chaine_SIMD
	$(QEMU) bin/bench_mouvement_simd
	$(QEMU) bin/bench_morpho_simd
	$(QEMU) bin/bench_chaine_simd


test_mouvement: dirs 
	$(CC) $(CFLAGS) src/nrutil.c c/test_mouvement.c -o bin/test_mouvement
test_morpho: dirs 
	$(CC) $(CFLAGS) src/nrutil.c c/test_morpho.c -o bin/test_morpho

test_mouvement_SIMD: dirs
	$(CC) $(CFLAGS) src/nrutil.c src/vnrutil.c c/valgo.c c/test_mouvement_SIMD.c -o bin/test_mouvement_simd
test_morpho_SIMD: dirs 
	$(CC) $(CFLAGS) src/nrutil.c src/vnrutil.c c/valgo.c c/test_morpho_SIMD.c -o bin/test_morpho_simd


bench_mouvement: dirs 
	$(CC) $(CFLAGS) src/nrutil.c c/mouvement.c c/bench_mouvement.c -o bin/bench_mouvement
bench_morpho: dirs 
	$(CC) $(CFLAGS) src/nrutil.c c/morpho.c c/bench_morpho.c -o bin/bench_morpho
bench_chaine: dirs
	$(CC) $(CFLAGS) src/nrutil.c c/morpho.c h/mouvement.h c/mouvement.c c/bench_chaine.c -o bin/bench_chaine

bench_mouvement_SIMD: dirs
	$(CC) $(CFLAGS) src/nrutil.c src/vnrutil.c c/valgo.c c/mouvement_SIMD.c c/bench_mouvement_SIMD.c -o bin/bench_mouvement_simd
bench_morpho_SIMD: dirs 
	$(CC) $(CFLAGS) src/nrutil.c src/vnrutil.c c/valgo.c c/morpho_SIMD.c c/bench_morpho_SIMD.c -o bin/bench_morpho_simd
bench_chaine_SIMD: dirs
	$(CC) $(CFLAGS) src/nrutil.c src/vnrutil.c c/valgo.c c/bench_chaine_SIMD.c c/mouvement_SIMD.c c/morpho_SIMD.c -o bin/bench_chaine_simd

dirs:
	mkdir -p bin
clean:
	rm -rf bin/*
