# #!/usr/bin/python3
# import sys
# import os
# import signal
# import rclpy
# import json
# from rclpy.node import Node
# from std_msgs.msg import Float32MultiArray
# from PySide6.QtWidgets import (
#     QWidget, QVBoxLayout, QHBoxLayout,
#     QLabel, QSlider, QPushButton, QApplication
# )
# from PySide6.QtCore import Qt, QTimer


# THRUSTER_NAMES = [
#     "Front Right",
#     "Front Left",
#     "Back Right",
#     "Back Left",
#     "Top Front",
#     "Top Back"
# ]

# FACTORS_DIR = os.path.join(os.path.expanduser("~"), "rov_factors")
# FACTORS_FILE = os.path.join(FACTORS_DIR, "factors.json")


# class FactorsWidget(QWidget):
#     def __init__(self):
#         super().__init__()
#         self.setWindowTitle("Thruster Factors")
#         self.factors = [1.0] * 6
#         self.sliders = []
#         self.labels = []

#         os.makedirs(FACTORS_DIR, exist_ok=True)
#         print(f"Factors file path: {FACTORS_FILE}")

#         self.load_factors()

#         layout = QVBoxLayout()

#         for i in range(6):
#             h_layout = QHBoxLayout()

#             label = QLabel(f"{THRUSTER_NAMES[i]}: {self.factors[i]:.2f}")
#             label.setMinimumWidth(200)

#             slider = QSlider(Qt.Horizontal)
#             slider.setRange(0, 100)
#             slider.setValue(int(self.factors[i] * 100))
#             slider.valueChanged.connect(
#                 lambda value, idx=i, lbl=label: self.slider_changed(value, idx, lbl)
#             )

#             self.sliders.append(slider)
#             self.labels.append(label)

#             h_layout.addWidget(label)
#             h_layout.addWidget(slider)
#             layout.addLayout(h_layout)

#         reset_btn = QPushButton("🔄 Reset All")
#         reset_btn.setStyleSheet("padding: 8px 16px;")
#         reset_btn.clicked.connect(self.reset_all)
#         layout.addWidget(reset_btn)

#         self.setLayout(layout)

#     def slider_changed(self, value, idx, label):
#         self.factors[idx] = round(value / 100.0, 2)
#         label.setText(f"{THRUSTER_NAMES[idx]}: {self.factors[idx]:.2f}")

#     def reset_all(self):
#         self.factors = [1.0] * 6
#         for i, slider in enumerate(self.sliders):
#             slider.blockSignals(True)
#             slider.setValue(100)
#             self.labels[i].setText(f"{THRUSTER_NAMES[i]}: 1.00")
#             slider.blockSignals(False)

#     def load_factors(self):
#         try:
#             with open(FACTORS_FILE, "r") as f:
#                 loaded = json.load(f)
#                 if isinstance(loaded, list) and len(loaded) == 6:
#                     self.factors = [round(float(v), 2) for v in loaded]
#                     print(f"Loaded factors: {self.factors}")
#                 else:
#                     raise ValueError("Invalid factors format")
#         except (FileNotFoundError, json.JSONDecodeError, ValueError) as e:
#             print(f"Could not load factors ({e}), using defaults")
#             self.factors = [1.0] * 6

#     def save_factors(self):
#         try:
#             tmp_file = FACTORS_FILE + ".tmp"
#             with open(tmp_file, "w") as f:
#                 json.dump([round(v, 2) for v in self.factors], f, indent=4)
#                 f.flush()
#                 os.fsync(f.fileno())
#             os.replace(tmp_file, FACTORS_FILE)
#             print(f"Saved on exit: {self.factors}")
#             return True
#         except Exception as e:
#             print(f"ERROR saving factors: {e}")
#             return False


# class FactorsNode(Node):
#     def __init__(self):
#         super().__init__('factors_gui')
#         self.publisher = self.create_publisher(Float32MultiArray, 'factors', 10)
#         self.get_logger().info("✅ Factors GUI node started")

