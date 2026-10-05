from flask import Flask, request, jsonify, render_template_string

# Initialize the Flask application
app = Flask(__name__)

# Global dictionary to store the latest alert status from the ESP32
latest_alert = {
    "status": "SAFE", 
    "latitude": 0.0, 
    "longitude": 0.0
}

# ==========================================
# WEB DASHBOARD UI (HTML, CSS & JavaScript)
# ==========================================
HTML_TEMPLATE = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Yantra Accident Alert Dashboard</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; margin-top: 80px; transition: 0.5s; background-color: #f4f4f4; }
        .safe { background-color: #e8f5e9; color: #2e7d32; }
        .danger { background-color: #ffebee; color: #c62828; }
        .box { padding: 40px; border-radius: 15px; display: inline-block; box-shadow: 0 4px 8px rgba(0,0,0,0.2); background: white; }
        h1 { font-size: 35px; margin: 10px 0; }
        a.map-btn { display: inline-block; margin-top: 20px; padding: 15px 30px; font-size: 18px; color: white; background-color: #d32f2f; text-decoration: none; border-radius: 8px; font-weight: bold; }
        a.map-btn:hover { background-color: #b71c1c; }
        /* Reset Button Style */
        .reset-btn { margin-top: 25px; padding: 10px 20px; font-size: 16px; color: white; background-color: #1976d2; border: none; border-radius: 6px; cursor: pointer; font-weight: bold; display: none; }
        .reset-btn:hover { background-color: #115293; }
    </style>
    <script>
        // Function to fetch the latest status from the server every 2 seconds
        function fetchStatus() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    let body = document.body;
                    let content = document.getElementById('content');
                    let resetBtn = document.getElementById('resetBtn');

                    if(data.status === "CRITICAL_ALERT") {
                        // Update UI to show emergency alert and location
                        body.className = "danger";
                        content.innerHTML = `
                            <h1>🚨 CRITICAL ACCIDENT DETECTED! 🚨</h1>
                            <h2>Rider separated from the bike.</h2>
                            <p>Location: Lat ${data.latitude} , Lng ${data.longitude}</p>
                            <a class="map-btn" href="https://www.google.com/maps?q=${data.latitude},${data.longitude}" target="_blank">
                                📍 View Live on Google Maps
                            </a>
                        `;
                        resetBtn.style.display = "inline-block"; // Show reset button during alert
                    } else {
                        // Update UI to show safe monitoring state
                        body.className = "safe";
                        content.innerHTML = `
                            <h1 style="color: #2e7d32;">✅ System Monitoring...</h1>
                            <p>Rider is safe. Waiting for sensor data.</p>
                        `;
                        resetBtn.style.display = "none"; // Hide reset button when safe
                    }
                });
        }

        // Function to reset status back to safe via button click
        function resetSystem() {
            fetch('/api/reset', { method: 'POST' })
                .then(response => response.json())
                .then(data => {
                    console.log("System reset to safe");
                    fetchStatus(); // Immediately update UI
                });
        }

        // Run the fetchStatus function every 2000 milliseconds (2 seconds)
        setInterval(fetchStatus, 2000);
    </script>
</head>
<body class="safe">
    <div class="box" id="content">
        <h1 style="color: #2e7d32;">✅ System Monitoring...</h1>
        <p>Loading data...</p>
    </div>
    <br>
    <!-- Reset Button -->
    <button id="resetBtn" class="reset-btn" onclick="resetSystem()">🔄 Reset Status to Safe</button>
</body>
</html>
"""

# ==========================================
# 1. Route to serve the Web Dashboard
# ==========================================
@app.route('/')
def index():
    return render_template_string(HTML_TEMPLATE)

# ==========================================
# 2. Route to receive Alert Data from ESP32
# ==========================================
@app.route('/api/alert', methods=['POST'])
def receive_alert():
    global latest_alert
    incoming_data = request.json
    
    latest_alert["status"] = "CRITICAL_ALERT"
    latest_alert["latitude"] = incoming_data.get("latitude", 0.0)
    latest_alert["longitude"] = incoming_data.get("longitude", 0.0)
    
    print(f"\n🚨 ALERT RECEIVED FROM ESP32! Lat: {latest_alert['latitude']}, Lng: {latest_alert['longitude']}\n")
    return jsonify({"message": "Alert received successfully!"}), 200

# ==========================================
# 3. Route to send current status to Web Page
# ==========================================
@app.route('/api/status', methods=['GET'])
def get_status():
    return jsonify(latest_alert)

# ==========================================
# 4. Route to Reset Status Back to Safe (New)
# ==========================================
@app.route('/api/reset', methods=['POST'])
def reset_status():
    global latest_alert
    latest_alert["status"] = "SAFE"
    print("\n🔄 System status manually reset to SAFE from Dashboard.\n")
    return jsonify({"message": "System reset to safe!"}), 200

# ==========================================
# START THE SERVER
# ==========================================
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)