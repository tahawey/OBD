#include <ACAN_ESP32.h>

void setup() {
  Serial.begin(115200);
  ACAN_ESP32_Settings settings(500000); // CAN baud rate: 500 kbps
  const uint32_t errorCode = ACAN_ESP32::can.begin(settings);
  if (errorCode != 0) {
    Serial.print("CAN error: 0x");
    Serial.println(errorCode, HEX);
  }
}

void loop() {
  CANMessage request;
  if (ACAN_ESP32::can.receive(request)) {
    if (request.id == 0x7DF) { // OBD-II request ID
      handleObdRequest(request);
    }
  }
}

void handleObdRequest(CANMessage &request) {
  uint8_t pid = request.data[2]; // Extract PID from request
  CANMessage response;
  response.id = 0x7E8; // ECU response ID
  response.len = 8;

  switch (pid) {
    case 0x0C: // RPM
      response.data[0] = 0x04; // 4 data bytes
      response.data[1] = 0x41; // Response mode (0x40 + 0x01)
      response.data[2] = 0x0C; // PID 0x0C
      response.data[3] = 0x0A; // Simulated RPM: (0x0A * 256 + 0x00) / 4 = 640 RPM
      response.data[4] = 0x00;
      break;
    case 0x0D: // Speed
      response.data[0] = 0x03;
      response.data[1] = 0x41;
      response.data[2] = 0x0D;
      response.data[3] = 0x32; // 50 km/h
      break;
    // Add more PIDs as needed
  }
  ACAN_ESP32::can.tryToSend(response);
}