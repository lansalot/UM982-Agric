#include <iostream>
#include <iomanip>
#include <cmath>

struct GeoPoint
{
    double lat;   // degrees
    double lon;   // degrees
    double alt;   // meters (ground altitude after correction)
};

constexpr double DEG2RAD = M_PI / 180.0;
constexpr double RAD2DEG = 180.0 / M_PI;

// meters per degree latitude (good local approximation)
constexpr double METERS_PER_DEG_LAT = 111320.0;

GeoPoint compensateAntenna(
    double latDeg,
    double lonDeg,
    double altGPS,
    double antennaHeight,
    double rollDeg,
    double headingDeg)
{
    double latRad = latDeg * DEG2RAD;
    double roll   = rollDeg * DEG2RAD;
    double heading = headingDeg * DEG2RAD;

    // --- vertical correction ---
    double altGround = altGPS - antennaHeight * std::cos(roll);

    // --- horizontal displacement ---
    double d = antennaHeight * std::sin(roll);

    // sideways direction (perpendicular to heading)
    double east  = d * std::cos(heading + M_PI_2);
    double north = d * std::sin(heading + M_PI_2);

    // convert meters → degrees
    double dLat = north / METERS_PER_DEG_LAT;
    double dLon = east / (METERS_PER_DEG_LAT * std::cos(latRad));

    GeoPoint result;
    result.lat = latDeg - dLat;
    result.lon = lonDeg - dLon;
    result.alt = altGround;

    return result;
}


int main()
{
    // Example GPS + IMU readings
    double latGPS = 51.234567;
    double lonGPS = -113.987654;
    double altGPS = 52.0;     // meters above sea level (antenna)
    double roll   = 10.0;     // degrees
    double heading = 12.0;    // degrees

    double antennaHeight = 4.0; // meters

    GeoPoint ground = compensateAntenna(
        latGPS,
        lonGPS,
        altGPS,
        antennaHeight,
        roll,
        heading
    );

    std::cout << "Corrected ground position:\n";
    std::cout << "Lat: " << ground.lat << "\n";
    std::cout << "Lon: " << ground.lon << "\n";
    std::cout << "Alt: " << ground.alt << " m\n";

    return 0;
}







#include <iostream>
#include <cmath>
#include <iomanip>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct GPSCoord {
    double lat;
    double lon;
    double alt;
};

/**
 * @brief Compensates GPS position for vehicle roll.
 * * @param raw Lat/Long/Alt from the GPS unit.
 * @param roll_deg Roll angle in degrees (positive = roll right, negative = roll left).
 * @param heading_deg Vehicle compass heading (0 = North, 90 = East).
 * @param antenna_height_m Height of antenna from road when level (e.g., 2.0).
 * @return GPSCoord The corrected ground-level coordinates.
 */
GPSCoord compensate_roll(GPSCoord raw, double roll_deg, double heading_deg, double antenna_height_m) {
    // 1. Convert degrees to radians
    double roll_rad = roll_deg * (M_PI / 180.0);
    double heading_rad = heading_deg * (M_PI / 180.0);

    // 2. Calculate the horizontal displacement magnitude (d)
    // and the vertical height of the antenna above the ground (v_height)
    double d = antenna_height_m * std::sin(roll_rad);
    double v_height = antenna_height_m * std::cos(roll_rad);

    // 3. Determine the bearing of the offset. 
    // If car rolls right, the antenna moved to the right of the heading.
    // To find the ROAD center, we must move the point in the OPPOSITE direction.
    double correction_bearing = heading_rad - (M_PI / 2.0); 
    if (roll_deg < 0) correction_bearing = heading_rad + (M_PI / 2.0);

    // 4. Calculate Lat/Lon offsets (Flat-Earth approximation for small d)
    const double EARTH_RADIUS = 6378137.0; // WGS84 Radius in meters
    
    double d_lat = d * std::cos(correction_bearing);
    double d_lon = d * std::sin(correction_bearing);

    GPSCoord corrected;
    corrected.lat = raw.lat + (d_lat / EARTH_RADIUS) * (180.0 / M_PI);
    corrected.lon = raw.lon + (d_lon / (EARTH_RADIUS * std::cos(raw.lat * M_PI / 180.0))) * (180.0 / M_PI);
    
    // 5. Correct Altitude
    // The GPS height (52m) minus the tilted vertical height of the antenna.
    corrected.alt = raw.alt - v_height;

    return corrected;
}

int main() {
    GPSCoord antenna_gps = {51.5074, -0.1278, 152.0}; // London, 52m Alt
    double roll = 10.0;     // 10 degrees right
    double heading = 45.0;  // Facing East
    double h = 2.0;         // 2 meters high

    GPSCoord ground = compensate_roll(antenna_gps, roll, heading, h);

    std::cout << std::fixed << std::setprecision(8);
    std::cout << "Raw GPS:  " << antenna_gps.lat << ", " << antenna_gps.lon << " Alt: " << antenna_gps.alt << "m\n";
    std::cout << "Ground:   " << ground.lat << ", " << ground.lon << " Alt: " << ground.alt << "m\n";

    return 0;
}


