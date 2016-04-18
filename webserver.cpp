// ESP8266 Wordclock
// Copyright (C) 2016 Thoralt Franz, https://github.com/thoralt
//
//  This module encapsulates a small webserver. It replies to requests on port 80
//  and triggers actions, manipulates configuration attributes or serves files
//  from the internal flash file system.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
#include <Arduino.h>
#include <FS.h>
#include <ArduinoJson.h>

#include "webserver.h"
#include "ntp.h"

//---------------------------------------------------------------------------------------
// global instance
//---------------------------------------------------------------------------------------
WebServerClass WebServer = WebServerClass();

//---------------------------------------------------------------------------------------
// WebServerClass
//
// Constructor, currently empty
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
WebServerClass::WebServerClass()
{
}

//---------------------------------------------------------------------------------------
// ~WebServerClass
//
// Destructor, removes allocated web server object
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
WebServerClass::~WebServerClass()
{
	if (this->server)
		delete this->server;
}

//---------------------------------------------------------------------------------------
// begin
//
// Sets up internal handlers and starts the server at port 80
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
void WebServerClass::begin()
{
	SPIFFS.begin();

	this->server = new ESP8266WebServer(80);

	this->server->on("/setcolor", std::bind(&WebServerClass::handleSetColor, this));
	this->server->on("/info", std::bind(&WebServerClass::handleInfo, this));
	this->server->on("/saveconfig", std::bind(&WebServerClass::handleSaveConfig, this));
	this->server->on("/loadconfig", std::bind(&WebServerClass::handleLoadConfig, this));
	this->server->on("/matrix", std::bind(&WebServerClass::handleMatrix, this));
	this->server->on("/heart", std::bind(&WebServerClass::handleHeart, this));
	this->server->on("/stars", std::bind(&WebServerClass::handleStars, this));
	this->server->on("/heartbeat", std::bind(&WebServerClass::handleHeartbeat, this));
	this->server->on("/hourglass", std::bind(&WebServerClass::handleHourglass, this));
	this->server->on("/getcolors", std::bind(&WebServerClass::handleGetColors, this));
	this->server->on("/getntpserver", std::bind(&WebServerClass::handleGetNtpServer, this));
	this->server->on("/setntpserver", std::bind(&WebServerClass::handleSetNtpServer, this));
	this->server->onNotFound(std::bind(&WebServerClass::handleNotFound, this));
	this->server->begin();
}

//---------------------------------------------------------------------------------------
// process
//
// Must be called repeatedly from main loop
//
// ->
// <- --
//---------------------------------------------------------------------------------------
void WebServerClass::process()
{
	this->server->handleClient();
}

