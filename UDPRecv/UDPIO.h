//====================================================
// program name : UDPIO.h
// date/author  : 2022/01/25  @chrmlinux03
//====================================================

#include <WiFi.h>
#include <WiFiUdp.h>
#define UDP_MAX_BYTE (1460)
#define BUF_MAX (1024 * 80)

WiFiUDP udp;
enum {RECV = 0, SEND};

static int udpMode;
uint8_t _buf[BUF_MAX] = {0};

const char *ssid = "M5UDPSAMPLE";
const char *pass = "#M5UDPSAMPLE#";
const int localPort = 50000;
const IPAddress    ipadrs(192, 168,  4,  1);
const IPAddress broadcast(192, 168,  4,255);
const IPAddress    subnet(255, 255,255,  0);


void setupAp() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(ipadrs, ipadrs, subnet);
  WiFi.softAP(ssid, pass);
  Serial.print("\nAPIP : ");
  Serial.print(WiFi.softAPIP());
  Serial.print(":");
  Serial.println(localPort);
  udp.begin(localPort);
  Serial.println("Recv");
  Serial.println("Ready");
}

void setupSta() {
  WiFi.persistent(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nSTAIP : "); Serial.println(WiFi.localIP());
  Serial.print("RSSI : "); Serial.println(WiFi.RSSI());
  udp.begin(localPort);
  Serial.println("Ready");
  Serial.println("Send");
}

void setupWifi() {
  if (!udpMode) {
    setupAp();
  } else {
    setupSta();
  }
}

void recvUdp() {
  uint8_t buf[UDP_MAX_BYTE] = {0};
  static uint16_t pos = 0;
  static uint16_t len = 0;

  uint16_t sz = udp.parsePacket();
  if (sz) {
    int16_t rtn = udp.read(buf, sz);
    if (rtn == 2) {
      len = buf[0] * 256 + buf[1];
      pos = 0;
      Serial.printf("size:%d\n", len);
    } else {
      memcpy(&_buf[pos], buf, sizeof(rtn));
      pos += rtn;
      Serial.printf("pos:%d\n", pos);
    }
  }
}

void sendUdp(int flag) {
  static uint8_t c = 0;
  uint8_t dt[2] = {0};
  
  if (!flag) udp.beginPacket(ipadrs, localPort);
  else       udp.beginPacket(broadcast, localPort);
  dt[0] = highByte(sizeof(_buf));
  dt[1] = lowByte(sizeof(_buf));
  udp.write(dt, sizeof(dt));
  udp.endPacket();

  if (!flag) udp.beginPacket(ipadrs, localPort);
  else       udp.beginPacket(broadcast, localPort);
  udp.write(_buf, sizeof(_buf));
  udp.endPacket();
  c = (c + 1) % 256;

}

void loopUdp(bool flag = false)
{
  if (!udpMode) {
    recvUdp();
  } else {
    sendUdp(flag);
  }
}
