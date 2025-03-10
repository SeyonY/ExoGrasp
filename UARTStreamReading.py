import serial
import struct

# Serial port configuration
SERIAL_PORT = "COM4"  # Replace with your actual port (e.g., "/dev/ttyUSB0" on Linux, "COM4" on Windows)
BAUD_RATE = 921600
DATA_SIZE = 800  # Number of uint16_t values
BYTES_TO_READ = DATA_SIZE * 2  # 400 bytes per transmission

# Open serial port
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

def read_serial_data():
    print("Reading data from UART... (Press Ctrl+C to stop)")
    try:
        while True:
            # Read 400 bytes (200 uint16_t values)
            raw_data = ser.read(BYTES_TO_READ)

            # Ensure enough bytes were received before processing
            if len(raw_data) == BYTES_TO_READ:
                # Convert binary data to uint16_t values (Little-Endian '<' format)
                data_array = struct.unpack('<{}H'.format(DATA_SIZE), raw_data)
                
                # Print received data
                print("Received Data:", data_array)
    except KeyboardInterrupt:
        print("\nStopping UART reading.")
        ser.close()

# Start continuous data reading
read_serial_data()
