EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 2
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L pspice:INDUCTOR L1
U 1 1 5E2FF7E6
P 3500 1600
F 0 "L1" H 3500 1815 50  0000 C CNN
F 1 "673-PE-53820NL" H 3500 1724 50  0000 C CNN
F 2 "" H 3500 1600 50  0001 C CNN
F 3 "~" H 3500 1600 50  0001 C CNN
	1    3500 1600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5E300316
P 2550 1900
F 0 "#PWR?" H 2550 1650 50  0001 C CNN
F 1 "GND" H 2555 1727 50  0000 C CNN
F 2 "" H 2550 1900 50  0001 C CNN
F 3 "" H 2550 1900 50  0001 C CNN
	1    2550 1900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5E301800
P 2900 1850
F 0 "#PWR?" H 2900 1600 50  0001 C CNN
F 1 "GND" H 2905 1677 50  0000 C CNN
F 2 "" H 2900 1850 50  0001 C CNN
F 3 "" H 2900 1850 50  0001 C CNN
	1    2900 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 1600 2900 1600
Connection ~ 2900 1600
Wire Wire Line
	2900 1850 2900 1900
$Comp
L Device:R R2
U 1 1 5E302BDE
P 3550 950
F 0 "R2" V 3343 950 50  0000 C CNN
F 1 "3k" V 3434 950 50  0000 C CNN
F 2 "" V 3480 950 50  0001 C CNN
F 3 "~" H 3550 950 50  0001 C CNN
	1    3550 950 
	0    1    1    0   
$EndComp
Wire Wire Line
	2900 1600 3250 1600
Wire Wire Line
	2750 1500 3100 1500
Wire Wire Line
	3000 950  3100 950 
$Comp
L Regulator_Switching:LM2594N-5.0 U1
U 1 1 5E2FABAE
P 2350 1600
F 0 "U1" H 2350 1967 50  0000 C CNN
F 1 "LM2594N-5.0" H 2350 1876 50  0000 C CNN
F 2 "Package_DIP:DIP-8_W7.62mm" H 2550 1350 50  0001 L CIN
F 3 "http://www.ti.com/lit/ds/symlink/lm2594.pdf" H 2350 1700 50  0001 C CNN
	1    2350 1600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5E3019C6
P 1950 1650
F 0 "#PWR?" H 1950 1400 50  0001 C CNN
F 1 "GND" H 1955 1477 50  0000 C CNN
F 2 "" H 1950 1650 50  0001 C CNN
F 3 "" H 1950 1650 50  0001 C CNN
	1    1950 1650
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 5E316936
P 1600 1650
F 0 "C1" H 1715 1696 50  0000 L CNN
F 1 "68n" H 1715 1605 50  0000 L CNN
F 2 "" H 1638 1500 50  0001 C CNN
F 3 "~" H 1600 1650 50  0001 C CNN
	1    1600 1650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5E3179BD
P 1600 1800
F 0 "#PWR?" H 1600 1550 50  0001 C CNN
F 1 "GND" H 1605 1627 50  0000 C CNN
F 2 "" H 1600 1800 50  0001 C CNN
F 3 "" H 1600 1800 50  0001 C CNN
	1    1600 1800
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 1600 1950 1650
Wire Wire Line
	1600 1500 1950 1500
Connection ~ 1600 1500
$Comp
L Device:CP1 C2
U 1 1 5E323D13
P 3950 1750
F 0 "C2" H 4065 1796 50  0000 L CNN
F 1 "47u" H 4065 1705 50  0000 L CNN
F 2 "" H 3950 1750 50  0001 C CNN
F 3 "~" H 3950 1750 50  0001 C CNN
	1    3950 1750
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1 C3
U 1 1 5E325337
P 4400 1750
F 0 "C3" H 4515 1796 50  0000 L CNN
F 1 "47u" H 4515 1705 50  0000 L CNN
F 2 "" H 4400 1750 50  0001 C CNN
F 3 "~" H 4400 1750 50  0001 C CNN
	1    4400 1750
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1 C4
U 1 1 5E325C2C
P 4850 1750
F 0 "C4" H 4965 1796 50  0000 L CNN
F 1 "10u" H 4965 1705 50  0000 L CNN
F 2 "" H 4850 1750 50  0001 C CNN
F 3 "~" H 4850 1750 50  0001 C CNN
	1    4850 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	3750 1600 3950 1600
Connection ~ 3950 1600
Wire Wire Line
	3950 1600 4400 1600
