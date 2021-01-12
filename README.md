# SkidLess
This is the repository for the intuitive auto-irrigation system. Here you can find code for the central hub, 
code for sensor nodes, and other relevant information. 
Testing files are found in the Testing Results folder. For questions or more information, 
please email Grant Skidmore at grant.skidmore@gmail.com

## How to Navigate the Repo
./Local_Master_RPI
	../bin
	../RF24
	../RF24Network
	../RF24Mesh
	../RPi
	../Local_Master.h
	../Local_Master_Defines.h
	../Local_Master.cpp
	../Makefile
	../Local_Master
	../RFpython_test.py
	../Data_Log.csv
./Local_Master_RPI_Testing
./Multi_Node
	../Multi_Node.ino
./Testing Results
	../LightLevelTests
	../PrototypeTesting
	../SoilMoistureTests_V2
	../RangeTesting_29Feb2020
	../RangeTesting_4March2020.txt
	../RangeTesting_5Feb2020.txt
./README.md

The most important directories here are Local_Master_RPI, which is where the main .cpp file is located as well as the binary executable, Local_Master_RPI_Testing, where you can find older versions of Local_Master_RPI used for testing, Multi_node, which contains the .ino file for the sensor nodes, Testing Results, which has a bunch of text files with results from testing, and the readme file. There are other, less notable directories in the repository but they don't contain any useful files or information.

## What is Intuitive Auto Irrigation
Introducing Intuitive Auto-Irrigation (IAI), an intelligent way to go about everyday irrigation benefitting both water reserves and plant health. The system takes advantage of real-time sensor data and forecast predictions to trigger water delivery only when necessary, actively reducing the quantity of wastewater produced compared to traditional timer-based automatic irrigation systems. IAI implements a network of sensor nodes to get the most accurate data possible, even for those on a budget.

The project is separated into two distinct systems: the central hub and the sensor nodes. The central hub is what controls the entire system, including the control loop, the wireless network, and the water delivery. The sensor nodes, on the other hand, are responsible for gathering environmental data and reporting back to the central hub while managing a tight power budget. In this repository, you can find the code for the central hub in the Local_Master_RPI directory with an executable file (Local_Master) and a C++ project (Local_Master.h, Local_Master_Defines.h, Local_Master.cpp, and Makefile) as well as a other imporant files and libraries. One thing to note is that you will need a Raspberry Pi with a BCM2835 GPIO board in order for the executable to work or for the C++ file to compile. The code for the sensor nodes is located in the Multi_Node directory where you will only find the Multi_Node.ino file. Another thing to note here is that this will only compile if you have the necessary libraries installed via the Arduino IDE; specifically you will need the RF24, RF24Network, RF24Mesh, Adafruit_BMP085, and AVR_Standard_C_Time_Libarary libraries. All of these can be found in the library manager in the Arduino IDE.

In terms of hardware, the central hub runs on a Raspberry Pi Zero W, although any Raspberry Pi with a BCM2835 GPIO board should work. As for the sensor nodes, they operate using Arduino Nanos for size and cost efficiency, but Arduino Unos will also work. The radio transceivers used are Zigbee radio substitutes called nRF24L01 radio transceivers. They operate on the 2.4GHz band and can be purchased from Amazon for about $1.25/unit. There is also a lot of other hardware used, like custom sensor boards and plubming connectors; for a full list of hardware, please refer to the full report which is available at https://gskid.github.io .

## How the System Works
The system works to deliver water to the connected network of plants. This is done by delivering water which is controlled via latching solenoid valves (LSV) on the central hub. These LSVs open and close with small electrical signals to allow or prevent water from flowing through hoses to the plants. The central hub determines when to water based on a custom algorithm developed to integrate the sensor's environmental data with user configuration details and forecast data. The algorithm checks for a majority of sensor nodes reporting they need water for each hose network and if there is no predicted rain (<30% chance), it will turn on the water delivery. If there is rain predicted, the system will wait no more than 36 for the water level in the soil to change before the forecast prediction is manually overridden and water delivery is triggerred regardless. Then the system will turn off the water delivery as soon as the soil moisture level drops down below a preset threshold or other environmental conditions are or are not met. This control loop then continues to operate 24/7, turning on and off the water as efficently as possible.

In order for the sensor nodes to communicate with the central hub, they utilitze wireless radio transceivers to establish a wireless LAN. The transceivers communicate with their respective microcontrollers via SPI, which is handled by the RF24 libraries, but the specific communication protocol was designed by the team. The protocol uses a variation of the ping-pong protocol in order to preserve as much battery life on the sensor nodes as possible. Essentially, the sensor nodes will wake up asynchronously and take measurements of their environmental sensors. Then they will run a short algorithm to determine if that specific sensor needs water or not. The sensor then transmits as much information as possible to the central hub without fragmenting the message and waits for confirmation. Once the central hub receives the information, it checks for any configuration updates for that sensor node (eg. change in thresholds), and transmits either a standard confirmation message or a reconfiguration message back to the sensor node. Once the sensor node receives the reply, it will process any information from the message and puts itself to sleep again for 64 seconds.

## What We Learned from the Project
This project required us to learn a lot of new skills in a relatively short amount of time, while also trying to balance taking at least 2 other classes and other extra-curricular priorities. So time management was the first thing we had to learn, espcially working and communicating as a team. The next we learned were all the different technology skills. Specifically, this refers to skills like C++, SPI, Makefiles, embedded linux, data logging, using open source APIs, proper PCB design and component selection, building a user interface, low-power switching regulators, and many other extremely relevant skills. We also quickly learned the importance of good documentation and throughout the project, ensuring that everything was properly documented in either our Github repository or our Google Teams drive.

## Future Development
If this project were to continue in the future, there are a couple things we would look to implement. The first would be adding solar panels on the sensor nodes to add a way to drip charge the power supplies. This would significantly reduce the dependancy on batteries as reduce rechage time. The next would be to add extra features to the user interface like a dedicated timer mode and further customization options, maybe even develop an app for the UI in addition to the LCD. And finally, we would look to switch out the sensor node microcontrollers for more dedicated boards. Although this would be more expensive, boards like the Particle Argon have several built-in features that allow them to function much more reliably than knock-off nanos as well as incorporating nRF24L01 radios into the SBC. Over all, the project was very successful and we had a lot of fun building it 10/10, would recommend.