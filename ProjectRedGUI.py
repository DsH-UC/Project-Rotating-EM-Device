import sys
import asyncio
import re
from PyQt5.QtWidgets import (
    QApplication,
    QWidget,
    QVBoxLayout,
    QPushButton,
    QTextEdit,
    QLabel,
    QComboBox,
    QHBoxLayout,
)
from PyQt5.QtCore import QThread, pyqtSignal
from bleak import BleakScanner, BleakClient


UART_SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
UART_RX_UUID = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"  # laptop writes to this
UART_TX_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"  # ESP32 notifies to this


class BLEWorker(QThread):
    log_signal = pyqtSignal(str)
    devices_signal = pyqtSignal(list)
    connected_signal = pyqtSignal(bool)
    telemetry_signal = pyqtSignal(dict)
    rotating_telemetry_signal = pyqtSignal(dict)

    def __init__(self):
        super().__init__()
        self.loop = None
        self.client = None
        self.devices = []
        self.target_address = None
        self.pending_commands = asyncio.Queue()
        self.running = True

    def run(self):
        self.loop = asyncio.new_event_loop()
        asyncio.set_event_loop(self.loop)
        self.loop.run_until_complete(self.main_loop())

    async def main_loop(self):
        while self.running:
            try:
                cmd = await self.pending_commands.get()

                if cmd["type"] == "scan":
                    await self.scan()
                elif cmd["type"] == "connect":
                    await self.connect(cmd["address"])
                elif cmd["type"] == "send":
                    await self.send_command(cmd["payload"])
                elif cmd["type"] == "disconnect":
                    await self.disconnect()

            except Exception as e:
                self.log_signal.emit(f"BLE error: {e}")

    def enqueue(self, cmd):
        if self.loop is not None:
            asyncio.run_coroutine_threadsafe(self.pending_commands.put(cmd), self.loop)

    async def scan(self):
        self.log_signal.emit("Scanning for BLE devices...")
        found = await BleakScanner.discover(timeout=5.0)

        self.devices = []
        for d in found:
            name = d.name or "Unknown"
            self.devices.append((name, d.address))

        self.devices_signal.emit(self.devices)
        self.log_signal.emit(f"Found {len(self.devices)} device(s).")

    async def connect(self, address):
        await self.disconnect()

        self.log_signal.emit(f"Connecting to {address}")
        self.client = BleakClient(address)
        await self.client.connect()

        if self.client.is_connected:
            self.connected_signal.emit(True)
            self.log_signal.emit("Connected.")
            
            try:
                await self.client.start_notify(UART_TX_UUID, self.notification_handler)
                self.log_signal.emit("Notifications enabled.")
            except Exception as e:
                self.log_signal.emit(f"Notify setup failed: {e}")
        else:
            self.connected_signal.emit(False)
            self.log_signal.emit("Connection failed.")

    async def disconnect(self):
        if self.client is not None:
            try:
                if self.client.is_connected:
                    await self.client.disconnect()
            except Exception:
                pass

        self.client = None
        self.connected_signal.emit(False)

    async def send_command(self, payload: bytes):
        if self.client is None or not self.client.is_connected:
            self.log_signal.emit("Not connected.")
            return

        await self.client.write_gatt_char(UART_RX_UUID, payload, response=False)
        self.log_signal.emit(f"Sent: {payload!r}")

    # def notification_handler(self, sender, data):
    #     try:
    #         text = data.decode(errors="replace")
    #     except Exception:
    #         text = str(data)

    #     for line in text.splitlines():
    #         line = line.strip()

    #         if line.startswith("TEL "):
    #             telemetry = self.parse_telemetry(line)
    #             if telemetry is not None:
    #                 self.telemetry_signal.emit(telemetry)
    #             else:
    #                 self.log_signal.emit(f"Bad telemetry: {line}")
    #         else:
    #             self.log_signal.emit(f"STM32/ESP32: {line}")   
    
    def notification_handler(self, sender, data):
        try:
            text = data.decode(errors="replace")
        except Exception:
            text = str(data)

        for line in text.splitlines():
            line = line.strip()

            if not line:
                continue

            if line.startswith("TEL "):
                telemetry = self.parse_telemetry(line)
                if telemetry is not None:
                    self.telemetry_signal.emit(telemetry)
                else:
                    self.log_signal.emit(f"Bad telemetry: {line}")

            elif line.startswith("RTEMP "):
                rotating = self.parse_rotating_telemetry(line)
                if rotating is not None:
                    self.rotating_telemetry_signal.emit(rotating)
                else:
                    self.log_signal.emit(f"Bad rotating telemetry: {line}")

                self.log_signal.emit(f"STM32/ESP32: {line}")

            else:
                self.log_signal.emit(f"STM32/ESP32: {line}")
    
    def parse_telemetry(self, line: str):
        pattern = (
            r"TEL\s+"
            r"temp=(?P<temp>-?\d+\.\d+)\s+"
            r"cap=(?P<cap>-?\d+\.\d+)\s+"
            r"fan=(?P<fan>[01])\s+"
            r"ready=(?P<ready>[01])\s+"
            r"aligned=(?P<aligned>[01])\s+"
            r"rails=(?P<rails>\d+)\s+"
            r"enc=(?P<enc>-?\d+\.\d+)\s+"
            r"active=(?P<active>\d+)\s+"
            r"vel=(?P<vel>-?\d+\.\d+)"
        )

        match = re.match(pattern, line)
        if not match:
            return None

        return {
            "temp": float(match.group("temp")),
            "cap": float(match.group("cap")),
            "fan": int(match.group("fan")),
            "ready": int(match.group("ready")),
            "aligned": int(match.group("aligned")),
            "rails": int(match.group("rails")),
            "enc": float(match.group("enc")),
            "active": int(match.group("active")),
            "vel": float(match.group("vel")),
        }

    def stop(self):
        self.running = False
        self.enqueue({"type": "disconnect"})

    def parse_rotating_telemetry(self, line: str):
        parts = line.split()

        if len(parts) < 8 or parts[0] != "RTEMP":
            return None

        data = {}

        for part in parts[1:]:
            if "=" not in part:
                continue

            key, value = part.split("=", 1)

            if key == "seq":
                data[key] = int(value)
            elif key.startswith("t"):
                try:
                    data[key] = float(value)
                except ValueError:
                    data[key] = float("nan")

        required = ["seq", "t0", "t1", "t2", "t3", "t4", "t5"]

        for key in required:
            if key not in data:
                return None

        return data

