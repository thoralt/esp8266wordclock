//	ESP8266 Wordclock
//	Copyright (C) 2016 Thoralt Franz, https://github.com/thoralt
//
//		This module contains a simple NTP client. NTP packets are sent using UDP to a
//		configurable NTP server. The server reply is processed and a callback is
//		executed to notifiy the calling application of the current time. The internal
//		state machine is called from an internal timer to take care of timeouts, the
//		NTP request is retried automatically if no reply is being received. The request
//		is being repeated every 59 minutes, so the calling module is updated regularly.
//
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//	This code is based on (heavily modified):
//	https://github.com/sandeepmistry/esp8266-Arduino/blob/master/esp8266com/esp8266/libraries/ESP8266WiFi/examples/NTPClient
#include <Arduino.h>
#include "ntp.h"

//---------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------
#define NTP_PACKET_SIZE 48
#define LOCAL_PORT 2390
#define NTP_TIMEOUT 5000
#define TIMER_RESOLUTION 10
#define NTP_RELOAD_INTERVAL (59*60*1000)

//---------------------------------------------------------------------------------------
// global instance
//---------------------------------------------------------------------------------------
NtpClass NTP = NtpClass();

//---------------------------------------------------------------------------------------
// tickerFunctionWrapper
//
// Static wrapper which is used as callback function for Ticker and takes supplied
// argument to call the handler tickerFunction() of instance method
//
// -> obj: Instance of class to call the method tickerFunctio() on
// <- --
//---------------------------------------------------------------------------------------
void NtpClass::tickerFunctionWrapper(NtpClass *obj)
{
    obj->tickerFunction();
}

//---------------------------------------------------------------------------------------
// NtpClass
//
// Constructor, sets up a timer to trigger the internal state machine
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
NtpClass::NtpClass()
{
    this->ticker.attach_ms(TIMER_RESOLUTION, NtpClass::tickerFunctionWrapper, this);
}

//---------------------------------------------------------------------------------------
// setServer
//
// Sets the current time server and schedules a new NTP request
//
// -> address: IP address of the new time server
// <- --
//---------------------------------------------------------------------------------------
void NtpClass::setServer(IPAddress address)
{
	this->timeServer = address;
    this->state = NtpState::waitingForReload;
    this->timer = NTP_RELOAD_INTERVAL - 2000;
}

//---------------------------------------------------------------------------------------
// getServer
//
// Gets the current time server
//
// -> --
// <- IPAddress current time server
//---------------------------------------------------------------------------------------
IPAddress NtpClass::getServer()
{
	return this->timeServer;
}

//---------------------------------------------------------------------------------------
// begin
//
// Initializes the class and starts the first NTP request, automatically fires callback
// upon success, repeats every 59 minutes
//
// -> ip: Address of an NTP server
//    callback: Function to receive the current time (hours, minutes, seconds, ms)
//    timezone: Hours difference from UTC (will be added to the received time, can be
//              negative)
// <- --
//---------------------------------------------------------------------------------------
void NtpClass::begin(IPAddress ip, TNtpCallback callback, int timezone)
{
    this->_callback = callback;
    this->timeServer = ip;
    this->tz = timezone * 3600;
    
    this->udp.begin(LOCAL_PORT);
    
    // wait 2 seconds before starting first request
    Serial.println("NtpClass::begin() Waiting 2 seconds");
    this->state = NtpState::waitingForReload;
    this->timer = NTP_RELOAD_INTERVAL - 2000;
}

//---------------------------------------------------------------------------------------
// tickerFunction
//
// Function to be called by the timer, drives the internal state machine
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
void NtpClass::tickerFunction()
{
	// increment timer variable
    this->timer += TIMER_RESOLUTION;
    
    switch(this->state)
    {
        case NtpState::startRequest:
        	this->udp.flush(); // clear previously received data
            this->sendPacket(); // request new time data
            this->timer = 0;
            this->state = NtpState::waitingForReply;
            this->syncInProgress = true;
            break;
            
        case NtpState::waitingForReply:
            if(this->timer >= NTP_TIMEOUT)
            {
                Serial.println("NtpClass: NTP request timeout");
                this->state = NtpState::startRequest;
            }
            else if(udp.parsePacket() > 0)
            {
                Serial.println("NtpClass: Received NTP packet");
                this->parse();
                if(this->_callback) this->_callback(this->h, this->m, this->s, this->ms);
                this->timer = 0;
                this->state = NtpState::waitingForReload;
                this->syncInProgress = false;
            }
            break;
            
        case NtpState::waitingForReload:
            if(this->timer >= NTP_RELOAD_INTERVAL)
            {
                Serial.println("NtpClass: NTP reload timer expired.");
                this->state = NtpState::startRequest;
            }
            break;
            
        case NtpState::idle:
        default:
            break;
    }
}

//---------------------------------------------------------------------------------------
// parse
//
// Reads the received UDP packet and decodes the current time, stores result in (this)
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
void NtpClass::parse()
{
    byte buf[NTP_PACKET_SIZE];
    Serial.print("NtpClass::parse() (");
    Serial.print(this->timer);
    
    this->udp.read(buf, NTP_PACKET_SIZE);
    this->udp.flush(); // discard additional data
    unsigned long highWord = word(buf[40], buf[41]);
    unsigned long lowWord = word(buf[42], buf[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears + this->tz;
    randomSeed(epoch);
    
    this->h = (epoch  % 86400L) / 3600;
    this->m = (epoch  % 3600) / 60;
    this->s = (epoch % 60);
    this->ms = 0;
    
    Serial.print("ms), local time: ");
    Serial.println(STRING2(this->h) + ":" + STRING2(this->m) + ":" + STRING2(this->s));
}

//---------------------------------------------------------------------------------------
// sendNTPpacket
//
// Requests time from an NTP server
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
void NtpClass::sendPacket()
{
    uint8_t buf[NTP_PACKET_SIZE];

    Serial.println("NtpClass::sendPacket()");
    memset(buf, 0, NTP_PACKET_SIZE);
    buf[0] = 0b11100011;
    buf[1] = 0;
    buf[2] = 6;
    buf[3] = 0xEC;
    buf[12] = 49;
    buf[13] = 0x4E;
    buf[14] = 49;
    buf[15] = 52;
    this->udp.beginPacket(this->timeServer, 123);
    this->udp.write(buf, NTP_PACKET_SIZE);
    this->udp.endPacket();
    
    this->timer = 0;
}
