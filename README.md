# WiFi Light Sensor Monitoring System


## Project Overview

This IoT project implements a comprehensive light monitoring solution that collects ambient light data in real-time and displays it through an intuitive web interface. The system uses ESP32 for data collection and WiFi transmission, with a Node.js backend for data processing and storage.

## Key Features

- **Real-time Monitoring**: Continuous light intensity measurement using BH1750 sensor
- **WiFi Connectivity**: Wireless data transmission via ESP32
- **Web Dashboard**: Interactive web interface for data visualization
- **Data Persistence**: SQLite database for historical data storage
- **Data Export**: CSV export functionality for data analysis
- **Line Charts**: Visual representation of light intensity trends
- **Pagination**: Efficient data browsing with customizable page sizes

## Hardware Components

### PCB Design Specifications
- **Size**: 7cm × 3cm (compact design)
- **Layer**: 2-layer PCB
- **Power Traces**: 24-25 mil width (handles WiFi peak current)
- **Signal Traces**: 10 mil width
- **Copper Pour**: Full GND plane on both layers for heat dissipation



<img width="1237" height="953" alt="{8F514837-F5FA-4D86-8F13-A0FCF39B3A38}" src="https://github.com/user-attachments/assets/d9367567-16ae-4001-8479-79ca454220f0" />

<img width="709" height="280" alt="{FAE5E07A-408F-491F-9583-0318D20D0B88}" src="https://github.com/user-attachments/assets/28731586-f16b-47b4-a1af-0202cfe0f660" />

<img width="719" height="262" alt="{5878CBF8-9B60-4320-BC85-2A5D61C85664}" src="https://github.com/user-attachments/assets/99771e8a-9007-4c0f-adef-d65e17917021" />





## Software Stack

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

## Data Flow

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

## Web Interface Features

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

<img width="1145" height="834" alt="{CF54610A-CD9C-435D-B40D-D840E8C8FA32}" src="https://github.com/user-attachments/assets/c18394af-d797-4b0d-bf65-70a3ae2331ae" />





## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.
