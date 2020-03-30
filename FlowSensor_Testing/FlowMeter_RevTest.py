#Brian Naranjo
#3/29/20

#Python Program for Polling YF-S201 Hall Effect Flow Meter Data 
#Prints out total Liters/Gallons Used and Instantaneous Flow Rate 


# Loops on Every Revolution and only outputs when flow is detected
# YF-S201 produces 6 pulses/ 1 revolution

#Input on GPIO 27-Pin 13, GND on Pin 6, 5V on Pin 4 

#Import Libraries
import RPi.GPIO as GPIO 	#Import GPIO Library 
import time, sys			#Import time, sys Library

#Set up GPIO
GPIO.setmode(GPIO.BCM)	#Set pin numbering to GPIO BOARD
sensorPin = 27					#Set Input Pin as GPIO 27
GPIO.setup(sensorPin,GPIO.IN)	#Set Sensor Pin as input

#Declare Variables 
pulses = 0					#Monitor count for sensor 0-5V pulses 
last_sensor_state = 0		#State of last sensor reading

rev_rate = 0  				#Revolution  Counts (Rev/Min)
total_revs = 0				#Total Revolutions
sys_time = 0.0 				#System Start Up time
start_time = 0.0			#Keep track of start time 
end_time = 0.0				#Keep track of end time
pulse_per_rev = 6			#Pulses per Rev


sensor_cal_constant  = 2.25 	#Water Meter Calibration Factor, 
								#	1 Pulse = 2.25 mL

print('Water Flow Sensor Impulse-Driven Testing:')
print('Press CTRL C to Exit')

sys_time = time.time()

while True:			#Continuous Loop 
	rev_rate = 0 	#reset revolution rate
	pulses = 0 		#reset pulses 
	start_time = time.time()	#Save Start time 
	
	while pulses <= 5:
		sensor_state = GPIO.input(sensorPin)

		if sensor_state != 0 and sensor_state != last_sensor_state: #Input Change
			pulses += 1
			
			
		last_sensor_state = sensor_state #Store Last State 
			
		try:
			#print(GPIO.input(sensorPin),end='') #Status Bit
			None
			
		except KeyboardInterrupt:
			print('\n CTRL - Exitting Flow Sensor Test')
				
			GPIO.cleanup()		#Clean up BPIO
			print('COMPLETE') 
			sys.exit()			#Exit Program
				
	rev_rate += 1					#Increment Revs/Time
	total_revs += 1					#Increment total Revs
	end_time = time.time()			#Save End time 
	
	
	#Print Resulting Data 
	#Flow rate and total Liters calibrated with constant
	print('------------------------------' )
	print('Flow Rate: ' ,
	round((rev_rate*pulse_per_rev*sensor_cal_constant)/(end_time - start_time),2)
	,'milliliters/ minute')
	
	# 1 Rev = 6 Pulses = 13.5mL
	print('Total Liters: ', round((total_revs*pulse_per_rev*sensor_cal_constant)/1000,2))
	print('Total Gallons: ', round(((total_revs*pulse_per_rev*sensor_cal_constant)/(3.79*1000)),2))
	print('Time Elapsed: ', round((end_time-start_time),2))
	print('System Time: ', time.asctime(time.localtime(time.time())))
	print('-----------------------------' )
	
	
	
	
