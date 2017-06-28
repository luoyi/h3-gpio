all :: gpio gpio-demo

gpio: gpio.c
	gcc gpio.c -lwiringPi -o gpio 

gpio-demo: gpio-demo.c gpio-h3.h gpio-h3.c
	gcc gpio-demo.c gpio-h3.c -o gpio-demo
