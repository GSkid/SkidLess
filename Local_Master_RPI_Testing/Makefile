CXXFLAGS = -Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -std=c++0x
VPATH = RF24:RF24Network:RF24Mesh


TARGET = rf_oled_test_main

${TARGET}: Local_Master.o font8.o font12.o font16.o font20.o font24.o font8.o  dev_hardware_SPI.o OLED_GFX.o OLED_Driver.o sysfs_gpio.o DEV_Config.o bcm2835.o 
	g++ bin/Local_Master.o ./bin/font12.o ./bin/font16.o ./bin/font20.o ./bin/font8.o ./bin/font24.o  ./bin/dev_hardware_i2c.o ./bin/sysfs_gpio.o ./bin/OLED_GFX.o ./bin/OLED_Driver.o ./bin/bcm2835.o ./bin/DEV_Config.o ./bin/dev_hardware_SPI.o $(CXXFLAGS) -Wall -I./ -lrf24-bcm  -lrf24network -lrf24mesh -pthread -o rf_oled_test_main


font12.o: Fonts/font12.c
		gcc -D USE_BCM2835_LIB -c  Fonts/font12.c -o bin/font12.o

font16.o: Fonts/font16.c
		gcc -D USE_BCM2835_LIB -c  Fonts/font16.c -o bin/font16.o 
		
font20.o: Fonts/font20.c
		gcc -D USE_BCM2835_LIB -c  Fonts/font20.c -o bin/font20.o 

font8.o: Fonts/font8.c
		gcc -D USE_BCM2835_LIB -c  Fonts/font8.c -o bin/font8.o 

font24.o: Fonts/font24.c
		gcc -D USE_BCM2835_LIB -c  Fonts/font24.c -o bin/font24.o 

#ASCII_Font.o: Fonts/ASCII_Font.c 
#		gcc -D USE_BCM2835_LIB -c  Fonts/ASCII_Font.c -o bin/ASCII_Font.o
		
dev_hardware_i2c.o: obj/dev_hardware_i2c.c obj/dev_hardware_i2c.h
		gcc -D USE_BCM2835_LIB -c  obj/dev_hardware_i2c.c -o bin/dev_hardware_i2c.o -I ./Fonts #-lbcm2835 -lm
	
sysfs_gpio.o: obj/sysfs_gpio.c obj/sysfs_gpio.h
		gcc -D USE_BCM2835_LIB -c  obj/sysfs_gpio.c -o bin/sysfs_gpio.o -I ./Fonts #-lbcm2835 -lm 
	
OLED_GFX.o: obj/OLED_GFX.c obj/OLED_GFX.h obj/OLED_Driver.h
		gcc -D USE_BCM2835_LIB -c  obj/OLED_GFX.c -o bin/OLED_GFX.o -I ./Fonts #-lbcm2835 -lm 

OLED_Driver.o: obj/OLED_Driver.c obj/OLED_Driver.h obj/DEV_Config.h
		gcc -D USE_BCM2835_LIB -c  obj/OLED_Driver.c -o bin/OLED_Driver.o -I ./Fonts #-lbcm2835 -lm 

bcm2835.o: obj/bcm2835.c obj/bcm2835.h 
		gcc -D USE_BCM2835_LIB -c  obj/bcm2835.c -o bin/bcm2835.o -I ./Fonts #-lbcm2835 -lm
	
DEV_Config.o: obj/DEV_Config.c obj/DEV_Config.h 
		gcc -D USE_BCM2835_LIB -c  obj/DEV_Config.c -o bin/DEV_Config.o -I ./Fonts #-lbcm2835 -lm 

dev_hardware_SPI.o: obj/dev_hardware_SPI.c obj/dev_hardware_SPI.h 
		gcc -D USE_BCM2835_LIB -c  obj/dev_hardware_SPI.c -o bin/dev_hardware_SPI.o -I ./Fonts #-lbcm2835 -lm 

Local_Master.o: obj/Local_Master.cpp obj/OLED_Driver.h obj/DEV_Config.h obj/OLED_GFX.h font8.o font12.o font16.o font20.o font24.o font8.o  dev_hardware_SPI.o OLED_GFX.o OLED_Driver.o sysfs_gpio.o DEV_Config.o bcm2835.o
		gcc -D  USE_BCM2835_LIB -c  obj/Local_Master.cpp $(CXXFLAGS) -Wall -o bin/Local_Master.o -I ./Fonts ./obj ./bin -lrf24-bcm -lrf24network -lrf24mesh -pthread #-lbcm2835 -lm 
		#g++ -c Local_Master.cpp  $(CXXFLAGS) -Wall -o -I ./obj ./Fonts -I./ -lrf24-bcm -lbin -lobj -lrf24network -lrf24mesh -pthread	 		
		 
#gcc -D USE_BCM2835_LIB -c  obj/main.c -o bin/main.o -I ./Fonts #-lbcm2835  -lm 


#font12.o: Fonts/font12.c#gcc -D USE_BCM2835_LIB ./bin/font12.o ./bin/font16.o ./bin/font20.o ./bin/font8.o ./bin/font24.o ./bin/ASCII_Font.o ./bin/dev_hardware_i2c.o ./bin/sysfs_gpio.o ./bin/OLED_GFX.o ./bin/main.o ./bin/OLED_Driver.o ./bin/bcm2835.o ./bin/DEV_Config.o ./bin/dev_hardware_SPI.o -o rf_oled_test_main #./bin/bcm2835.o  #-lbcm2835 -lrf24-bcm -lrf24network -lrf24mesh -lm 

clean:
	$(RM) $(DIR_BIN)/*.* $(TARGET) $(DIR_OBJ)/.*.sw?
#	rm *.o
#	rm Local_Master
