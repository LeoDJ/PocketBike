#include <USB/PluggableUSBSerial.h>
#include <TinyGPS++.h>
#include <ArduinoJson.h>

extern UART uart1_GPS;
extern USBSerial SerialUSB1_GPS, SerialUSB2_BMS;

TinyGPSPlus gps;

// term number 1 is the term after the Message ID (e.g. "$GNGST"), so one needs to subtract one from normal term numbers
TinyGPSCustom GST_stdLat(gps, "GNGST", 6);
TinyGPSCustom GST_stdLng(gps, "GNGST", 7);
TinyGPSCustom GST_stdAlt(gps, "GNGST", 8);

TinyGPSCustom RMC_posMode(gps, "GNRMC", 12);

TinyGPSCustom GSA_fix(gps, "GNGSA", 2);


void gpsLoop() {
    if (!SerialUSB1_GPS.connected()) { // check if passthrough is not active
        while (uart1_GPS.available()) {
            gps.encode(uart1_GPS.read());
        }

        if (gps.satellites.isUpdated() ||
            gps.speed.isUpdated() ||
            GST_stdLat.isUpdated() ||
            GSA_fix.isUpdated() ) {

            JsonDocument doc;
            doc["timestamp_controller"] = millis();
            doc["system"] = "gps";
            doc["type"] = "";
            JsonObject data = doc["data"].to<JsonObject>();

            if (gps.satellites.isUpdated()) {   // GGA packet
                doc["type"] = "GGA";

                data["lat"] = gps.location.lat();
                data["lng"] = gps.location.lng();
                data["alt"] = gps.altitude.meters();
                data["numSV"] = gps.satellites.value();
            }
            else if (gps.speed.isUpdated()) {    // RMC packet
                // Wait what? Despite the GPS module not sending any RMC packets that I can see,
                // TinyGPS++ somehow manages to get those values anyways?! And the time is correct! HOW?
                // => OK, the RMC packet seems pretty seldom / maybe gets borked by the Serial passthrough when viewing externally?
                doc["type"] = "RMC";

                data["speed"] = gps.speed.kmph();
                data["heading"] = gps.course.deg();
                data["mode"] = RMC_posMode.value();
                char buf[32];
                snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02d", 
                    gps.date.year(), gps.date.month(), gps.date.day(),
                    gps.time.hour(), gps.time.minute(), gps.time.second());
                data["time"] = buf;
            }
            else if (GST_stdLat.isUpdated()) {   // GST packet
                doc["type"] = "GST";
                
                float stdLat = TinyGPSPlus::parseDecimal(GST_stdLat.value()) / 100;
                float stdLng = TinyGPSPlus::parseDecimal(GST_stdLng.value()) / 100;
                float stdAlt = TinyGPSPlus::parseDecimal(GST_stdAlt.value()) / 100;
                data["acc_m"] = sqrt(stdLat * stdLat + stdLng * stdLng + stdAlt * stdAlt);
            }
            else if (GSA_fix.isUpdated()) {     // GSA packet
                doc["type"] = "GSA";
                data["fixType"] = GSA_fix.value();
                SerialUSB2_BMS.println(GSA_fix.value());
            }

            serializeJson(doc, Serial);
            Serial.print('\n');
        }
    }
}
