kernel top dir: 
	arch/mips/def-rtl8196-gw-8m(or arch/mips/def-rtl8196-gw) is the default kernel config file
	copy it to top dir, and rename to .config
	then, make menuconfig && make dep && make clean && make
	
	notice: need to entry dir: rtkload, and make clean;make

AP:
	make gw

result is in 8196c dir!!!
