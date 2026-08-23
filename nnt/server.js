const express = require('express');
const sqlite3 = require('sqlite3').verbose();
const path = require('path');
const fs = require('fs');

const app = express();
const port = 3000;

app.use(express.json());
app.use(express.static('front-end'));

// Connect to SQLite
const dbPath = path.resolve(__dirname, 'data/database.sqlite');
// Create data directory if it doesn't exist
const dir = path.dirname(dbPath);
if (!fs.existsSync(dir)){
    fs.mkdirSync(dir);
}

const db = new sqlite3.Database(dbPath, (err) => {
    if (err) console.error("DB connection error:", err.message);
    else {
        console.log("DB Connected.");
        db.run(`CREATE TABLE IF NOT EXISTS sensor_data (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            device_id TEXT NOT NULL,
            lux REAL NOT NULL,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
        )`);
    }
});

// ---- API ENDPOINTS ----

// 1. Receive data from ESP32
app.post('/api/data', (req, res) => {
    const { device_id, lux } = req.body;
    // Create timestamp in Vietnam time
    const now = new Date();
    const offsetMs = 7 * 60 * 60 * 1000;
    const vnTime = new Date(now.getTime() + offsetMs);
    const timestamp = vnTime.toISOString().replace('T', ' ').slice(0, 19);

    if (!device_id || lux === undefined) {
        return res.status(400).json({ error: "Missing information" });
    }

    const sql = `INSERT INTO sensor_data (device_id, lux, timestamp) VALUES (?, ?, ?)`;
    db.run(sql, [device_id, lux, timestamp], function(err) {
        if (err) return res.status(500).json({ error: err.message });
        console.log(`Saved: ${device_id} - ${lux}`);
        res.status(201).json({ message: "Success", id: this.lastID });
    });
});

// 2. Get PAGINATED data for table and chart
app.get('/api/data', (req, res) => {
    // Default to page 1, 10 rows per page if no parameters provided
    const page = parseInt(req.query.page) || 1;
    const limit = parseInt(req.query.limit) || 10;
    const offset = (page - 1) * limit;

    // Query to count total rows for pagination
    const countSql = `SELECT COUNT(*) as total FROM sensor_data`;
    
    db.get(countSql, [], (err, row) => {
        if (err) return res.status(500).json({ error: err.message });
        const totalRecords = row.total;
        const totalPages = Math.ceil(totalRecords / limit);

        // Query to get paginated data
        const dataSql = `SELECT * FROM sensor_data ORDER BY timestamp DESC LIMIT ? OFFSET ?`;
        db.all(dataSql, [limit, offset], (err, rows) => {
            if (err) return res.status(500).json({ error: err.message });
            
            res.json({
                data: rows,
                pagination: {
                    page: page,
                    limit: limit,
                    totalRecords: totalRecords,
                    totalPages: totalPages
                }
            });
        });
    });
});

// 3. Get list of Device IDs (to display in device selection dropdown)
app.get('/api/devices', (req, res) => {
    const sql = `SELECT DISTINCT device_id FROM sensor_data`;
    db.all(sql, [], (err, rows) => {
        if (err) return res.status(500).json({ error: err.message });
        res.json(rows.map(r => r.device_id));
    });
});

// 4. EXPORT CSV (Important)
app.get('/api/export-csv', (req, res) => {
    const { device_id, start_date, end_date } = req.query;

    if (!device_id || !start_date || !end_date) {
        return res.status(400).send("Please select Device, Start Date, and End Date.");
    }

    // Convert input format (if needed) to match DB: 'YYYY-MM-DD HH:MM:SS'
    // Note: html datetime-local input returns 'YYYY-MM-DDTHH:MM', need to replace T with space
    const startStr = start_date.replace('T', ' ');
    const endStr = end_date.replace('T', ' ');

    const sql = `SELECT * FROM sensor_data WHERE device_id = ? AND timestamp BETWEEN ? AND ? ORDER BY timestamp ASC`;

    db.all(sql, [device_id, startStr, endStr], (err, rows) => {
        if (err) return res.status(500).send("Database Error");

        // Manually create CSV content (or use fast-csv library if preferred)
        let csvContent = "ID,Device ID,Lux,Timestamp\n"; // Header
        rows.forEach(row => {
            csvContent += `${row.id},${row.device_id},${row.lux},${row.timestamp}\n`;
        });

        // Set header so the browser understands this is a downloaded file
        res.setHeader('Content-Type', 'text/csv');
        res.setHeader('Content-Disposition', `attachment; filename=data_${device_id}_${Date.now()}.csv`);
        res.status(200).send(csvContent);
    });
});

app.listen(port, () => {
    console.log(`Server running at http://localhost:${port}`);
});