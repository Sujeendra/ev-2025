import cantools
import binascii

# Load the DBC file
dbc_file_path = 'C:\\Users\\sujee\\OneDrive\\Desktop\\VIG_Gen1_0_VEH_CAN_J1939.dbc'
db = cantools.database.load_file(dbc_file_path)

# Specify the message name to encode
message_name = 'S1_ES_PGN61427'
message = db.get_message_by_name(message_name)

# Define signal values to encode
signal_values = {
    'ShutDown_Clearance': 3,
    'Control_Es_Relay': 3,
    'Reserved_1_PGN61427': 1,
    'Service_Mode_Request': 3,
    'Reserved_2_PGN61427': 4294967295,
    'Terminal_Voltage_Actual': 2007.5,
    'Counter_PGN61427': 15,
    'CRC_PGN61427': 255
}

# Function to encode a single signal based on its properties
def encode_signal(signal_info, value):
    # Retrieve signal attributes
    start_bit = signal_info.start
    length = signal_info.length
    scale = signal_info.scale
    offset = signal_info.offset
    is_big_endian = (signal_info.byte_order == 'big_endian')
    
    # Scale and offset adjustment for physical value
    raw_value = int((value - offset) / scale)
    max_value = (1 << length) - 1
    raw_value = min(max_value, max(0, raw_value))
    
    # Shift to the correct bit position within the CAN frame
    if is_big_endian:
        bit_position = 64 - start_bit - length
    else:
        bit_position = start_bit
    return raw_value << bit_position

# Function to encode all signals of a message
def encode_message(message, signal_values):
    # Start with an empty frame (64 bits for an 8-byte message)
    frame = 0
    
    # Encode each signal into the frame
    for signal_name, value in signal_values.items():
        signal_info = message.get_signal_by_name(signal_name)
        encoded_signal = encode_signal(signal_info, value)
        frame |= encoded_signal  # Combine encoded signal bits into the frame
    
    # Convert the frame to bytes (assuming big-endian message format)
    frame_bytes = frame.to_bytes(message.length, byteorder='big')
    return frame_bytes

# Encode the message using the custom encoder
encoded_frame = encode_message(message, signal_values)

# Convert to hexadecimal representation and print
hex_frame = binascii.hexlify(encoded_frame).decode('utf-8').upper()
print(f"Encoded CAN frame for message '{message_name}': {hex_frame}")
