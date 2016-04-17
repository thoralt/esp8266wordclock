#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_

#include <stdint.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "config.h"

class WebServerClass
{
public:
	WebServerClass();
	virtual ~WebServerClass();
	void begin();
	void process();

	bool showMatrix = false;
	bool showHeart = false;
	bool showStars = false;
	bool showHourglass = false;

private:
	ESP8266WebServer *server = NULL;

	String contentType(String filename);
	bool serveFile(String path);
	void handleSaveConfig();
	void handleLoadConfig();
	void handleHourglass();
	void handleGetColors();
	void handleSetColor();
	void handleNotFound();
	void handleMatrix();
	void handleHeart();
	void handleHeartbeat();
	void handleStars();
	void handleInfo();
	void handleGetNtpServer();
	void handleSetNtpServer();
	void extractColor(String argName, palette_entry *result);
};

extern WebServerClass WebServer;

#endif
