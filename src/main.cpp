#include <ESP8266WiFi.h>
#include "ESP8266HTTPClient.h"
#include <WiFiClientSecure.h>
#include <NewPing.h>

//Network credetials
const char * ssid = "shepherd";
const char * password = "BVrIVumj2Ji1repEC5K29GX4";

//Initialize bot token(from Bot Father)
#define BOT_TOKEN "1509649201:AAFKRc4HnNlvENHhFbTVtzSvbnS19eU29Hg"

//chat id
#define CHAT_ID "802745636"

//Define pins
#define TRIGGER_PIN 14 //D5
#define ECHO_PIN 12 //D6
#define MAX_DISTANCE 200

// NewPing setup of pins and maximum distance
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

const char* url_tmpl = "https://api.telegram.org/bot%s/sendMessage?chat_id=%s&text=%s%d";

const char* host = "api.telegram.org";
const int httpsPort = 443;

// DigiCert High Assurance EV Root CA
const char trustRoot[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDxTCCAq2gAwIBAgIBADANBgkqhkiG9w0BAQsFADCBgzELMAkGA1UEBhMCVVMx
EDAOBgNVBAgTB0FyaXpvbmExEzARBgNVBAcTClNjb3R0c2RhbGUxGjAYBgNVBAoT
EUdvRGFkZHkuY29tLCBJbmMuMTEwLwYDVQQDEyhHbyBEYWRkeSBSb290IENlcnRp
ZmljYXRlIEF1dGhvcml0eSAtIEcyMB4XDTA5MDkwMTAwMDAwMFoXDTM3MTIzMTIz
NTk1OVowgYMxCzAJBgNVBAYTAlVTMRAwDgYDVQQIEwdBcml6b25hMRMwEQYDVQQH
EwpTY290dHNkYWxlMRowGAYDVQQKExFHb0RhZGR5LmNvbSwgSW5jLjExMC8GA1UE
AxMoR28gRGFkZHkgUm9vdCBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkgLSBHMjCCASIw
DQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL9xYgjx+lk09xvJGKP3gElY6SKD
E6bFIEMBO4Tx5oVJnyfq9oQbTqC023CYxzIBsQU+B07u9PpPL1kwIuerGVZr4oAH
/PMWdYA5UXvl+TW2dE6pjYIT5LY/qQOD+qK+ihVqf94Lw7YZFAXK6sOoBJQ7Rnwy
DfMAZiLIjWltNowRGLfTshxgtDj6AozO091GB94KPutdfMh8+7ArU6SSYmlRJQVh
GkSBjCypQ5Yj36w6gZoOKcUcqeldHraenjAKOc7xiID7S13MMuyFYkMlNAJWJwGR
tDtwKj9useiciAF9n9T521NtYJ2/LOdYq7hfRvzOxBsDPAnrSTFcaUaz4EcCAwEA
AaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYwHQYDVR0OBBYE
FDqahQcQZyi27/a9BUFuIMGU2g/eMA0GCSqGSIb3DQEBCwUAA4IBAQCZ21151fmX
WWcDYfF+OwYxdS2hII5PZYe096acvNjpL9DbWu7PdIxztDhC2gV7+AJ1uP2lsdeu
9tfeE8tTEH6KRtGX+rcuKxGrkLAngPnon1rpN5+r5N9ss4UXnT3ZJE95kTXWXwTr
gIOrmgIttRD02JDHBHNA7XIloKmf7J6raBKZV8aPEjoJpL1E/QYVN8Gb5DKj7Tjo
2GTzLH4U/ALqn83/B2gX2yKQOC16jdFU8WnjXzPKej17CuPKf1855eJ1usV2GDPO
LPAvTK33sefOT6jEm0pUBsV/fdUID+Ic/n4XuKxe9tQWskMJDE32p2u0mYRlynqI
4uJEvlz36hz1
-----END CERTIFICATE-----
)EOF";

X509List cert(trustRoot);

unsigned long timeout = 0;

void setup() {
    Serial.begin(115200);
    Serial.println();

    Serial.print("Connecting to ");Serial.println(ssid);    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    //Set time via NTP, as required for x.509 validation
    configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

    Serial.printf("Waiting for NTP time sync: ");
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2) {
        delay(500);
        Serial.printf(".");
        now = time(nullptr);
    }

    Serial.println("");
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);

    Serial.print("Current time: ");
    Serial.println(asctime(&timeinfo));

}


unsigned int measure_distance(){
    unsigned int max_distance = 26;
    unsigned int depth = sonar.ping_cm();
    depth = depth - max_distance;
    Serial.printf("depth :");
    Serial.println(depth);

    return depth;
}


void telegram(const char* msg, unsigned int depth){
        // Use WiFiClientSecure class to create TLS connection
    WiFiClientSecure client;
    Serial.print("Connecting to ");
    Serial.println(host);

    client.setTrustAnchors(&cert);

    if (!client.connect(host, httpsPort)) {
        Serial.println("Connection failed");
        return;
    }
    
    Serial.println("sending msg");
    char url[256];
  
    sprintf(url, url_tmpl, BOT_TOKEN, CHAT_ID, msg, (int)depth);

    HTTPClient https;

    if(https.begin(client, (const char*)url)){
        int status = https.POST("");

        Serial.printf("status: %d\r\n", status);

        if(status != 200){
            Serial.println("Failed to send message");
        }
    }else{
        Serial.println("connection error");
    }
    client.stop();
}



void loop() {

    if(!(millis() - timeout > 5000)){
        return;
    }

    timeout = millis();
    unsigned int depth =  measure_distance();

    const char* yellow= "yellow alert! Depth at ";
    const char*red = "red alert! Depth at ";
    const char*black = "black alert! Depth at ";

    if( depth >= 15 && depth < 26){
        telegram(yellow, depth);
    }else if(depth >= 10 && depth < 15){
        telegram(red, depth);
    }else if(depth >= 0 && depth < 10){
        telegram(black, depth);
    }   

}
