#include "Airport.h"
#include <cmath>

Airport::Airport(QString code, double lat, double lon)
    : m_code(std::move(code)), m_latitude(lat), m_longitude(lon)
{
}

double Airport::distanceTo(const Airport& other) const {
    const double R = 6371.0; // км
    
    double lat1 = m_latitude * M_PI / 180.0;
    double lon1 = m_longitude * M_PI / 180.0;
    double lat2 = other.m_latitude * M_PI / 180.0;
    double lon2 = other.m_longitude * M_PI / 180.0;
    
    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;
    
    double a = sin(dlat/2) * sin(dlat/2) +
            cos(lat1) * cos(lat2) *
            sin(dlon/2) * sin(dlon/2);
    
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    
    return R * c;
}

bool Airport::isValid() const {
    return !m_code.isEmpty() && 
           m_latitude >= -90 && m_latitude <= 90 &&
           m_longitude >= -180 && m_longitude <= 180;
}

bool Airport::operator==(const Airport& other) const {
    return m_code == other.m_code;
}

bool Airport::operator!=(const Airport& other) const {
    return m_code != other.m_code;
}

bool Airport::operator<(const Airport& other) const {
    return m_code < other.m_code;
}