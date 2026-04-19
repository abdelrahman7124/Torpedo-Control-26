# pid_tuner_gui.py
import sys
import os
import signal
import json
import rclpy
from rclpy.node import Node
from std_msgs.msg import String
from PySide6.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QGridLayout,
    QLabel, QSlider, QPushButton, QApplication, QGroupBox,
    QDoubleSpinBox
)
from PySide6.QtCore import Qt, QTimer
from PySide6.QtGui import QFont


TUNING_DIR = os.path.join(os.path.expanduser("~"), "rov_factors")
TUNING_FILE = os.path.join(TUNING_DIR, "pid_gains.json")

GAIN_CONFIG = {
    'yaw_kp':    {'min': 0.0, 'max': 0.2,  'step': 0.001, 'default': 0.015},
    'yaw_ki':    {'min': 0.0, 'max': 0.05, 'step': 0.001, 'default': 0.002},
    'yaw_kd':    {'min': 0.0, 'max': 1.0,  'step': 0.01,  'default': 0.45},
    'depth_kp':  {'min': 0.0, 'max': 2.0,  'step': 0.01,  'default': 0.5},
    'depth_ki':  {'min': 0.0, 'max': 0.1,  'step': 0.001, 'default': 0.005},
    'depth_kd':  {'min': 0.0, 'max': 1.0,  'step': 0.01,  'default': 0.3},
    'pitch_kp':  {'min': 0.0, 'max': 0.2,  'step': 0.001, 'default': 0.04},
    'pitch_ki':  {'min': 0.0, 'max': 0.05, 'step': 0.001, 'default': 0.002},
    'pitch_kd':  {'min': 0.0, 'max': 1.0,  'step': 0.01,  'default': 0.12},
}

AXIS_ORDER = ['yaw', 'depth', 'pitch']