#     def publish_factors(self, factors):
#         msg = Float32MultiArray()
#         msg.data = [round(v, 2) for v in factors]
#         self.publisher.publish(msg)


# def main(args=None):
#     rclpy.init(args=args)
#     app = QApplication(sys.argv)

#     signal.signal(signal.SIGINT, lambda *args: app.quit())

#     widget = FactorsWidget()
#     node = FactorsNode()

#     timer = QTimer()
#     timer.timeout.connect(lambda: (
#         node.publish_factors(widget.factors),
#         rclpy.spin_once(node, timeout_sec=0)
#     ))
#     timer.start(100)

#     signal_timer = QTimer()
#     signal_timer.timeout.connect(lambda: None)
#     signal_timer.start(200)

#     widget.show()

#     def on_exit():
#         timer.stop()
#         signal_timer.stop()
#         widget.save_factors()                # ← saves ONLY here on exit
#         print("Factors saved on exit")
#         node.destroy_node()
#         rclpy.shutdown()

#     app.aboutToQuit.connect(on_exit)
#     sys.exit(app.exec())


# if __name__ == '__main__':
#     main()



#!/usr/bin/python3
import sys
import os
import signal
import rclpy
import json
from rclpy.node import Node
from std_msgs.msg import String, Int32
from PySide6.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QGridLayout,
    QLabel, QSlider, QPushButton, QApplication, QTabWidget,
    QGroupBox
)
from PySide6.QtCore import Qt, QTimer


THRUSTER_NAMES = [
    "Front Right",
    "Front Left",
    "Back Right",
    "Back Left",
    "Top Front",
    "Top Back"
]

SPEED_LEVELS = ['LOW', 'MEDIUM', 'HIGH']

FACTORS_DIR = os.path.join(os.path.expanduser("~"), "rov_factors")
FACTORS_FILE = os.path.join(FACTORS_DIR, "factors.json")


def default_factors():
    """Return default factors structure: 3 levels x 2 directions x 6 thrusters."""
    return {
        str(level): {
            "forward":  [1.0] * 6,
            "backward": [1.0] * 6
        }
        for level in SPEED_LEVELS
    }


