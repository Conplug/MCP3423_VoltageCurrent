//
// Copyright (c) 2019 Conplug
// Author: Hartman Hsieh
//
//   ADC : MCP3423
//
// Required Library :
//   https://github.com/uChip/MCP342X
//

#include  <Wire.h>
#include  <MCP342X.h>

MCP342X myADC (0x6E);

const double RVH = 51.0 * 1000; // resistance divider
const double RVL = 1.1 * 1000; // resistance divider
const double RI = 0.01; // shunt resistance

static int16_t result1 = 0, result2 = 0;
double Voltage = 0.0;
double Current = 0.0;

void setup() {

  Wire.begin();  // join I2C bus
  TWBR = 12;  // 400 kHz (maximum)
  
  Serial.begin(9600); // Open serial connection to send info to the host
  while (!Serial) {}  // wait for Serial comms to become ready
  Serial.println("Starting up");
  Serial.println("Testing device connection...");
  Serial.println(myADC.testConnection() ? "MCP342X connection successful" : "MCP342X connection failed");

}  // End of setup()

void loop() {

  //
  // Read the data from channel 1
  //
  myADC.configure( MCP342X_MODE_CONTINUOUS |
                   MCP342X_CHANNEL_1 |
                   MCP342X_SIZE_16BIT |
                   MCP342X_GAIN_1X
                 );
  myADC.startConversion();
  myADC.getResult(&result1);
  result1 &= 0xfff0;

  //
  // From MCP3423 spec.
  // http://ww1.microchip.com/downloads/en/DeviceDoc/22088b.pdf
  //
  // On-Board Voltage Reference (V REF):
  // - Accuracy: 2.048V ± 0.05%
  //
  // MINIMUM AND MAXIMUM OUTPUT CODES
  // +--------------------------------------------------------------+
  // | Resolution Setting | Data Rate | Minimum Code | Maximum Code |
  // +--------------------------------------------------------------+
  // | 16                 | 15 SPS    | -32768       | 32767        |
  // +--------------------------------------------------------------+
  //
  // Calculate voltage
  //
  Voltage = (((RVH + RVL) * 2.048) / (RVL * 32767.0)) * result1;

  //
  // Read the data from channel 2
  //
  myADC.configure( MCP342X_MODE_CONTINUOUS |
                   MCP342X_CHANNEL_2 |
                   MCP342X_SIZE_16BIT |
                   MCP342X_GAIN_1X
                 );
  myADC.startConversion();
  myADC.getResult(&result2);

  //
  // Calculate current
  //
  Current = ((2.048 * result2) / 32767.0) / RI;

  //
  // Print voltage and current
  //
  Serial.print(Voltage);
  Serial.print("V,");
  Serial.print(Current);
  Serial.println("A");

  delay(500);

}  // End of loop()