class BluetoothGUI(QWidget):
    def __init__(self):
        super().__init__()

        self.worker = BLEWorker()
        self.worker.log_signal.connect(self.log)
        self.worker.devices_signal.connect(self.update_devices)
        self.worker.connected_signal.connect(self.update_connected_state)
        self.worker.telemetry_signal.connect(self.update_telemetry)
        self.worker.rotating_telemetry_signal.connect(self.update_rotating_telemetry)
        self.worker.start()

        self.init_ui()

    def init_ui(self):
        self.setWindowTitle("STM32 Bluetooth Control GUI")

        layout = QVBoxLayout()

        self.status_label = QLabel("Disconnected")
        layout.addWidget(self.status_label)

        device_row = QHBoxLayout()
        self.device_box = QComboBox()
        self.scan_btn = QPushButton("Scan")
        self.connect_btn = QPushButton("Connect")

        self.scan_btn.clicked.connect(self.scan_devices)
        self.connect_btn.clicked.connect(self.connect_selected)

        device_row.addWidget(self.device_box)
        device_row.addWidget(self.scan_btn)
        device_row.addWidget(self.connect_btn)
        layout.addLayout(device_row)

        self.fan_btn = QPushButton("Fan Toggle")
        self.launch_btn = QPushButton("Launch")
        self.reset_btn = QPushButton("Reset / Discharge")
        self.status_btn = QPushButton("Request Status")

        self.fan_btn.clicked.connect(lambda: self.send_command(b"F\n"))
        self.launch_btn.clicked.connect(lambda: self.send_command(b"L\n"))
        self.reset_btn.clicked.connect(lambda: self.send_command(b"R\n"))
        self.status_btn.clicked.connect(lambda: self.send_command(b"S\n"))

        self.temp_label = QLabel("Rail Temp: -- °C")
        self.cap_label = QLabel("Cap Voltage: -- V")
        self.fan_label = QLabel("Fan: --")
        self.ready_label = QLabel("Payload Ready: --")
        self.aligned_label = QLabel("Motor Aligned: --")
        self.rails_label = QLabel("Used Rails: --")
        self.ir_velocity_label = QLabel("IR Velocity: -- m/s")

        self.rot_seq_label = QLabel("Rotating Seq: --")
        self.rot_t0_label = QLabel("Rotating T0: -- °C")
        self.rot_t1_label = QLabel("Rotating T1: -- °C")
        self.rot_t2_label = QLabel("Rotating T2: -- °C")
        self.rot_t3_label = QLabel("Rotating T3: -- °C")
        self.rot_t4_label = QLabel("Rotating T4: -- °C")
        self.rot_t5_label = QLabel("Rotating T5: -- °C")

        self.encoder_label = QLabel("Encoder Angle: -- °")
        self.active_rail_label = QLabel("Active Rail: --")

        #layout.addWidget(self.temp_label)
        #layout.addWidget(self.cap_label)
        layout.addWidget(self.fan_label)
        layout.addWidget(self.ready_label)
        layout.addWidget(self.aligned_label)
        layout.addWidget(self.rails_label)

        layout.addWidget(self.encoder_label)
        layout.addWidget(self.active_rail_label)
        layout.addWidget(self.ir_velocity_label)

        layout.addWidget(self.fan_btn)
        layout.addWidget(self.launch_btn)
        layout.addWidget(self.reset_btn)
        layout.addWidget(self.status_btn)

        layout.addWidget(self.rot_seq_label)
        layout.addWidget(self.rot_t0_label)
        layout.addWidget(self.rot_t1_label)
        layout.addWidget(self.rot_t2_label)
        layout.addWidget(self.rot_t3_label)
        layout.addWidget(self.rot_t4_label)
        layout.addWidget(self.rot_t5_label)

        self.show_area = QTextEdit()
        self.show_area.setReadOnly(True)
        layout.addWidget(self.show_area)

        self.setLayout(layout)
        self.update_connected_state(False)

    def scan_devices(self):
        self.worker.enqueue({"type": "scan"})

    def connect_selected(self):
        index = self.device_box.currentIndex()
        if index < 0:
            self.log("No device selected.")
            return

        address = self.device_box.itemData(index)
        if address is None:
            self.log("Invalid selected device.")
            return

        self.worker.enqueue({"type": "connect", "address": address})

    def send_command(self, payload: bytes):
        self.worker.enqueue({"type": "send", "payload": payload})

    def update_devices(self, devices):
        self.device_box.clear()

        for name, address in devices:
            label = f"{name} - {address}"
            self.device_box.addItem(label, address)

    def update_connected_state(self, connected):
        self.status_label.setText("Connected" if connected else "Disconnected")

        self.fan_btn.setEnabled(connected)
        self.launch_btn.setEnabled(connected)
        self.reset_btn.setEnabled(connected)
        self.status_btn.setEnabled(connected)
    
    def update_telemetry(self, tel: dict):
        self.temp_label.setText(f"Rail Temp: {tel['temp']:.2f} °C")
        self.cap_label.setText(f"Cap Voltage: {tel['cap']:.2f} V")
        self.fan_label.setText(f"Fan: {'ON' if tel['fan'] else 'OFF'}")
        self.ready_label.setText(f"Payload Ready: {'YES' if tel['ready'] else 'NO'}")
        self.aligned_label.setText(f"Motor Aligned: {'YES' if tel['aligned'] else 'NO'}")
        self.rails_label.setText(f"Used Rails: {tel['rails']}")
        self.encoder_label.setText(f"Encoder Angle: {tel['enc']:.2f} °")
        self.active_rail_label.setText(f"Active Rail: {tel['active']}")
        self.ir_velocity_label.setText(f"IR Velocity: {tel['vel']:.2f} m/s")
    
    def update_rotating_telemetry(self, tel: dict):
        self.rot_seq_label.setText(f"Rotating Seq: {tel['seq']}")
        self.rot_t0_label.setText(f"Rotating T0: {tel['t0']:.2f} °C")
        self.rot_t1_label.setText(f"Rotating T1: {tel['t1']:.2f} °C")
        self.rot_t2_label.setText(f"Rotating T2: {tel['t2']:.2f} °C")
        self.rot_t3_label.setText(f"Rotating T3: {tel['t3']:.2f} °C")
        self.rot_t4_label.setText(f"Rotating T4: {tel['t4']:.2f} °C")
        self.rot_t5_label.setText(f"Rotating T5: {tel['t5']:.2f} °C")

    def log(self, message):
        self.show_area.append(message)

    def closeEvent(self, event):
        self.worker.stop()
        self.worker.quit()
        self.worker.wait(1000)
        event.accept()


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = BluetoothGUI()
    window.resize(500, 400)
    window.show()
    sys.exit(app.exec_())