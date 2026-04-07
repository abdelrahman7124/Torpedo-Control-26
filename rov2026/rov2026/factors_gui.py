#!/usr/bin/python3
import sys
import os
import signal
import rclpy
import json
from rclpy.node import Node
from std_msgs.msg import Float32MultiArray
from PySide6.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout,
    QLabel, QSlider, QPushButton, QApplication
)
from PySide6.QtCore import Qt, QTimer


THRUSTER_NAMES = [
    "Front Left",
    "Front Right",
    "Back Left",
    "Back Right",
    "Top Left",
    "Top Right"
]

FACTORS_DIR = os.path.join(os.path.expanduser("~"), "rov_factors")
FACTORS_FILE = os.path.join(FACTORS_DIR, "factors.json")


class FactorsWidget(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Thruster Factors")
        self.factors = [1.0] * 6
        self.sliders = []
        self.labels = []

        os.makedirs(FACTORS_DIR, exist_ok=True)
        print(f"Factors file path: {FACTORS_FILE}")

        self.load_factors()

        layout = QVBoxLayout()

        for i in range(6):
            h_layout = QHBoxLayout()

            label = QLabel(f"{THRUSTER_NAMES[i]}: {self.factors[i]:.2f}")
            label.setMinimumWidth(200)

            slider = QSlider(Qt.Horizontal)
            slider.setRange(0, 100)
            slider.setValue(int(self.factors[i] * 100))
            slider.valueChanged.connect(
                lambda value, idx=i, lbl=label: self.slider_changed(value, idx, lbl)
            )

            self.sliders.append(slider)
            self.labels.append(label)

            h_layout.addWidget(label)
            h_layout.addWidget(slider)
            layout.addLayout(h_layout)

        reset_btn = QPushButton("🔄 Reset All")
        reset_btn.setStyleSheet("padding: 8px 16px;")
        reset_btn.clicked.connect(self.reset_all)
        layout.addWidget(reset_btn)

        self.setLayout(layout)

    def slider_changed(self, value, idx, label):
        self.factors[idx] = round(value / 100.0, 2)
        label.setText(f"{THRUSTER_NAMES[idx]}: {self.factors[idx]:.2f}")

    def reset_all(self):
        self.factors = [1.0] * 6
        for i, slider in enumerate(self.sliders):
            slider.blockSignals(True)
            slider.setValue(100)
            self.labels[i].setText(f"{THRUSTER_NAMES[i]}: 1.00")
            slider.blockSignals(False)

    def load_factors(self):
        try:
            with open(FACTORS_FILE, "r") as f:
                loaded = json.load(f)
                if isinstance(loaded, list) and len(loaded) == 6:
                    self.factors = [round(float(v), 2) for v in loaded]
                    print(f"Loaded factors: {self.factors}")
                else:
                    raise ValueError("Invalid factors format")
        except (FileNotFoundError, json.JSONDecodeError, ValueError) as e:
            print(f"Could not load factors ({e}), using defaults")
            self.factors = [1.0] * 6

    def save_factors(self):
        try:
            tmp_file = FACTORS_FILE + ".tmp"
            with open(tmp_file, "w") as f:
                json.dump([round(v, 2) for v in self.factors], f, indent=4)
                f.flush()
                os.fsync(f.fileno())
            os.replace(tmp_file, FACTORS_FILE)
            print(f"Saved on exit: {self.factors}")
            return True
        except Exception as e:
            print(f"ERROR saving factors: {e}")
            return False


class FactorsNode(Node):
    def __init__(self):
        super().__init__('factors_gui')
        self.publisher = self.create_publisher(Float32MultiArray, 'factors', 10)
        self.get_logger().info("✅ Factors GUI node started")

    def publish_factors(self, factors):
        msg = Float32MultiArray()
        msg.data = [round(v, 2) for v in factors]
        self.publisher.publish(msg)


def main(args=None):
    rclpy.init(args=args)
    app = QApplication(sys.argv)

    signal.signal(signal.SIGINT, lambda *args: app.quit())

    widget = FactorsWidget()
    node = FactorsNode()

    timer = QTimer()
    timer.timeout.connect(lambda: (
        node.publish_factors(widget.factors),
        rclpy.spin_once(node, timeout_sec=0)
    ))
    timer.start(100)

    signal_timer = QTimer()
    signal_timer.timeout.connect(lambda: None)
    signal_timer.start(200)

    widget.show()

    def on_exit():
        timer.stop()
        signal_timer.stop()
        widget.save_factors()                # ← saves ONLY here on exit
        print("Factors saved on exit")
        node.destroy_node()
        rclpy.shutdown()

    app.aboutToQuit.connect(on_exit)
    sys.exit(app.exec())


if __name__ == '__main__':
    main()