class FactorsWidget(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Thruster Factors")
        self.factors = default_factors()
        # sliders[level_str][direction][thruster_idx]
        self.sliders = {}
        self.labels = {}
        self.current_level = 'LOW'

        os.makedirs(FACTORS_DIR, exist_ok=True)
        print(f"Factors file path: {FACTORS_FILE}")

        self.load_factors()

        main_layout = QVBoxLayout()

        # ── Current speed level indicator ──
        self.level_label = QLabel(f"🔵 Current Speed Level: {self.current_level}")
        self.level_label.setStyleSheet(
            "font-size: 16px; font-weight: bold; padding: 6px;"
        )
        main_layout.addWidget(self.level_label)

        # ── Tabs for each speed level ──
        self.tabs = QTabWidget()

        for level in SPEED_LEVELS:
            level_str = str(level)
            self.sliders[level_str] = {"forward": [], "backward": []}
            self.labels[level_str] = {"forward": [], "backward": []}

            tab = QWidget()
            tab_layout = QVBoxLayout()

            for i in range(6):
                group = QGroupBox(THRUSTER_NAMES[i])
                grid = QGridLayout()

                for col, direction in enumerate(["forward", "backward"]):
                    val = self.factors[level_str][direction][i]
                    emoji = "⬆️" if direction == "forward" else "⬇️"

                    lbl = QLabel(
                        f"{emoji} {direction.capitalize()}: {val:.2f}"
                    )
                    lbl.setMinimumWidth(180)

                    slider = QSlider(Qt.Horizontal)
                    slider.setRange(0, 100)
                    slider.setValue(int(val * 100))
                    slider.valueChanged.connect(
                        self._make_slider_callback(
                            level_str, direction, i, lbl
                        )
                    )

                    self.sliders[level_str][direction].append(slider)
                    self.labels[level_str][direction].append(lbl)

                    row = col
                    grid.addWidget(lbl, row, 0)
                    grid.addWidget(slider, row, 1)

                group.setLayout(grid)
                tab_layout.addWidget(group)

            # Reset button per level
            reset_btn = QPushButton(f"🔄 Reset Level {level}")
            reset_btn.setStyleSheet("padding: 8px 16px;")
            reset_btn.clicked.connect(self._make_reset_callback(level_str))
            tab_layout.addWidget(reset_btn)

            tab.setLayout(tab_layout)
            self.tabs.addTab(tab, f"Speed Level {level}")

        main_layout.addWidget(self.tabs)

        # Global reset
        reset_all_btn = QPushButton("🔄 Reset ALL Levels")
        reset_all_btn.setStyleSheet(
            "padding: 10px 20px; font-weight: bold;"
        )
        reset_all_btn.clicked.connect(self.reset_all)
        main_layout.addWidget(reset_all_btn)

        self.setLayout(main_layout)

    # ── helpers to avoid closure issues ──

    def _make_slider_callback(self, level_str, direction, idx, label):
        def callback(value):
            self.slider_changed(value, level_str, direction, idx, label)
        return callback

    def _make_reset_callback(self, level_str):
        def callback():
            self.reset_level(level_str)
        return callback

    # ── slot handlers ──

    def slider_changed(self, value, level_str, direction, idx, label):
        val = round(value / 100.0, 2)
        self.factors[level_str][direction][idx] = val
        emoji = "⬆️" if direction == "forward" else "⬇️"
        label.setText(f"{emoji} {direction.capitalize()}: {val:.2f}")

    def reset_level(self, level_str):
        for direction in ["forward", "backward"]:
            self.factors[level_str][direction] = [1.0] * 6
            for i, slider in enumerate(self.sliders[level_str][direction]):
                slider.blockSignals(True)
                slider.setValue(100)
                emoji = "⬆️" if direction == "forward" else "⬇️"
                self.labels[level_str][direction][i].setText(
                    f"{emoji} {direction.capitalize()}: 1.00"
                )
                slider.blockSignals(False)

    def reset_all(self):
        for level in SPEED_LEVELS:
            self.reset_level(str(level))

    def set_current_level(self, level):
        self.current_level = level
        self.level_label.setText(f"🔵 Current Speed Level: {self.current_level}")

    # ── persistence ──

    def load_factors(self):
        try:
            with open(FACTORS_FILE, "r") as f:
                loaded = json.load(f)
                # validate structure
                for level in SPEED_LEVELS:
                    ls = str(level)
                    if ls not in loaded:
                        raise ValueError(f"Missing level {ls}")
                    for d in ["forward", "backward"]:
                        if d not in loaded[ls]:
                            raise ValueError(f"Missing {d} in level {ls}")
                        if not (isinstance(loaded[ls][d], list)
                                and len(loaded[ls][d]) == 6):
                            raise ValueError(f"Bad {d} array in level {ls}")
                self.factors = {
                    str(lv): {
                        d: [round(float(v), 2) for v in loaded[str(lv)][d]]
                        for d in ["forward", "backward"]
                    }
                    for lv in SPEED_LEVELS
                }
                print(f"Loaded factors: {json.dumps(self.factors, indent=2)}")
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
            print(f"Saved factors on exit")
            return True
        except Exception as e:
            print(f"ERROR saving factors: {e}")
            return False


class FactorsNode(Node):
    def __init__(self):
        super().__init__('factors_gui')
        self.publisher = self.create_publisher(String, 'factors', 10)

        # Subscribe to speed level so the GUI can display it
        self.create_subscription(String, 'speed_level', self.level_callback, 10)
        self.level_cb_external = None          # set from main()

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

    # wire speed‑level subscription → GUI
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
        print("Factors saved on exit")
        node.destroy_node()
        rclpy.shutdown()

    app.aboutToQuit.connect(on_exit)
    sys.exit(app.exec())


if __name__ == '__main__':
    main()