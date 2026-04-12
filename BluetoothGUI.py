import sys
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QPushButton, QTextEdit
from PyQt5.QtBluetooth import QBluetoothDeviceDiscoveryAgent, QBluetoothDeviceInfo

class BluetoothGUI(QWidget):
    def __init__(self):
        super().__init__()
        self.init_ui()
        self.agent = QBluetoothDeviceDiscoveryAgent()
        self.agent.deviceDiscovered.connect(self.display_packet)

    def init_ui(self):
        self.setWindowTitle("Bluetooth GUI")
        layout = QVBoxLayout()
        self.show_area = QTextEdit()
        self.rd_btn = QPushButton("Start Reading")
        self.rd_btn.clicked.connect(self.start_scan)
        layout.addWidget(self.show_area)
        layout.addWidget(self.rd_btn)
        self.setLayout(layout)

    def start_scan(self):
        self.agent.start()

    def display_packet(self, info: QBluetoothDeviceInfo):
        self.show_area.append(f"{info.name()} - {info.address().toString()}") # Display device metadata

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = BluetoothGUI()
    window.show()
    sys.exit(app.exec_())
