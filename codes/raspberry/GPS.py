#!/usr/local/bin/python
from pathlib import Path

def read_GPS():

	result = [];	

	#GPS connected by USB
	gps_file = Path("/dev/ttyACM0");
	
	#If GPS not connected
	if gps_file.exists() == False:
		result.append("ERROR");
		result.append("Lost satellite connection");
		return result;

	f = open("/dev/ttyACM0", "r");
	#Read USB input
	text = f.readline();
	text = text.split(',');
	while True:
		#If is the information that we need
		if text[0] == "$GPGGA":
			#If has coverage
			if text[2] !="" and text[4] != "":
				#Convert from NMEA to decimal coordinates
				lat = float(text[2][:2]);
				lat = lat + float(text[2][2:])/60;
				if text[3] == "S":
					lat = -lat;

				lon = float(text[4][:3]);
				lon = lon + float(text[4][3:])/60;
				if text[5] == "W":
					lon = -lon;			

				result.append("{0:.6f}".format(lat));
				result.append("{0:.6f}".format(lon));
				result.append(text[9]);
			else:
				result.append("ERROR");
				result.append("Lost satellite connection");	
			f.close();
			return result;
		text = f.readline();
		text = text.split(',');

