#include <Arduino.h>
#include <math.h>

// ---------- Camp position (degrees) ----------
#define CAMP_LAT        18.556283
#define CAMP_LON        73.955067
#define EARTH_RADIUS_KM 6371.0

#define SERIAL_BAUD     9600        // as in the task: set the Serial Monitor to 9600

const char *const DIRECTIONS[8] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};

// "X=18.543070,Y=73.852389" -> lat, lon. False if the format is wrong.
bool parseCoordinates(String text, double &lat, double &lon) {
  text.replace(" ", "");            // also accept "X=18.5, Y=73.8"
  int yPos = text.indexOf(",Y=");
  if (!text.startsWith("X=") || yPos < 0) {
    return false;
  }
  lat = text.substring(2, yPos).toDouble();
  lon = text.substring(yPos + 3).toDouble();
  return true;
}

// Great-circle distance with the haversine formula, in km
double distanceKm(double lat1, double lon1, double lat2, double lon2) {
  double dLat = radians(lat2 - lat1);
  double dLon = radians(lon2 - lon1);
  double a = sin(dLat / 2) * sin(dLat / 2) +
             cos(radians(lat1)) * cos(radians(lat2)) * sin(dLon / 2) * sin(dLon / 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  return EARTH_RADIUS_KM * c;
}

// Initial bearing from point 1 to point 2, 0..360 degrees (0 = north, 90 = east)
double bearingDeg(double lat1, double lon1, double lat2, double lon2) {
  double p1 = radians(lat1);
  double p2 = radians(lat2);
  double dLon = radians(lon2 - lon1);
  double y = sin(dLon) * cos(p2);
  double x = cos(p1) * sin(p2) - sin(p1) * cos(p2) * cos(dLon);
  double bearing = degrees(atan2(y, x));
  return fmod(bearing + 360.0, 360.0);
}

const char *compassPoint(double bearing) {   // 8 sectors of 45 degrees, N centred on 0
  int index = (int)((bearing + 22.5) / 45.0) % 8;
  return DIRECTIONS[index];
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  Serial.println("Send your position as X=<latitude>,Y=<longitude>");
}

void loop() {
  if (Serial.available() > 0) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) {
      return;
    }
    double lat, lon;
    if (!parseCoordinates(line, lat, lon)) {
      Serial.println("Error: Invalid GPS format");
      return;
    }
    double km = distanceKm(lat, lon, CAMP_LAT, CAMP_LON);
    double bearing = bearingDeg(lat, lon, CAMP_LAT, CAMP_LON);
    Serial.print("Distance to camp: ");
    Serial.print(km, 2);
    Serial.println(" km");
    Serial.print("Direction to camp: ");
    Serial.println(compassPoint(bearing));
  }
}
