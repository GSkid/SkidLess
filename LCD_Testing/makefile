CXXFLAGS=  -Wall -g -I


LCD_test_main: LCD_test_main.o U8x8lib.o
	g++ -o LCD_test_main LCD_test_main.o cppsrc/U8x8lib.h  $(CXXFLAGS) 

LCD_test_main.o: LCD_test_main.c 
		g++ $(CXXFLAGS) -c LCD_test_main.c cppsrc/U8x8lib.h  

U8x8lib.o: cppsrc/U8x8lib.cpp cppsrc/U8x8lib.h 
		g++ $(CXXFLAGS) -c ./cppsrc/U8x8lib.cpp cppsrc/U8x8lib.h		
		
		
		
		
clean:
	rm *.o
	rm LCD_test_main