class PIDTunerWidget(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("🎛️ PID Tuner")
        self.setMinimumWidth(550)

        self.gains = {}
        self.spinboxes = {}
        self.sliders = {}

        os.makedirs(TUNING_DIR, exist_ok=True)
        self.load_gains()

        main_layout = QVBoxLayout()

        # Title
        title = QLabel("ROV PID Tuner")
        title.setFont(QFont("Arial", 16, QFont.Bold))
        title.setAlignment(Qt.AlignCenter)
        main_layout.addWidget(title)

        # Axis groups
        colors = {'yaw': '#e74c3c', 'depth': '#3498db', 'pitch': '#2ecc71'}
        for axis in AXIS_ORDER:
            group = self._create_axis_group(axis, colors[axis])
            main_layout.addWidget(group)

        # Buttons
        btn_layout = QHBoxLayout()

        reset_btn = QPushButton("🔄 Reset Defaults")
        reset_btn.setStyleSheet("padding: 8px; font-size: 13px;")
        reset_btn.clicked.connect(self.reset_all)
        btn_layout.addWidget(reset_btn)

        save_btn = QPushButton("💾 Save")
        save_btn.setStyleSheet(
            "padding: 8px; font-size: 13px; "
            "background-color: #27ae60; color: white;"
        )
        save_btn.clicked.connect(self.save_gains)
        btn_layout.addWidget(save_btn)

        load_btn = QPushButton("📂 Load")
        load_btn.setStyleSheet("padding: 8px; font-size: 13px;")
        load_btn.clicked.connect(self.load_and_apply)
        btn_layout.addWidget(load_btn)

        main_layout.addLayout(btn_layout)

        # Status
        self.status_label = QLabel("Move sliders to tune — changes apply instantly")
        self.status_label.setAlignment(Qt.AlignCenter)
        self.status_label.setStyleSheet("color: gray; padding: 5px;")
        main_layout.addWidget(self.status_label)

        self.setLayout(main_layout)

    def _create_axis_group(self, axis, color):
        group = QGroupBox(f"  {axis.upper()} PID")
        group.setStyleSheet(f"""
            QGroupBox {{
                font-size: 14px; font-weight: bold;
                border: 2px solid {color};
                border-radius: 8px;
                margin-top: 10px; padding-top: 15px;
            }}
            QGroupBox::title {{
                color: {color};
                subcontrol-position: top left;
                padding: 0 5px;
            }}
        """)

        grid = QGridLayout()
        grid.setColumnStretch(1, 1)

        for row, gain_type in enumerate(['kp', 'ki', 'kd']):
            key = f'{axis}_{gain_type}'
            config = GAIN_CONFIG[key]
            current_val = self.gains.get(key, config['default'])

            label = QLabel(f"{gain_type.upper()}:")
            label.setFont(QFont("Courier", 12, QFont.Bold))
            grid.addWidget(label, row, 0)

            slider = QSlider(Qt.Horizontal)
            steps = int((config['max'] - config['min']) / config['step'])
            slider.setRange(0, steps)
            slider.setValue(int((current_val - config['min']) / config['step']))
            grid.addWidget(slider, row, 1)

            spinbox = QDoubleSpinBox()
            spinbox.setRange(config['min'], config['max'])
            spinbox.setSingleStep(config['step'])
            spinbox.setDecimals(4)
            spinbox.setValue(current_val)
            spinbox.setMinimumWidth(90)
            grid.addWidget(spinbox, row, 2)

            slider.valueChanged.connect(
                lambda val, k=key, sb=spinbox: self._slider_to_spinbox(val, k, sb)
            )
            spinbox.valueChanged.connect(
                lambda val, k=key, sl=slider: self._spinbox_to_slider(val, k, sl)
            )

            self.sliders[key] = slider
            self.spinboxes[key] = spinbox
            self.gains[key] = current_val

        group.setLayout(grid)
        return group

    def _slider_to_spinbox(self, slider_val, key, spinbox):
        config = GAIN_CONFIG[key]
        value = round(config['min'] + slider_val * config['step'], 4)
        self.gains[key] = value
        spinbox.blockSignals(True)
        spinbox.setValue(value)
        spinbox.blockSignals(False)

    def _spinbox_to_slider(self, value, key, slider):
        config = GAIN_CONFIG[key]
        self.gains[key] = round(value, 4)
        slider_val = int((value - config['min']) / config['step'])
        slider.blockSignals(True)
        slider.setValue(slider_val)
        slider.blockSignals(False)

    def reset_all(self):
        for key, config in GAIN_CONFIG.items():
            self._set_gain(key, config['default'])
        self.status_label.setText("🔄 Reset to defaults")

    def _set_gain(self, key, value):
        config = GAIN_CONFIG[key]
        self.gains[key] = round(value, 4)
        self.spinboxes[key].blockSignals(True)
        self.spinboxes[key].setValue(value)
        self.spinboxes[key].blockSignals(False)
        slider_val = int((value - config['min']) / config['step'])
        self.sliders[key].blockSignals(True)
        self.sliders[key].setValue(slider_val)
        self.sliders[key].blockSignals(False)

    def load_gains(self):
        try:
            with open(TUNING_FILE, 'r') as f:
                loaded = json.load(f)
                for key in GAIN_CONFIG:
                    self.gains[key] = float(loaded.get(key, GAIN_CONFIG[key]['default']))
            print(f"Loaded gains from {TUNING_FILE}")
        except (FileNotFoundError, json.JSONDecodeError, ValueError):
            print("No saved gains, using defaults")
            for key in GAIN_CONFIG:
                self.gains[key] = GAIN_CONFIG[key]['default']

    def load_and_apply(self):
        self.load_gains()
        for key, value in self.gains.items():
            self._set_gain(key, value)
        self.status_label.setText(f"📂 Loaded from {TUNING_FILE}")

    def save_gains(self):
        try:
            with open(TUNING_FILE, 'w') as f:
                json.dump({k: round(v, 4) for k, v in self.gains.items()}, f, indent=4)
            self.status_label.setText(f"💾 Saved to {TUNING_FILE}")
        except Exception as e:
            self.status_label.setText(f"❌ Save failed: {e}")


class PIDTunerNode(Node):
    def __init__(self):
        super().__init__('pid_tuner')

        # Publish gains to controller
        self.gains_pub = self.create_publisher(String, 'pid_gains', 10)

        # Listen to current gains from controller (for sync on startup)
        self.create_subscription(String, 'pid_gains_current', self.current_gains_cb, 10)

        self.synced = False
        self.controller_gains = {}

        self.get_logger().info("🎛️ PID Tuner node started")

    def current_gains_cb(self, msg):
        """Read current gains from controller on startup."""
        try:
            self.controller_gains = json.loads(msg.data)
        except json.JSONDecodeError:
            pass

    def publish_gains(self, gains):
        msg = String()
        msg.data = json.dumps({k: round(v, 4) for k, v in gains.items()})
        self.gains_pub.publish(msg)

    def get_controller_gains(self):
        return self.controller_gains


def main(args=None):
    rclpy.init(args=args)
    app = QApplication(sys.argv)

    signal.signal(signal.SIGINT, lambda *a: app.quit())

    widget = PIDTunerWidget()
    node = PIDTunerNode()

    # Sync GUI with controller on startup
    def try_sync():
        rclpy.spin_once(node, timeout_sec=0)
        controller_gains = node.get_controller_gains()
        if controller_gains and not node.synced:
            for key, value in controller_gains.items():
                if key in GAIN_CONFIG:
                    widget._set_gain(key, value)
            node.synced = True
            widget.status_label.setText("✅ Synced with controller")

    # Publish gains at 10Hz
    timer = QTimer()
    timer.timeout.connect(lambda: (
        try_sync(),
        node.publish_gains(widget.gains),
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
        widget.save_gains()
        print("Gains saved on exit")
        node.destroy_node()
        rclpy.shutdown()

    app.aboutToQuit.connect(on_exit)
    sys.exit(app.exec())


if __name__ == '__main__':
    main()