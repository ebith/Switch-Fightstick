#!/usr/bin/env python3
import argparse
import serial
from time import sleep

parser = argparse.ArgumentParser()
parser.add_argument('--port', default='COM4')
args = parser.parse_args()
ser = serial.Serial(args.port, 9600)

def send(msg, duration=0):
    global ser
    try:
        ser.write(f'{msg}\r\n'.encode('utf-8'))
        print(msg.replace('Button ', '').replace('HAT ', '').replace('LY MIN', '△ ').replace('LY MAX', '▽ ').replace('LX MIN', '◁').replace('LX MAX', '▷'), end=' ', flush=True)
        sleep(duration)
        ser.write(b'RELEASE\r\n')
    except serial.serialutil.SerialException:
        while True:
            print("Reconnecting... ", end=' ', flush=True)
            try:
                sleep(0.4)
                ser = serial.Serial(args.port, 9600)
                print("Success.")
                sleep(0.1)
                send(msg, duration)
                break
            except:
                print("Faild. Retrying...")

try:
    while True:
        send('Button ZL', 0.1)
        sleep(2.50)
        send('Button A', 0.1)
        sleep(0.85)
        send('Button A', 0.1)
        sleep(0.85)
        send('Button B', 0.1)
        sleep(0.80)
        send('LY MIN', 0.1)
        sleep(0.80)
        send('Button ZL', 0.1)
        sleep(2.50)
        send('Button A', 0.1)
        sleep(0.80)
        send('Button ZL', 0.1)
        sleep(2.50)
        send('Button A', 0.1)
        sleep(0.80)
        send('LY MIN', 0.1)
        sleep(0.80)
        send('Button ZL', 0.1)
        sleep(2.50)
        send('Button A', 0.1)
        sleep(0.80)
        send('LY MIN', 0.1)
        sleep(0.80)
        send('Button ZL', 0.1)
        sleep(2.50)
        print(' ')
except KeyboardInterrupt:
    send('RELEASE')
    ser.close()
