import serial

PORT = "/tmp/sonar"

try:
    with serial.Serial(PORT, 115200, timeout=1) as connection:
        print(f"Listening on {PORT}...")

        while True:
            data = connection.read(4)
            if len(data) == 4:
                sample = int.from_bytes(data[0:2], byteorder="little")
                battery = data[2]
                temperature = data[3]

                print("Height : ", sample)
                print("Battery : ", battery)
                print("Temperature : ", temperature)

except serial.SerialException as error:
    print("Serial error:", error)

except KeyboardInterrupt:
    print("\nReceiver stopped.")
