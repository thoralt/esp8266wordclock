#ifndef _NTP_H_
#define _NTP_H_

#include <stdint.h>
#include <Ticker.h>
#include <WiFiUdp.h>

#define STRING2(i) (((i<10) ? String("0") : String("")) + String(i))

class NtpClass
{
public:
    typedef void(*TNtpCallback)(uint8_t, uint8_t, uint8_t, uint8_t);
    NtpClass();
    void begin(IPAddress ip, TNtpCallback callback, int timezone);
    bool syncInProgress = false;
    void setServer(IPAddress address);
    IPAddress getServer();
private:
    enum class NtpState {idle, startRequest, waitingForReply, waitingForReload};
    
    static void tickerFunctionWrapper(NtpClass *obj);
    void tickerFunction();
    void sendPacket();
    void parse();

    IPAddress timeServer;
    Ticker ticker;
    WiFiUDP udp;

    NtpState state = NtpState::idle;
    TNtpCallback _callback = NULL;
    int timer = 0;
    int h = 0;
    int m = 0;
    int s = 0;
    int ms = 0;
    int tz = 0;
};

extern NtpClass NTP;

#endif


