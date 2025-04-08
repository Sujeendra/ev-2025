import sys
import numpy as np
import time
import cantools
from PyQt5 import QtWidgets, QtGui, QtCore
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import threading
import can

# Lock variable for thread safety
lockvar = threading.Lock()

# Dictionary to store delta time for each CAN channel
delta_t = {}

# Dictionary to store the latest values of signals using message name and signal name as a key
signal_values = {}

# DBC Database
dbc_db = None

# Load the DBC file
def load_dbc_file(dbc_file_path):
    global dbc_db
    dbc_db = cantools.database.load_file(dbc_file_path)

# Load the DBC file (replace with your actual DBC file path)
dbc_file_path = "C:\\Users\\sujee\\OneDrive\\Desktop\\Vehicle_J1939.dbc"
load_dbc_file(dbc_file_path)

app = QtWidgets.QApplication(sys.argv)

# Initialize CAN interface using python-can library for PEAK USB
channel = None
is_channel_initialized = False

def initialize_channel():
    global channel, is_channel_initialized
    try:
        channel = can.interface.Bus(interface='pcan', channel='PCAN_USBBUS1', bitrate=500000)
        is_channel_initialized = True
        print("PEAK USB CAN channel initialized successfully.")
    except Exception as e:
        print(f"Failed to initialize PEAK USB CAN channel: {e}")

def decode_signal(data, offset, scale, start_bit, length, byte_order):
    binary_value = 0

    for i in range(length):
        if byte_order == "little_endian":
            bit_position = start_bit + i
        else:  # BigEndian
            bit_position = int(start_bit + (length/2) - 1 - i)
        byte_index = bit_position // 8
        bit_index = bit_position % 8

        if data[byte_index] & (1 << bit_index):
            binary_value |= (1 << i)

    decoded_value = (binary_value * scale) + offset
    return decoded_value

def on_can_event(msg):
    message_id = msg.arbitration_id
    data = msg.data

    try:
        message = dbc_db.get_message_by_frame_id(message_id)
    except KeyError:
        #print(f"No message found with CAN ID {message_id}")
        return

    for signal in message.signals:
        signal_value = decode_signal(data, signal.offset, signal.scale, signal.start, signal.length,signal.byte_order)
        key = f"{message.name}.{signal.name}"
        signal_values[key] = signal_value

class SignalViewer(QtWidgets.QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Real-Time CAN Signal Viewer")

        layout = QtWidgets.QVBoxLayout(self)
        
        self.search_input = QtWidgets.QLineEdit(self)
        self.search_button = QtWidgets.QPushButton("Search", self)
        self.search_button.clicked.connect(self.search_signal)
        
        self.tree_widget = QtWidgets.QTreeWidget(self)
        self.tree_widget.setHeaderLabel("CAN Signals")
        self.plot_widget = RealTimePlot(self)
        self.description_label = QtWidgets.QLabel(self)
        
        layout.addWidget(self.search_input)
        layout.addWidget(self.search_button)
        layout.addWidget(self.tree_widget)
        layout.addWidget(self.plot_widget)
        layout.addWidget(self.description_label)

        self.current_signal_function = None
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.plot_next_value)
        self.timer.start(100)

        self.tree_widget.itemClicked.connect(self.signal_selected)

    def add_signals_to_tree(self):
        self.tree_widget.clear()
        sorted_messages = sorted(dbc_db.messages, key=lambda msg: msg.name)

        for message in sorted_messages:
            message_item = QtWidgets.QTreeWidgetItem([message.name])
            sorted_signals = sorted(message.signals, key=lambda sig: sig.start)
            
            for signal in sorted_signals:
                signal_item = QtWidgets.QTreeWidgetItem([signal.name])
                message_item.addChild(signal_item)
                
            self.tree_widget.addTopLevelItem(message_item)

    def signal_selected(self, item):
        signal_name = item.text(0)
        for message in dbc_db.messages:
            for signal in message.signals:
                if signal.name == signal_name:
                    key = f"{message.name}.{signal.name}"
                    if key in signal_values:
                        self.current_signal_function = lambda t: signal_values.get(key, 0)
                        print(f"Selected signal: {signal_name} from message: {message.name}")
                        self.plot_widget.clear_plot()

                        self.current_signal_descriptions = signal.choices if signal.choices else {}
                        self.plot_widget.update_descriptions(self.current_signal_descriptions)
                        self.update_description_label(self.current_signal_descriptions)
                    return

    def update_description_label(self, descriptions):
        description_text = "\n".join([f"{v}: {desc}" for v, desc in descriptions.items()])
        self.description_label.setText(f"Signal Descriptions:\n{description_text}")

    def search_signal(self):
        search_term = self.search_input.text().lower()
        
        for i in range(self.tree_widget.topLevelItemCount()):
            self.tree_widget.topLevelItem(i).setHidden(True)
        
        for i in range(self.tree_widget.topLevelItemCount()):
            message_item = self.tree_widget.topLevelItem(i)
            for j in range(message_item.childCount()):
                signal_item = message_item.child(j)
                if search_term in signal_item.text(0).lower():
                    message_item.setHidden(False)
                    signal_item.setHidden(False)

    def plot_signal(self, signal_name, value):
        print(f"Updating plot for {signal_name}: {value}")
        self.plot_widget.update_plot(value)

    def plot_next_value(self):
        if self.current_signal_function:
            elapsed_time = time.time() - self.plot_widget.start_time
            next_value = self.current_signal_function(elapsed_time)
            self.plot_widget.update_plot(next_value)

class RealTimePlot(FigureCanvas):
    def __init__(self, parent=None):
        fig = Figure()
        super().__init__(fig)
        self.ax = fig.add_subplot(111)
        self.ax.set_title("Real-Time Signal Plot")
        self.ax.set_xlabel("Time")
        self.ax.set_ylabel("Signal Value")
        self.line, = self.ax.plot([], [], lw=2)
        self.time_data = []
        self.value_data = []
        self.start_time = time.time()
        self.signal_descriptions = {}

    def update_plot(self, value):
        current_time = time.time() - self.start_time
        self.time_data.append(current_time)
        self.value_data.append(value)
        self.time_data = self.time_data[-100:]
        self.value_data = self.value_data[-100:]
        self.line.set_data(self.time_data, self.value_data)
        self.ax.relim()
        self.ax.autoscale_view()
        self.draw()

    def clear_plot(self):
        self.time_data = []
        self.value_data = []
        self.line.set_data([], [])
        self.ax.relim()
        self.ax.autoscale_view()
        self.draw()

    def update_descriptions(self, descriptions):
        self.signal_descriptions = descriptions
        self.draw()

viewer = SignalViewer()
viewer.show()
viewer.add_signals_to_tree()

def main():
    initialize_channel()

    # Start a thread to read CAN messages and trigger events
    def read_can_messages():
        while is_channel_initialized:
            msg = channel.recv(timeout=1.0)
            if msg:
                on_can_event(msg)

    threading.Thread(target=read_can_messages, daemon=True).start()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()
