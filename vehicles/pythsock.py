import socket
import sys
import cartdriver as cart

HOST = '127.0.0.1'
PORT = 1234 + int(sys.argv[1])




def dir_from_str(dir_str):
   if dir_str == 'left':
      return 0
   elif dir_str == 'up':
      return 1
   elif dir_str == 'right':
      return 2
   elif dir_str == 'down':
      return 3




def process_command(command):

   if command == 'stop':
       cart.stop()

   elif command == 'drive':
       cart.drive()

   elif command == 'up' or command == 'down' or command == 'left' or command == 'right':
      cart.turn(dir_from_str(command))

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

        if data.decode() == 'drive':
            s.send('stopped'.encode())

           
        elif data.decode() == 'up' or data.decode() == 'down' or data.decode() == 'right' or data.decode() == 'left':
           while not cart.c_dir == dir_from_str(data.decode()):
              time.sleep(0.05)
           s.send('turned'.encode())

         
        elif data.decode() == 'shutdown':
            s.close()
            break
