all : main.cpp bootstrap.S
	riscv32-unknown-elf-g++ main.cpp bootstrap.S -o main -lm -nostartfiles -march=rv32ima -mabi=ilp32
	
sim: all
	~/ee6470/riscv-vp/vp/build/bin/tiny32-mc --intercept-syscalls  main
	
clean:
	rm -f main
