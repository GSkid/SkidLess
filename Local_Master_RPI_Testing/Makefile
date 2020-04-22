prefix := usr/local

CXXFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -std=c++0x
VPATH=RF24:RF24Network:RF24Mesh

Local_Master: Local_Master.o 
	g++ Local_Master.o $(CXXFLAGS) -Wall -I./ -lrf24-bcm  -lrf24network -lrf24mesh -loled -pthread -o Local_Master 

Local_Master.o: Local_Master.cpp  
	g++ -c Local_Master.cpp $(CXXFLAGS) -Wall -I./ -lrf24-bcm -lrf24network -loled -lrf24mesh -pthread	
		
			
		
clean:
	rm *.o
	rm Local_Master