Connection ~ 4400 1600
$Comp
L power:GND #PWR?
U 1 1 5E3288D0
P 3950 1900
F 0 "#PWR?" H 3950 1650 50  0001 C CNN
F 1 "GND" H 3955 1727 50  0000 C CNN
F 2 "" H 3950 1900 50  0001 C CNN
F 3 "" H 3950 1900 50  0001 C CNN
	1    3950 1900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5E328F96
P 4400 1900
F 0 "#PWR?" H 4400 1650 50  0001 C CNN
F 1 "GND" H 4405 1727 50  0000 C CNN
F 2 "" H 4400 1900 50  0001 C CNN
F 3 "" H 4400 1900 50  0001 C CNN
	1    4400 1900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5E329271
P 4850 1900
F 0 "#PWR?" H 4850 1650 50  0001 C CNN
F 1 "GND" H 4855 1727 50  0000 C CNN
F 2 "" H 4850 1900 50  0001 C CNN
F 3 "" H 4850 1900 50  0001 C CNN
	1    4850 1900
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 5E312419
P 2850 950
F 0 "R1" V 2643 950 50  0000 C CNN
F 1 "1k" V 2734 950 50  0000 C CNN
F 2 "" V 2780 950 50  0001 C CNN
F 3 "~" H 2850 950 50  0001 C CNN
	1    2850 950 
	0    1    1    0   
$EndComp
Wire Wire Line
	3100 950  3100 1500
$Comp
L power:GND #PWR?
U 1 1 5E32C9DD
P 2650 1000
F 0 "#PWR?" H 2650 750 50  0001 C CNN
F 1 "GND" H 2655 827 50  0000 C CNN
F 2 "" H 2650 1000 50  0001 C CNN
F 3 "" H 2650 1000 50  0001 C CNN
	1    2650 1000
	1    0    0    -1  
$EndComp
Wire Wire Line
	2650 1000 2650 950 
Wire Wire Line
	2650 950  2700 950 
Connection ~ 4850 1600
Wire Wire Line
	1050 1500 1600 1500
Wire Wire Line
	1050 1650 1050 1600
$Comp
L power:GND #PWR?
U 1 1 5E323486
P 1050 1650
F 0 "#PWR?" H 1050 1400 50  0001 C CNN
F 1 "GND" H 1055 1477 50  0000 C CNN
F 2 "" H 1050 1650 50  0001 C CNN
F 3 "" H 1050 1650 50  0001 C CNN
	1    1050 1650
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J1
U 1 1 5E31D2B9
P 850 1500
F 0 "J1" H 860 1593 50  0000 C CNN
F 1 "Header_01x02_Female" H 742 1594 50  0001 C CNN
F 2 "" H 850 1500 50  0001 C CNN
F 3 "~" H 850 1500 50  0001 C CNN
	1    850  1500
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3950 950  3950 1600
Wire Wire Line
	3700 950  3950 950 
Wire Wire Line
	3100 950  3400 950 
Connection ~ 3100 950 
$Comp
L Diode:1N5819 D1
U 1 1 5E2FCB24
P 2900 1750
F 0 "D1" V 2854 1829 50  0000 L CNN
F 1 "1N5819" V 2945 1829 50  0000 L CNN
F 2 "Diode_THT:D_DO-41_SOD81_P10.16mm_Horizontal" H 2900 1575 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88525/1n5817.pdf" H 2900 1750 50  0001 C CNN
	1    2900 1750
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 5E3402C8
P 4850 1450
F 0 "#PWR?" H 4850 1300 50  0001 C CNN
F 1 "+5V" H 4865 1623 50  0000 C CNN
F 2 "" H 4850 1450 50  0001 C CNN
F 3 "" H 4850 1450 50  0001 C CNN
	1    4850 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 1450 4850 1600
Wire Wire Line
	4400 1600 4850 1600
Wire Wire Line
	4850 1600 5400 1600
Wire Wire Line
	5400 1750 5400 1700
$Comp
L power:GND #PWR?
U 1 1 5E332708
P 5400 1750
F 0 "#PWR?" H 5400 1500 50  0001 C CNN
F 1 "GND" H 5405 1577 50  0000 C CNN
F 2 "" H 5400 1750 50  0001 C CNN
F 3 "" H 5400 1750 50  0001 C CNN
	1    5400 1750
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Male J2
U 1 1 5E32F9DD
P 5600 1600
F 0 "J2" H 5650 1686 50  0000 R CNN
F 1 "Conn_01x02_Male" H 5572 1483 50  0001 R CNN
F 2 "" H 5600 1600 50  0001 C CNN
F 3 "~" H 5600 1600 50  0001 C CNN
	1    5600 1600
	-1   0    0    -1  
