import serial

# Replace 'COM3' with the actual serial port your Arduino is connected to
ser = serial.Serial('COM15', 115200, timeout=1)

def read_distance_data():
    while True:
        data = ser.readline().decode().strip()
        if data:
            print(f"Distance: {data} cm")

if __name__ == "__main__":
    try:
        read_distance_data()
    except KeyboardInterrupt:
        print("Program terminated by user.")
    finally:
        ser.close()
