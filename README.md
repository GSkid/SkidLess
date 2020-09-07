# SkidLess
This is the repository for the intuitive auto-irrigation system. Here you can find code for the central hub, 
both if you're using an arduino or a raspberry pi, code for sensor nodes, and other relevant information. 
Testing files are found in the Testing Results folder. For questions or more information, 
please email Grant Skidmore at grant.skidmore@gmail.com

The central directory is Local_Master_RPI. This is the code for the central hub and contains all the relevant librarires (RF & OLED). This directory also includes files like the python file for the forecast API, the .csv file to store the data, the .txt file that stores the dhcp list to preserve the network addresses even after power cycling. For a hard system reset, be sure to clear out this file so that there are no default connected nodes. The file Local_Master is the actual exectuable that contains the most recent version of the central hub program.

The code used for the sensor nodes is found in the Multi_Node directory. Multi_Node and Multi_Node2 are near-identical copies of each other and only differ by the nodeID assigned to the file. These folders include only a single Arduino file to run all of the sensor node functions. This directory assumes all the necessary libraries are installed on your device (RF24, RF24_Network, RF24_Mesh) which can be installed via the libraries manager in the Arduino IDE. When burning the eeprom on the arduino microcontrollers, individual devices need to have distinct nodeID's. This can be done by changing the #define for the nodeID; keep in mind that available numbers range from 1-254, where 0 is reserved for the master and the system can only handle 255 unique nodes.
