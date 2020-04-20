import socket
import sys
import cartdriver as cart

HOST = '127.0.0.1'
PORT = 1234 + int(sys.argv[1])

def process_command(command):

   if command == 'stop':
       cart.stop()

   elif command == 'drive':
       cart.drive()

   elif command == 'turnleft':
       cart.turn_left()

   elif command == 'turnright':
       cart.turn_right()

   elif command == 'shutdown':
       cart.shutdown()

   else:
       print('Invalid command')




with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.send('stopped'.encode())
    while True:

        data = s.recv(1024)
        if not data:
            break
        #conn.sendall(data)

        print(data.decode())

        process_command(data.decode())

        if(data.decode() == 'drive'):

            while(cart.stopped()):
                 cart.check_intersection()

            cart.toggle_stop()
            s.send('stopped'.encode())


        elif(data.decode() == 'shutdown'):
            s.close()
            break
