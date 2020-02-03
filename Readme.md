# ADS-B C Collector


This is an ADS-B (Automatic Dependent Surveillance - Broadcast) messages decoding system written in C programming language, called ADS-B Collector. This project is part of the [RadarLivre ](https://github.com/RadarLivre) project, developed by students and professors of the Federal University of Ceará, campus Quixadá.

## Structure of the system

The system performs four main activities, which are: the reading of a serial port for communication with the micro [ADS-B receptor](https://www.google.com/url?q=http://www.anteni.net/adsb/index.html%23!/microADSB-USB-receiver/p/15504142/category%3D3647494&sa=D&ust=1580750718822000&usg=AFQjCNEDi4bLTWrB6PAnjjdxSLeQZRJJ5A), which catches the ADS-B messages; the decoding of the received ADS-B messages, to extract position, velocity and identification information; the storing of this information in a local database; and the sending of this information to a remote server which belongs to the RadarLivre Project.

The system is composed of two threads: the first one performs the reading of the serial port, the decoding of the ADS-B messages and the storing of the information in the local database; the second thread sends a “hello” to the remote server every one minute and sends the information obtained from the ADS-B messages every 500 milliseconds. A fluxogram of these operations can be seen in the Figure 1.

![ADS-B C Collector](https://github.com/Marianna-Pinho/TCC02-Implementation/blob/master/Diagrama%20do%20Coletor%20ADS-B%20em%20C%20%28english%29.png)

## ADS-B Information

The system saves the following information about the aircrafts that send ADS-B messages:

**1.  Identification information** <br>
	a. ICAO <br>
	b. Callsign
	
**2.  Position information** <br>
	a. Latitude <br>
	b. Longitude <br>
	c. Altitude
	
**3. Velocity information** <br>
	a. Vertical velocity <br>
	b. Horizontal velocity<br>
	c. Heading

It is important to note that the Latitude and Longitude information is decoded based on Odd and Even position messages.

## Database

The system is using a SQlite database and is saving the information in a table called “radarlivre_api_adsbinfo”. The fields of this table can be seen below:
<pre><code> CREATE TABLE IF NOT EXISTS "radarlivre_api_adsbinfo"
(
"id" integer NOT NULL PRIMARY KEY AUTOINCREMENT,
"collectorKey" varchar(64) NULL,
"modeSCode" varchar(16) NULL,
"callsign" varchar(16) NULL,
"latitude" decimal NOT NULL,
"longitude" decimal NOT NULL,
"altitude" decimal NOT NULL,
"verticalVelocity" decimal NOT NULL,
"horizontalVelocity" decimal NOT NULL,
"groundTrackHeading" decimal NOT NULL,
"timestamp" bigint NOT NULL,
"timestampSent" bigint NOT NULL,
"messageDataId" varchar(100) NOT NULL,
"messageDataPositionEven" varchar(100) NOT NULL,
"messageDataPositionOdd" varchar(100) NOT NULL,
"messageDataVelocity" varchar(100) NOT NUL
); </code></pre>

“id” is a primary key created automatically by the database; “collectorKey” is the identification of the system in the remote server; “modeSCode” is the ICAO information; “timestamp” and “timestampSent” are the time when the message arrives to the system and is sent to the remote server, respectively; “messageDataId”, “messageDataPositionEven”, “messageDataPositionOdd” and “messageDataVelocity” are the messages received by the system, from where the information is extracted.

## Communication with the remote server

To communicate with the remote server, the system is using the [libcurl API](https://curl.haxx.se/libcurl/) and the information is being sent in a JSON format. To transform the information to a JSON format, the system is using the [json-c library](https://ubuntu.pkgs.org/18.04/ubuntu-main-i386/libjson-c-dev_0.12.1-1.3_i386.deb.html).

## Configuring, Building and Executing
**1.  Collector information**
The collector information is setted up in the file “adsb_info.h”, where is necessary to provide the collector key, the login and the password, which are registered in the remote server. The login and password should be written as follows:
<pre><code>#define USER_AUTH “login:password”</code></pre>

**2.  Server information**
The server information is setted up in the file “adsb_network.h”. It is necessary to configure the address to which the system must send the “hello” messages (PUT_URL) and that used to send the ADS-B information (POST_URL).

**3. Build information**
The build information, like .c and .h files location, build attributes and the compiler, is setted up in the Makefile.

To build and execute the system, the following steps can be executed:
#### 1. To clean the project
	make clean
#### 2. To compile the project
	make
#### 3. To execute the system
	sudo ./run_collector
