import socket
import sys

HOST = '127.0.0.1'
PORT = 1234 + int(sys.argv[1])

def process_command(command):

   if command == 'stop':
       stop()

   elif command == 'drive':
       drive()

   elif command == 'turnleft':
       turn_left()

   elif command == 'turnright':
       turn_right()

   elif command == 'shutdown':
       shutdown()

   else:
       print('Invalid command')




def stop():
    print('stop')

def drive():
    print('drive')

def turn_left():
    print('left')

def turn_right():
    print('right')

def shutdown():
    print('shutdown')


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    while True:
        data = s.recv(1024)
        if not data:
            break
        #conn.sendall(data)

        print(data.decode())

        process_command(data.decode())
