#pragma once

#include <QString>
#include <cmath>

class Airport {
private:
    QString m_code;
    double m_latitude;
    double m_longitude;
    
public:
    Airport() = default;
    Airport(QString code, double lat, double lon);
    
    const QString& code() const { return m_code; }
    double latitude() const { return m_latitude; }
    double longitude() const { return m_longitude; }
    
    double distanceTo(const Airport& other) const;
    bool isValid() const;
    
    bool operator==(const Airport& other) const;
    bool operator!=(const Airport& other) const;
    bool operator<(const Airport& other) const;
};