//---------------------------------------------------------------------------------------
// serveFile
//
// Looks up a given file name in internal flash file system, streams the file if found
//
// -> path: name of the file; "index.html" will be added if name ends with "/"
// <- true: file was found and served to client
//    false: file not found
//---------------------------------------------------------------------------------------
bool WebServerClass::serveFile(String path)
{
	Serial.println("serveFile: " + path);
	if (path.endsWith("/"))
		path += "index.html";
	if (SPIFFS.exists(path))
	{
		File file = SPIFFS.open(path, "r");
		this->server->streamFile(file, this->contentType(path));
		file.close();
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------------------
// contentType
//
// Returns an HTML content type based on a given file name extension
//
// -> filename: name of the file
// <- HTML content type matching file extension
//---------------------------------------------------------------------------------------
String WebServerClass::contentType(String filename)
{
	if (this->server->hasArg("download"))
		return "application/octet-stream";
	else if (filename.endsWith(".htm"))
		return "text/html";
	else if (filename.endsWith(".html"))
		return "text/html";
	else if (filename.endsWith(".css"))
		return "text/css";
	else if (filename.endsWith(".js"))
		return "application/javascript";
	else if (filename.endsWith(".png"))
		return "image/png";
	else if (filename.endsWith(".gif"))
		return "image/gif";
	else if (filename.endsWith(".jpg"))
		return "image/jpeg";
	else if (filename.endsWith(".ico"))
		return "image/x-icon";
	else if (filename.endsWith(".xml"))
		return "text/xml";
	else if (filename.endsWith(".pdf"))
		return "application/x-pdf";
	else if (filename.endsWith(".zip"))
		return "application/x-zip";
	else if (filename.endsWith(".gz"))
		return "application/x-gzip";
	return "text/plain";
}

//---------------------------------------------------------------------------------------
// handleNotFound
//
// Handles all requests not bound to other handlers, tries to serve a file if found in
// flash, responds with 404 otherwise
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
void WebServerClass::handleNotFound()
{
	// first, try to serve the requested file from flash
	if (!serveFile(this->server->uri()))
	{
		// create 404 message if no file was found for this URI
		String message = "File Not Found\n\n";
		message += "URI: ";
		message += this->server->uri();
		message += "\nMethod: ";
		message += (this->server->method() == HTTP_GET) ? "GET" : "POST";
		message += "\nArguments: ";
		message += this->server->args();
		message += "\n";
		for (uint8_t i = 0; i < this->server->args(); i++)
		{
			message += " " + this->server->argName(i) + ": "
					+ this->server->arg(i) + "\n";
		}
		this->server->send(404, "text/plain", message);
	}
}

//---------------------------------------------------------------------------------------
// handleGetNtpServer
//
// Delivers the currently configured NTP server IP address
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
void WebServerClass::handleGetNtpServer()
{
	this->server->send(200, "application/json", Config.ntpserver.toString());
}

//---------------------------------------------------------------------------------------
// handleSetNtpServer
//
// Sets a new IP address for the NTP client
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
void WebServerClass::handleSetNtpServer()
{
	if (this->server->hasArg("ip"))
	{
		IPAddress ip;
		if (ip.fromString(this->server->arg("ip")))
		{
			// set IP address in config
			Config.ntpserver = ip;
			Config.save();

			// set IP address in client
			NTP.setServer(ip);
		}
	}
	this->server->send(200, "application/json", "OK");
}

//---------------------------------------------------------------------------------------
// handleInfo
//
// Handles requests to "/info", replies with JSON structure containing system status
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
void WebServerClass::handleInfo()
{
	StaticJsonBuffer<512> jsonBuffer;
	char buf[512];
	JsonObject& json = jsonBuffer.createObject();
	json["heap"] = ESP.getFreeHeap();
	json["sketchsize"] = ESP.getSketchSize();
	json["sketchspace"] = ESP.getFreeSketchSpace();
	json["cpufrequency"] = ESP.getCpuFreqMHz();
	json["chipid"] = ESP.getChipId();
	json["sdkversion"] = ESP.getSdkVersion();
	json["bootversion"] = ESP.getBootVersion();
	json["bootmode"] = ESP.getBootMode();
	json["flashid"] = ESP.getFlashChipId();
	json["flashspeed"] = ESP.getFlashChipSpeed();
	json["flashsize"] = ESP.getFlashChipRealSize();
	json["resetreason"] = ESP.getResetReason();
	json["resetinfo"] = ESP.getResetInfo();
	json.printTo(buf, sizeof(buf));
	this->server->send(200, "application/json", buf);
}

//---------------------------------------------------------------------------------------
// extractColor
//
// Converts the given web server argument to a color struct
// -> argName: Name of the web server argument
//    result: Pointer to palette_entry struct to receive result
// <- --
//---------------------------------------------------------------------------------------
void WebServerClass::extractColor(String argName, palette_entry *result)
{
	char c[3];

	if (this->server->hasArg(argName)
			&& this->server->arg(argName).length() == 6)
	{
		String color = this->server->arg(argName);
		color.substring(0, 2).toCharArray(c, sizeof(c));
		result->r = strtol(c, NULL, 16);
		color.substring(2, 4).toCharArray(c, sizeof(c));
		result->g = strtol(c, NULL, 16);
		color.substring(4, 6).toCharArray(c, sizeof(c));
		result->b = strtol(c, NULL, 16);
	}
}

//---------------------------------------------------------------------------------------
// handleSetColor
//
// Handles the "/setcolor" request, expects arguments:
//    /setcolor?fg=xxxxxx&bg=yyyyyy&s=zzzzzz
//    with xxxxxx, yyyyyy and zzzzzz being hexadecimal HTML colors (without leading '#')
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
void WebServerClass::handleSetColor()
{
	this->extractColor("fg", &Config.fg);
	this->extractColor("bg", &Config.bg);
	this->extractColor("s", &Config.s);
	this->server->send(200, "text/plain", "OK");
}

//---------------------------------------------------------------------------------------
// handleSaveConfig
//
// Saves the current configuration to EEPROM
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
void WebServerClass::handleSaveConfig()
{
	Config.save();
	this->server->send(200, "text/plain", "OK");
}

//---------------------------------------------------------------------------------------
// handleLoadConfig
//
// Loads the current configuration from EEPROM
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
void WebServerClass::handleLoadConfig()
{
	Config.load();
	this->server->send(200, "text/plain", "OK");
}

//---------------------------------------------------------------------------------------
// handleMatrix
//
// Sets or resets the showMatrix flag based on agrument "state"
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
void WebServerClass::handleMatrix()
{
	showMatrix = (this->server->hasArg("state")
			&& this->server->arg("state") == "1");
	this->server->send(200, "text/plain", "OK");
}

//---------------------------------------------------------------------------------------
// handleHeartbeat
//
// Sets or resets the heartbeat flag in the configuration based on agrument "state"
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
void WebServerClass::handleHeartbeat()
{
	Config.heartbeat = (this->server->hasArg("state")
			&& this->server->arg("state") == "1");
	this->server->send(200, "text/plain", "OK");
}

//---------------------------------------------------------------------------------------
// handleStars
//
// Sets or resets the showStars flag based on agrument "state"
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
void WebServerClass::handleStars()
{
	showStars = (this->server->hasArg("state")
			&& this->server->arg("state") == "1");
	this->server->send(200, "text/plain", "OK");
}

//---------------------------------------------------------------------------------------
// handleHourglass
//
// Sets or resets the showHourglass flag based on agrument "state"
//
// ->
// <- --
//---------------------------------------------------------------------------------------
void WebServerClass::handleHourglass()
{
	showHourglass = (this->server->hasArg("state")
			&& this->server->arg("state") == "1");
	this->server->send(200, "text/plain", "OK");
}

//---------------------------------------------------------------------------------------
// handleHeart
//
// Sets or resets the showHeart flag based on agrument "state"
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
void WebServerClass::handleHeart()
{
	showHeart = (this->server->hasArg("state")
			&& this->server->arg("state") == "1");
	this->server->send(200, "text/plain", "OK");
}

//---------------------------------------------------------------------------------------
// handleGetColors
//
// Outputs the currently active colors as comma separated list for background, foreground
// and seconds color with 3 values each (red, green, blue)
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
void WebServerClass::handleGetColors()
{
	String message = String(Config.bg.r) + "," + String(Config.bg.g) + ","
			+ String(Config.bg.b) + "," + String(Config.fg.r) + ","
			+ String(Config.fg.g) + "," + String(Config.fg.b) + ","
			+ String(Config.s.r) + "," + String(Config.s.g) + ","
			+ String(Config.s.b);
	this->server->send(200, "text/plain", message);
}
