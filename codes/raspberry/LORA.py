#!/usr/local/bin/python
import serial
import codecs

def init_Lora():
	global ser;
	ser = serial.Serial ("/dev/ttyS0", 57600);

	ser.write("mac pause\r\n");
	ser.readline();

	ser.write("radio set mod lora\r\n");
	ser.readline();

	ser.write("radio set freq 869100000\r\n");
	ser.readline();

	ser.write("radio set pwr 14\r\n");
	ser.readline();

	ser.write("radio set sf sf7\r\n");
	ser.readline();

	ser.write("radio set afcbw 41.7\r\n");
	ser.readline();

	ser.write("radio set rxbw 125\r\n");
	ser.readline();

	ser.write("radio set prlen 8\r\n");
	ser.readline();

	ser.write("radio set crc on\r\n");
	ser.readline();

	ser.write("radio set iqi off\r\n");
	ser.readline();

	ser.write("radio set cr 4/5\r\n");
	ser.readline();

	ser.write("radio set wdt 60000\r\n");
	ser.readline();

	ser.write("radio set sync 12\r\n");
	ser.readline();

	ser.write("radio set bw 125\r\n");
	ser.readline();



def read_Lora():
	result = [];	
	#Read USB input
	ser.write("radio rx 0\r\n");
	ser.readline();	
	text = ser.readline();
	text = text.split(' ');
	
	if text[0] == "radio_rx":
		return(codecs.decode(text[2][:-2], "hex"));
	else:
		return("ERROR");

def test_Lora():
	return "{t:24.60,hA:20.23,i:100,hS:20,p:1005.00}";


