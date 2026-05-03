import sys
import os
import signal
import rclpy
import json
from rclpy.node import Node
from std_msgs.msg import String
from PySide6.QtWidgets import (
    QWidget, QVBoxLayout, QGridLayout,
    QLabel, QSlider, QPushButton, QApplication, QTabWidget,
    QGroupBox, QScrollArea
)
from PySide6.QtCore import Qt, QTimer


THRUSTER_NAMES = [
    "Front Right", "Front Left",
    "Back Right", "Back Left",
    "Top Front", "Top Back"
]

AXES = ['fb', 'rl', 'yaw', 'ud', 'pitch']
DIRECTIONS = ['fwd', 'bwd']

AXIS_LABELS = {
    'fb': '↕️ Forward/Back',
    'rl': '↔️ Right/Left',
    'yaw': '🔄 Yaw',
    'ud': '⬆️ Up/Down',
    'pitch': '📐 Pitch',
}

DIR_LABELS = {
    'fwd': '▶ Forward',
    'bwd': '◀ Backward',
}

SPEED_LEVELS = ['LOW', 'MEDIUM', 'HIGH']

THRUSTER_AXES = {
    0: ['fb', 'rl', 'yaw'],
    1: ['fb', 'rl', 'yaw'],
    2: ['fb', 'rl', 'yaw'],
    3: ['fb', 'rl', 'yaw'],
    4: ['ud', 'pitch'],
    5: ['ud', 'pitch'],
}

FACTORS_DIR = os.path.join(os.path.expanduser("~"), "rov_factors")
FACTORS_FILE = os.path.join(FACTORS_DIR, "factors.json")


def default_factors():
    return {
        level: {
            axis: {
                d: [1.0] * 6
                for d in DIRECTIONS
            }
            for axis in AXES
        }
        for level in SPEED_LEVELS
    }


