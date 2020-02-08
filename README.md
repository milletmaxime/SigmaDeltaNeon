# SigmaDelta + Morpho en SIMD Neon

## Cross compiling
aarch64-linux-gnu-gcc -Iinclude src/nrutil.c src/vnrutil.c algo.c -o algo


qemu-aarch64 -L /usr/aarch64-linux-gnu/ ./algo
