obj-m += top.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	sudo insmod top.ko
	gcc -g3 -O0 --std=c99 -fsanitize=address,undefined,leak ./client.c -o client

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
