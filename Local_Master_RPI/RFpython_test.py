import sys

sys.path.append('/home/pi/.local/lib/python2.7/site-packages')

import forecastio

# Location: Santa Cruz (lat = 36.9741, lng = -122.0308)
# Example 2: Egypt (lat = 26.8206, lng = 30.8025)
api_key = "2ef3d37cae4747a0cdc3c75cb4c5b3ad"
# Santa Cruz
lat = 36.9655
lng = -121.9937

# Egypt
#lat = 26.8206
#lng = 30.8025

# this returns the load forecast object with the given parametersL key, latitiude, and longitude
forecast = forecastio.load_forecast(api_key, lat, lng)

# provides a vague hourly forecast
byHour = forecast.hourly()
daily = forecast.daily()
currentForecast = forecast.currently()
#print ("Daily Forecast: %s" % daily.summary)
#print ("Hourly Forecast: %s" % byHour.summary)
#print ("Current Forecast: %s" % currentForecast.summary)
#print (byHour.icon)

# this returns a map object, which contains all the weather data
result = forecast.json
curr = result.get('currently')

# here we print out the data for testing
#print (list(result))
#print ("\n")
#print(result)
#prints our current data map
#print ("Current: %s" % result.get('currently'))
#print ("\n")
# converts seconds to clock time
seconds = curr.get('time')

seconds = seconds % (24 * 3600) 
hour = seconds // 3600
hour = hour
seconds %= 3600
minutes = seconds // 60
seconds %= 60

# convert humidity to percent
humidity = curr.get('humidity') * 100

# convert precipProb to percent
precip = curr.get('precipProbability') * 100

# wind direction conversion
#windDirection = curr.get('windBearing') / 22.5
#windDir = 'F'

#if windDirection == 1:
#    windDir = 'N'



# outputs
#print ("Current time: %d:%02d:%02d" % (hour, minutes, seconds))
#print ("Current Forecast Summary: %s" % curr.get('summary'))
#print ("Rain Probability (percent): %s" % curr.get('precipProbability'))
#print ("Temperature (F): %s" % curr.get('temperature'))
#print ("Humidity (percent): %s" % humidity)
#print ("Pressure (mb): %s" % curr.get('pressure'))
#print ("Wind Speed (km/hr): %s" % curr.get('windSpeed'))
#print ("Wind Direction (degrees): %s" % curr.get('windBearing'))
#print ("Wind Direction: %s" % windDir)

print (precip)
print (curr.get('temperature'))
print (humidity)
print (curr.get('pressure'))
print (curr.get('windSpeed'))
print (curr.get('windBearing'))
