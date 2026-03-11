from flask import Flask, render_template, jsonify, request, make_response
import serial
import serial.tools.list_ports
import threading
import time

app = Flask(__name__)
app.config['TEMPLATES_AUTO_RELOAD'] = True
app.config['SEND_FILE_MAX_AGE_DEFAULT'] = 0

arduino = None
serial_lock = threading.Lock()
log_messages = []

# ─── Serial Helpers ──────────────────────────────────────────────────────────

def log(msg):
    timestamp = time.strftime("%H:%M:%S")
    entry = f"[{timestamp}] {msg}"
    log_messages.append(entry)
    if len(log_messages) > 50:
        log_messages.pop(0)
    print(entry)

def send_command(cmd):
    global arduino
    with serial_lock:
        if arduino and arduino.is_open:
            try:
                arduino.write((cmd + '\n').encode())
                time.sleep(0.05)
                log(f"Sent: {cmd}")
                return True
            except Exception as e:
                log(f"Send error: {e}")
                return False
    log("Not connected to Arduino")
    return False

# ─── Routes ──────────────────────────────────────────────────────────────────

@app.route('/')
def index():
    resp = make_response(render_template('index.html'))
    resp.headers['Cache-Control'] = 'no-store, no-cache, must-revalidate, max-age=0'
    resp.headers['Pragma'] = 'no-cache'
    resp.headers['Expires'] = '0'
    return resp

@app.route('/api/ports')
def get_ports():
    ports = []
    for p in serial.tools.list_ports.comports():
        ports.append({'device': p.device, 'description': p.description})
    return jsonify({'ports': ports})

@app.route('/api/connect', methods=['POST'])
def connect():
    global arduino
    port = request.json.get('port')
    baud = request.json.get('baud', 9600)
    try:
        if arduino and arduino.is_open:
            arduino.close()
        arduino = serial.Serial(port, baud, timeout=1)
        time.sleep(2)  # Wait for Arduino reset
        log(f"Connected to {port} at {baud} baud")
        return jsonify({'success': True, 'message': f'Connected to {port}'})
    except Exception as e:
        log(f"Connection failed: {e}")
        return jsonify({'success': False, 'message': str(e)})

@app.route('/api/disconnect', methods=['POST'])
def disconnect():
    global arduino
    if arduino and arduino.is_open:
        arduino.close()
        arduino = None
        log("Disconnected from Arduino")
    return jsonify({'success': True})

@app.route('/api/status')
def status():
    connected = arduino is not None and arduino.is_open
    return jsonify({'connected': connected})

@app.route('/api/move', methods=['POST'])
def move():
    data  = request.json
    joint = data.get('joint')   # B, S, E, G
    angle = int(data.get('angle', 90))
    cmd   = f"{joint}{angle}"
    success = send_command(cmd)
    return jsonify({'success': success, 'command': cmd})

@app.route('/api/home', methods=['POST'])
def home():
    success = send_command('H')
    return jsonify({'success': success})

@app.route('/api/demo', methods=['POST'])
def demo():
    success = send_command('P')
    return jsonify({'success': success})

@app.route('/api/logs')
def get_logs():
    return jsonify({'logs': log_messages})

# ─── Run ─────────────────────────────────────────────────────────────────────

if __name__ == '__main__':
    print("\n" + "="*45)
    print("  Robotic Arm Controller")
    print("  Open: http://localhost:5000")
    print("="*45 + "\n")
    app.run(debug=False, port=5000)
