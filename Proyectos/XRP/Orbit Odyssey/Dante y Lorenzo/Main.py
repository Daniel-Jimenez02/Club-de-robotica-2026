# XRP BLE joystick control — save as xrp_ble_control.py
# Requires XRP MicroPython with BLE and XRPLib installed.
import bluetooth
import struct
import time
from XRPLib.defaults import drivetrain
from XRPLib.defaults import Servo
servo = Servo.get_default_servo(1)
SERVICE = bluetooth.UUID('6E400001-B5A3-F393-E0A9-E50E24DCCA9E')
RX = bluetooth.UUID('6E400002-B5A3-F393-E0A9-E50E24DCCA9E')
TX = bluetooth.UUID('6E400003-B5A3-F393-E0A9-E50E24DCCA9E')
ble = bluetooth.BLE()
ble.active(True)
((tx_handle, rx_handle),) = ble.gatts_register_services((
    (SERVICE, ((TX, bluetooth.FLAG_READ | bluetooth.FLAG_NOTIFY),
               (RX, bluetooth.FLAG_WRITE | bluetooth.FLAG_WRITE_NO_RESPONSE))),
))
ble.gatts_set_buffer(rx_handle, 64)
last_command = time.ticks_ms()
connected = False

# BLE advertising packet: complete name + UART service UUID.
name = b'XRP-Control'
service_bytes = bytes(SERVICE)
advertisement = (bytes((2, 1, 6)) +
                 bytes((len(name) + 1, 9)) + name +
                 bytes((len(service_bytes) + 1, 7)) + service_bytes)

def advertise():
    ble.gap_advertise(100000, adv_data=advertisement)

def stop():
    drivetrain.set_effort(0, 0)

def on_ble(event, data):
    global last_command, connected
    if event == 1:  # central connected
        connected = True
        last_command = time.ticks_ms()
    elif event == 2:  # central disconnected
        connected = False
        stop()
        advertise()
    elif event == 3:  # characteristic written
        connection, handle = data
        if handle != rx_handle:
            return
        try:
            message = ble.gatts_read(rx_handle).decode().strip()
            # Servo command: "S:<angle>" where angle is 0-180 degrees.
            if message.startswith('S:'):
                angle = max(0, min(180, int(message[2:])))
                servo.set_angle(angle)
                last_command = time.ticks_ms()
                return
            # Two signed wheel percentages, e.g. 50,50 or -30,30
            values = message.split(',')
            if len(values) != 2:
                return
            left = max(-100, min(100, int(values[0])))
            right = max(-100, min(100, int(values[1])))
            drivetrain.set_effort(left / 100, right / 100)
            last_command = time.ticks_ms()
        except Exception as e:
            print("Error BLE:", e)
            stop()

ble.irq(on_ble)
stop()
advertise()
print('XRP-Control listo para conectar')
try:
    while True:
        # Stop within 500 ms if commands stop arriving, even if BLE stays connected.
        if time.ticks_diff(time.ticks_ms(), last_command) > 500:
            stop()
        time.sleep_ms(50)
finally:
    stop()
    ble.active(False)