$EndComp
$Comp
L power:+9V #PWR?
U 1 1 5E33E9B3
P 1600 1350
F 0 "#PWR?" H 1600 1200 50  0001 C CNN
F 1 "+9V" H 1615 1523 50  0000 C CNN
F 2 "" H 1600 1350 50  0001 C CNN
F 3 "" H 1600 1350 50  0001 C CNN
	1    1600 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	1600 1350 1600 1500
$Sheet
S 650  650  5100 1650
U 5E37822E
F0 "Power Supply" 50
F1 "PowerSupply.sch" 50
$EndSheet
$Comp
L Device:Q_PMOS_DGS Q1
U 1 1 5E378CA2
P 1950 3300
F 0 "Q1" V 2292 3300 50  0000 C CNN
F 1 "Q_PMOS_DGS" V 2201 3300 50  0000 C CNN
F 2 "" H 2150 3400 50  0001 C CNN
F 3 "~" H 1950 3300 50  0001 C CNN
	1    1950 3300
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Conn_01x07_Female J3
U 1 1 5E383272
P 750 3700
F 0 "J3" H 700 4108 50  0000 C CNN
F 1 "Conn_01x07_Female" H 642 4094 50  0001 C CNN
F 2 "" H 750 3700 50  0001 C CNN
F 3 "~" H 750 3700 50  0001 C CNN
	1    750  3700
	-1   0    0    -1  
$EndComp
Wire Wire Line
	950  3500 1950 3500
Wire Wire Line
	1150 3200 1150 3400
Wire Wire Line
	1150 3400 950  3400
$Comp
L power:+5V #PWR?
U 1 1 5E38FCC2
P 1150 3200
F 0 "#PWR?" H 1150 3050 50  0001 C CNN
F 1 "+5V" H 1165 3373 50  0000 C CNN
F 2 "" H 1150 3200 50  0001 C CNN
F 3 "" H 1150 3200 50  0001 C CNN
	1    1150 3200
	1    0    0    -1  
$EndComp
Connection ~ 1150 3400
Wire Wire Line
	1750 3400 1150 3400
Wire Wire Line
	1750 3200 1750 3400
$Comp
L power:GND #PWR?
U 1 1 5E3940D1
P 1150 4100
F 0 "#PWR?" H 1150 3850 50  0001 C CNN
F 1 "GND" H 1155 3927 50  0000 C CNN
F 2 "" H 1150 4100 50  0001 C CNN
F 3 "" H 1150 4100 50  0001 C CNN
	1    1150 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	950  4000 1150 4000
Wire Wire Line
	1150 4000 1150 4100
$Comp
L Device:R_PHOTO R3
U 1 1 5E39CEAF
P 2500 3350
F 0 "R3" H 2570 3396 50  0000 L CNN
F 1 "R_PHOTO" H 2570 3305 50  0000 L CNN
F 2 "" V 2550 3100 50  0001 L CNN
F 3 "~" H 2500 3300 50  0001 C CNN
	1    2500 3350
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Variable R5
U 1 1 5E3A12C9
P 3200 3350
F 0 "R5" H 3328 3396 50  0000 L CNN
F 1 "R_Moisture" H 3328 3305 50  0000 L CNN
F 2 "" V 3130 3350 50  0001 C CNN
F 3 "~" H 3200 3350 50  0001 C CNN
	1    3200 3350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 5E3A2A61
P 2500 3650
F 0 "R4" H 2570 3696 50  0000 L CNN
F 1 "2.2k" H 2570 3605 50  0000 L CNN
F 2 "" V 2430 3650 50  0001 C CNN
F 3 "~" H 2500 3650 50  0001 C CNN
	1    2500 3650
	1    0    0    -1  
$EndComp
$Comp
L Device:R R6
U 1 1 5E3A3E9C
P 3200 3650
F 0 "R6" H 3270 3696 50  0000 L CNN
F 1 "10k" H 3270 3605 50  0000 L CNN
F 2 "" V 3130 3650 50  0001 C CNN
F 3 "~" H 3200 3650 50  0001 C CNN
	1    3200 3650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5E3A47E4
