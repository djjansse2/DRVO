kdir := /mnt/clfs/build/kernel/linux-4.19.94 #locatie van jouw kernel
cdir := ${PWD}   #locatie van de objecten
# en bouwomgeving. Mag ook ${PWD} 
# zijn, de naam van de huidige directory

obj-m:=arduino_driver.o                           #naam van de te maken object of objecten

all:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- -C $(kdir) M=$(cdir) modules
clean:
	make -C ${kdir} M=${cdir} clean