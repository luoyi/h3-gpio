#include <wiringPi.h>
#include <unistd.h>
#include <stdio.h>

int main(void)
{
  wiringPiSetup() ;
  int i = 0;
  int v = 0;
  for ( i =0; i < 100; i++ ) {
	  pinMode (22, OUTPUT) ;
	  pinMode (23, OUTPUT) ;
	  pinMode (24, OUTPUT) ;
	  pinMode (25, OUTPUT) ;
	  pinMode (29, INPUT) ;
	  digitalWrite(22, HIGH) ;
	  digitalWrite(23, HIGH) ;
	  digitalWrite(24, HIGH) ;
	  digitalWrite(25, HIGH) ;
	  v = digitalRead(22);
	  printf("digi read 22 = %d\n", v);
	  v = digitalRead(29);
	  printf("digi read 29 = %d\n", v);
  	  sleep(1);
  }
}