P 2500 3800
F 0 "#PWR?" H 2500 3550 50  0001 C CNN
F 1 "GND" H 2505 3627 50  0000 C CNN
F 2 "" H 2500 3800 50  0001 C CNN
F 3 "" H 2500 3800 50  0001 C CNN
	1    2500 3800
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5E3A51A5
P 3200 3800
F 0 "#PWR?" H 3200 3550 50  0001 C CNN
F 1 "GND" H 3205 3627 50  0000 C CNN
F 2 "" H 3200 3800 50  0001 C CNN
F 3 "" H 3200 3800 50  0001 C CNN
	1    3200 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	950  3600 2050 3600
Wire Wire Line
	2050 3600 2050 3500
Wire Wire Line
	2050 3500 2500 3500
Connection ~ 2500 3500
Wire Wire Line
	3000 4100 3000 3500
Wire Wire Line
	3000 3500 3200 3500
Connection ~ 3200 3500
Wire Wire Line
	2300 3700 2300 4100
Wire Wire Line
	2300 4100 3000 4100
$Comp
L Sensor_Pressure:BMP180 U2
U 1 1 5E3AEAD3
P 4750 3100
F 0 "U2" H 5028 2876 50  0000 L CNN
F 1 "BMP180" H 5028 2785 50  0000 L CNN
F 2 "" H 4750 3200 50  0001 C CNN
F 3 "" H 4750 3200 50  0001 C CNN
	1    4750 3100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5E3B0962
P 4450 3600
F 0 "#PWR?" H 4450 3350 50  0001 C CNN
F 1 "GND" H 4455 3427 50  0000 C CNN
F 2 "" H 4450 3600 50  0001 C CNN
F 3 "" H 4450 3600 50  0001 C CNN
	1    4450 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 3600 4450 3500
Wire Wire Line
	4450 3500 4500 3500
Wire Wire Line
	2150 3200 2500 3200
Connection ~ 2500 3200
Wire Wire Line
	2500 3200 3200 3200
Wire Wire Line
	2200 3800 2200 4200
Wire Wire Line
	4350 4200 4350 3400
Wire Wire Line
	4350 3400 4500 3400
Wire Wire Line
	950  3800 2200 3800
Wire Wire Line
	950  3900 2100 3900
Wire Wire Line
	2100 3900 2100 4300
Wire Wire Line
	4250 4300 4250 3300
Wire Wire Line
	4250 3300 4500 3300
Wire Wire Line
	950  3700 2300 3700
$Comp
L power:VDD #PWR?
U 1 1 5E3BB86B
P 4450 3100
F 0 "#PWR?" H 4450 2950 50  0001 C CNN
F 1 "VDD" H 4467 3273 50  0000 C CNN
F 2 "" H 4450 3100 50  0001 C CNN
F 3 "" H 4450 3100 50  0001 C CNN
	1    4450 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 3100 4450 3200
Connection ~ 4450 3200
Wire Wire Line
	4450 3200 4500 3200
Text Label 1150 3600 0    50   ~ 0
Analog_Light
Text Label 1150 3700 0    50   ~ 0
Analog_Moisture
Text Label 1150 3800 0    50   ~ 0
SDA
Text Label 1150 3900 0    50   ~ 0
SCL
$Comp
L Device:C C5
U 1 1 5E3C229F
P 3900 3350
F 0 "C5" H 4015 3396 50  0000 L CNN
F 1 "100n" H 4015 3305 50  0000 L CNN
F 2 "" H 3938 3200 50  0001 C CNN
F 3 "~" H 3900 3350 50  0001 C CNN
	1    3900 3350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5E3C3859
P 3900 3500
F 0 "#PWR?" H 3900 3250 50  0001 C CNN
F 1 "GND" H 3905 3327 50  0000 C CNN
F 2 "" H 3900 3500 50  0001 C CNN
F 3 "" H 3900 3500 50  0001 C CNN
	1    3900 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3200 3200 3900 3200
Connection ~ 3200 3200
Wire Wire Line
	3900 3200 4450 3200
Connection ~ 3900 3200
Wire Wire Line
	2200 4200 4350 4200
Wire Wire Line
	2100 4300 4250 4300
$Sheet
S 650  2800 5100 1800
U 5E3DB4A8
F0 "Sensor Array" 50
F1 "SensorArray.sch" 50
$EndSheet
Text Label 1150 3500 0    50   ~ 0
Enable
Text Notes 7350 6950 0    200  ~ 0
Intuitive Auto Irrigation\n
Text Notes 7400 7550 0    100  ~ 0
Sensor Node\n
Text Notes 10600 7650 0    50   ~ 0
v1.0\n
Text Notes 8150 7650 0    50   ~ 0
1/27/2020\n
$EndSCHEMATC