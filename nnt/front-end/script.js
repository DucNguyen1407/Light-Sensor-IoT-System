let currentPage = 1;
let limit = 10;
let totalPages = 1;
let myChart = null; // Variable to store chart instance

// 1. Initialize when page loads
document.addEventListener("DOMContentLoaded", () => {
    loadDevices(); // Load device list into dropdown
    fetchData();   // Load table data
    
    // Auto refresh every 10s (should not be too fast because of pagination)
    setInterval(() => {
        if(currentPage === 1) fetchData(); 
    }, 10000);
});

// 2. Function to fetch device list for Dropdown
async function loadDevices() {
    try {
        const res = await fetch('/api/devices');
        const devices = await res.json();
        const select = document.getElementById('deviceSelect');
        devices.forEach(dev => {
            const opt = document.createElement('option');
            opt.value = dev;
            opt.innerText = dev;
            select.appendChild(opt);
        });
    } catch (err) {
        console.error("Error loading devices:", err);
    }
}

// 3. Main fetch data function (with pagination)
async function fetchData() {
    try {
        const res = await fetch(`/api/data?page=${currentPage}&limit=${limit}`);
        const json = await res.json();
        
        const data = json.data;
        const pagination = json.pagination;
        totalPages = pagination.totalPages;

        // Render Table
        renderTable(data);
        // Render Chart
        renderChart(data);
        // Update page information
        document.getElementById('pageInfo').innerText = `Page ${pagination.page} / ${pagination.totalPages}`;

    } catch (err) {
        console.error("Error fetching data:", err);
    }
}

// 4. Render Table
function renderTable(data) {
    const tbody = document.getElementById('data-body');
    tbody.innerHTML = '';
    data.forEach(row => {
        const tr = document.createElement('tr');
        tr.innerHTML = `
            <td>${row.id}</td>
            <td><span class="badge">${row.device_id}</span></td>
            <td style="font-weight:bold; color:#2980b9">${row.lux}</td>
            <td>${row.timestamp}</td>
        `;
        tbody.appendChild(tr);
    });
}

// 5. Render Chart (Chart.js)
function renderChart(data) {
    // Reverse array so the chart flows from left to right (old -> new)
    // Since the table shows newest first, but chart time flows forward
    const chartData = [...data].reverse(); 

    const labels = chartData.map(d => d.timestamp.split(' ')[1]); // Get only time
    const luxValues = chartData.map(d => d.lux);

    const ctx = document.getElementById('luxChart').getContext('2d');

    if (myChart) {
        // If chart already exists, just update data
        myChart.data.labels = labels;
        myChart.data.datasets[0].data = luxValues;
        myChart.update();
    } else {
        // If not, create a new one
        myChart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: labels,
                datasets: [{
                    label: 'Cường độ sáng (Lux)',
                    data: luxValues,
                    borderColor: '#4e73df',
                    backgroundColor: 'rgba(78, 115, 223, 0.1)',
                    borderWidth: 2,
                    fill: true,
                    tension: 0.3 // Smooth the curve
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                scales: {
                    y: { beginAtZero: true }
                }
            }
        });
    }
}

// 6. Handle pagination
function changePage(step) {
    const nextPage = currentPage + step;
    if (nextPage > 0 && nextPage <= totalPages) {
        currentPage = nextPage;
        fetchData();
    }
}

function changeLimit() {
    limit = document.getElementById('limitSelect').value;
    currentPage = 1; // Reset to page 1
    fetchData();
}

// 7. Export CSV Function
function exportCSV() {
    const deviceId = document.getElementById('deviceSelect').value;
    const startDate = document.getElementById('startDate').value;
    const endDate = document.getElementById('endDate').value;

    if (!deviceId || !startDate || !endDate) {
        alert("Please select all: Device, Start Date, End Date!");
        return;
    }

    // Create URL query string
    const url = `/api/export-csv?device_id=${deviceId}&start_date=${startDate}&end_date=${endDate}`;
    
    // Open in new tab or download directly
    window.location.href = url;
}