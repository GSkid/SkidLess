#Brian Naranjo
#3/29/20

#Python Program for Polling YF-S201 Hall Effect Flow Meter Data 
#Prints out total Liters/Gallons Used and Instantaneous Flow Rate 


# Loops on Specified Time Interval and only outputs when flow is detected
#Keeps track of how long it has been since last flow was detected 

#Input on GPIO 27-Pin 13, GND on Pin 6, 5V on Pin 4 

#Import Libraries
import RPi.GPIO as GPIO 	#Import GPIO Library 
import time, sys			#Import time, sys Library

#Set up GPIO
GPIO.setmode(GPIO.BCM)	#Set pin numbering to GPIO BOARD
sensorPin = 27					#Set Input Pin as GPIO 27
GPIO.setup(sensorPin,GPIO.IN)	#Set Sensor Pin as input

#Declare Variables 

last_sensor_state = 0		#State of last sensor reading
total_pulses = 0				#Total count for sensor 0-5V pulses 
last_pulse_count = 0			#Saved count for pulses 
pulse_rate = 0  				#(Pulse/Min)

sys_time = 0.0 				#System Start Up time
sample_time = 0.0			#Variable tracking sampling time to sample
end_time = 0.0				#Keep track of end time
minutes = 0 				#Total Minutes
time_last_flow = 0.0 		#Time of last flow
time_elapsed_flow = 0.0		#Time elapsed since last flow

sensor_cal_constant  = 2.25 	#Water Meter Calibration Factor, 
								#	1 Pulse = 2.25 mL

print('Water Flow Sensor Timer-Based Testing:')
print('Press CTRL C to Exit')

sys_time = time.time()

while True:			#Continuous Loop 
	pulse_rate = 0 	#reset pulse rate
	last_pulse_count = total_pulses
	
	for sec_mult in range(0,20):	#Loop Every minute 
		sample_time = time.time() + 3	#Reset every 3 seconds 
		
		while time.time() <= sample_time:
			sensor_state = GPIO.input(sensorPin)
			
			if sensor_state != last_sensor_state:	#if triggered, increment pulse
				pulse_rate += 1
				total_pulses += 1
				time_last_flow = time.time()		#Store as time of last flow

			else:
				pulse_rate = pulse_rate
				total_pulses = total_pulses
	
			try:
				None
				
			except KeyboardInterrupt:
				print('\n CTRL - Exitting Flow Sensor Test')
				
				GPIO.cleanup()		#Clean up BPIO
				print('COMPLETE') 
				sys.exit()			#Exit Program
				
			last_sensor_state = sensor_state
			
	minutes += 1
				
	
	if total_pulses != last_pulse_count: #Only print if Flow is detected
		
		#Print Resulting Data 
		#Flow rate and total Liters calibrated with constant
		print('------------------------------' )
		print('Flow Rate: ' ,
		round((pulse_rate*sensor_cal_constant)/1000,2)
		,'Liters/ minute')
	

		print('Total Liters: ', round((total_pulses*sensor_cal_constant)/1000,2))
		print('Total Gallons: ', round(((total_pulses*sensor_cal_constant)/(3.79*1000)),2))
		print('Last Flow Reading: ', round(time_elapsed_flow/60,1),' minutes ago')
		print('System Time: ', time.asctime(time.localtime(time.time())))
		print('-----------------------------' )
	
		time_elapsed_flow = 0.0 	#Initialize since Flow was Detected
	
	else:
		time_elapsed_flow += 1		#Increment Every Minute w/o Flow
		
		
	
	
	