class FactorsWidget(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Thruster Factors")
        self.factors = default_factors()
        self.sliders = {}
        self.labels = {}
        self.current_level = 'MEDIUM'

        os.makedirs(FACTORS_DIR, exist_ok=True)
        self.load_factors()

        main_layout = QVBoxLayout()

        self.level_label = QLabel(
            f"🔵 Current Speed Level: {self.current_level}"
        )
        self.level_label.setStyleSheet(
            "font-size: 16px; font-weight: bold; padding: 6px;"
        )
        main_layout.addWidget(self.level_label)

        self.tabs = QTabWidget()

        for level in SPEED_LEVELS:
            self.sliders[level] = {
                axis: {d: [None] * 6 for d in DIRECTIONS}
                for axis in AXES
            }
            self.labels[level] = {
                axis: {d: [None] * 6 for d in DIRECTIONS}
                for axis in AXES
            }

            tab = QWidget()
            tab_layout = QVBoxLayout()

            scroll = QScrollArea()
            scroll.setWidgetResizable(True)
            scroll_content = QWidget()
            scroll_layout = QVBoxLayout()

            for thruster_idx in range(6):
                relevant_axes = THRUSTER_AXES[thruster_idx]
                group = QGroupBox(THRUSTER_NAMES[thruster_idx])
                grid = QGridLayout()

                row = 0
                for axis in relevant_axes:
                    # ── Axis header (motion name) ──
                    header = QLabel(f"── {AXIS_LABELS[axis]} ──")
                    header.setStyleSheet(
                        "font-weight: bold; color: #555; padding-top: 4px;"
                    )
                    grid.addWidget(header, row, 0, 1, 2)
                    row += 1

                    for d in DIRECTIONS:
                        val = self.factors[level][axis][d][thruster_idx]

                        # ── Direction label (just Forward/Backward) ──
                        lbl = QLabel(f"    {DIR_LABELS[d]}: {val:.2f}")
                        lbl.setMinimumWidth(200)

                        slider = QSlider(Qt.Horizontal)
                        slider.setRange(0, 100)
                        slider.setValue(int(val * 100))
                        slider.valueChanged.connect(
                            self._make_slider_callback(
                                level, axis, d, thruster_idx, lbl
                            )
                        )

                        self.sliders[level][axis][d][thruster_idx] = slider
                        self.labels[level][axis][d][thruster_idx] = lbl

                        grid.addWidget(lbl, row, 0)
                        grid.addWidget(slider, row, 1)
                        row += 1

                group.setLayout(grid)
                scroll_layout.addWidget(group)

            scroll_content.setLayout(scroll_layout)
            scroll.setWidget(scroll_content)
            tab_layout.addWidget(scroll)

            reset_btn = QPushButton(f"🔄 Reset Level {level}")
            reset_btn.setStyleSheet("padding: 8px 16px;")
            reset_btn.clicked.connect(self._make_reset_callback(level))
            tab_layout.addWidget(reset_btn)

            tab.setLayout(tab_layout)
            self.tabs.addTab(tab, f"Speed: {level}")

        main_layout.addWidget(self.tabs)

        reset_all_btn = QPushButton("🔄 Reset ALL Levels")
        reset_all_btn.setStyleSheet(
            "padding: 10px 20px; font-weight: bold;"
        )
        reset_all_btn.clicked.connect(self.reset_all)
        main_layout.addWidget(reset_all_btn)

        self.setLayout(main_layout)

    def _make_slider_callback(self, level, axis, direction, thruster_idx, label):
        def callback(value):
            val = round(value / 100.0, 2)
            self.factors[level][axis][direction][thruster_idx] = val
            label.setText(f"    {DIR_LABELS[direction]}: {val:.2f}")
        return callback

    def _make_reset_callback(self, level):
        def callback():
            self.reset_level(level)
        return callback

    def reset_level(self, level):
        for thruster_idx in range(6):
            for axis in THRUSTER_AXES[thruster_idx]:
                for d in DIRECTIONS:
                    self.factors[level][axis][d][thruster_idx] = 1.0
                    slider = self.sliders[level][axis][d][thruster_idx]
                    lbl = self.labels[level][axis][d][thruster_idx]
                    if slider and lbl:
                        slider.blockSignals(True)
                        slider.setValue(100)
                        lbl.setText(f"    {DIR_LABELS[d]}: 1.00")
                        slider.blockSignals(False)

    def reset_all(self):
        for level in SPEED_LEVELS:
            self.reset_level(level)

    def set_current_level(self, level):
        self.current_level = level
        self.level_label.setText(
            f"🔵 Current Speed Level: {self.current_level}"
        )

    def load_factors(self):
        try:
            with open(FACTORS_FILE, "r") as f:
                loaded = json.load(f)
                for level in SPEED_LEVELS:
                    if level not in loaded:
                        raise ValueError(f"Missing level {level}")
                    for axis in AXES:
                        if axis not in loaded[level]:
                            raise ValueError(f"Missing {axis} in {level}")
                        for d in DIRECTIONS:
                            if d not in loaded[level][axis]:
                                raise ValueError(
                                    f"Missing {d} in {level}/{axis}"
                                )
                            if not (isinstance(loaded[level][axis][d], list)
                                    and len(loaded[level][axis][d]) == 6):
                                raise ValueError(
                                    f"Bad array {level}/{axis}/{d}"
                                )
                self.factors = {
                    lv: {
                        ax: {
                            d: [round(float(v), 2)
                                for v in loaded[lv][ax][d]]
                            for d in DIRECTIONS
                        }
                        for ax in AXES
                    }
                    for lv in SPEED_LEVELS
                }
                print(f"Loaded factors from {FACTORS_FILE}")
        except (FileNotFoundError, json.JSONDecodeError, ValueError) as e:
            print(f"Could not load factors ({e}), using defaults")
            self.factors = default_factors()

    def save_factors(self):
        try:
            tmp_file = FACTORS_FILE + ".tmp"
            with open(tmp_file, "w") as f:
                json.dump(self.factors, f, indent=4)
                f.flush()
                os.fsync(f.fileno())
            os.replace(tmp_file, FACTORS_FILE)
            print("Saved factors on exit")
            return True
        except Exception as e:
            print(f"ERROR saving factors: {e}")
            return False


class FactorsNode(Node):
    def __init__(self):
        super().__init__('factors_gui')
        self.publisher = self.create_publisher(String, 'factors', 10)
        self.create_subscription(
            String, 'speed_level', self.level_callback, 10
        )
        self.level_cb_external = None
        self.get_logger().info("✅ Factors GUI node started")

    def level_callback(self, msg):
        if self.level_cb_external:
            self.level_cb_external(msg.data)

    def publish_factors(self, factors):
        msg = String()
        msg.data = json.dumps(factors)
        self.publisher.publish(msg)


def main(args=None):
    rclpy.init(args=args)
    app = QApplication(sys.argv)
    signal.signal(signal.SIGINT, lambda *a: app.quit())

    widget = FactorsWidget()
    node = FactorsNode()
    node.level_cb_external = widget.set_current_level

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
        widget.save_factors()
        node.destroy_node()
        rclpy.shutdown()

    app.aboutToQuit.connect(on_exit)
    sys.exit(app.exec())


if __name__ == '__main__':
    main()