# WiFi Light Sensor Monitoring System

A real-time light monitoring system using ESP32 microcontroller with BH1750 sensor, featuring WiFi connectivity and web-based dashboard.

## 🎯 Project Overview

This IoT project implements a comprehensive light monitoring solution that collects ambient light data in real-time and displays it through an intuitive web interface. The system uses ESP32 for data collection and WiFi transmission, with a Node.js backend for data processing and storage.

## ✨ Key Features

- **Real-time Monitoring**: Continuous light intensity measurement using BH1750 sensor
- **WiFi Connectivity**: Wireless data transmission via ESP32
- **Web Dashboard**: Interactive web interface for data visualization
- **Data Persistence**: SQLite database for historical data storage
- **Data Export**: CSV export functionality for data analysis
- **Line Charts**: Visual representation of light intensity trends
- **Pagination**: Efficient data browsing with customizable page sizes

## 🔧 Hardware Components

### PCB Design Specifications
- **Size**: 7cm × 3cm (compact design)
- **Layer**: 2-layer PCB
- **Power Traces**: 24-25 mil width (handles WiFi peak current)
- **Signal Traces**: 10 mil width
- **Copper Pour**: Full GND plane on both layers for heat dissipation

### Components List
1. **Microcontroller**: ESP32 Module (WiFi-enabled)
2. **Sensor**: BH1750 Light Sensor (I2C interface)
3. **Power Supply**: 5V (USB or adapter)
4. **Interface**: USB-to-TTL converter
5. **User Interface**: 
   - Reset button
   - User button
   - Status LED

### Pin Configuration
- **I2C Communication**: SDA, SCL pins for BH1750 connection
- **Power**: 3.3V LDO regulator (AMS1117-3.3)
- **USB Interface**: For programming and power

## 💻 Software Stack

### Firmware (ESP32)
- **Language**: C/C++ (ESP-IDF framework)
- **Libraries**: 
  - BH1750 driver
  - WiFi library
  - HTTP client
- **Storage**: NVS (Non-Volatile Storage) for WiFi credentials

### Backend
- **Runtime**: Node.js
- **Framework**: Express.js
- **Database**: SQLite
- **Features**:
  - RESTful API endpoints
  - Data validation
  - Error handling
  - CSV export functionality

### Frontend
- **Technologies**: HTML5, CSS3, JavaScript
- **Charts**: Dynamic line chart visualization
- **Features**:
  - Real-time data display
  - Date range filtering
  - Pagination controls
  - Responsive design

## 📈 Data Flow

1. **Data Collection**: BH1750 sensor measures ambient light in Lux
2. **Data Processing**: ESP32 reads sensor data via I2C
3. **Data Transmission**: ESP32 sends JSON payload to server via HTTP POST
4. **Data Storage**: Server stores data in SQLite database
5. **Data Visualization**: Frontend fetches and displays data via HTTP GET

### JSON Data Format
```json
{
  "device_id": "bh1750",
  "lux": 320.83,
  "timestamp": "2025-12-23 18:00:36"
}
```

## 🎯 Web Interface Features

### Dashboard Components
- **Real-time Display**: Latest sensor reading with timestamp
- **Line Chart**: Visualizes light intensity trends over time
- **Data Table**: Paginated view of historical readings
- **Date Filter**: Custom date range selection
- **Export**: Download data as CSV for external analysis

### Chart Visualization
- X-axis: Timestamp (formatted)
- Y-axis: Light intensity (Lux)
- Interactive tooltips
- Responsive design

## 🔮 Future Enhancements

1. **WebSocket Integration**: Real-time bidirectional communication for instant updates
2. **MySQL Migration**: Scalable database solution for larger deployments
3. **MQTT Protocol**: Efficient IoT messaging for reduced bandwidth usage
4. **Alert System**: Configurable thresholds with notifications
5. **Multi-sensor Support**: Expand to monitor multiple devices simultaneously
6. **Mobile App**: Native mobile application for iOS/Android
7. **Data Analytics**: Advanced analytics with ML predictions
8. **Cloud Integration**: AWS/Azure integration for cloud storage

## 🤝 Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.

ystems course, demonstrating IoT concepts, hardware design, and full-stack web development skills.
