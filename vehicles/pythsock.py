# IMPORTS
import socket
import sys
import cartdriver as cart

# Server information, port corresponds to cart index
HOST = '192.168.4.85'
PORT = 1234 + int(sys.argv[1])


# Determine numeric value from direction string
def dir_from_str(dir_str):
   if dir_str == 'left':
      return 0
   elif dir_str == 'up':
      return 1
   elif dir_str == 'right':
      return 2
   elif dir_str == 'down':
      return 3


# Determine what the server is telling us to do
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


# Main entry point for vehicles. Opens socket and begins state machine:
# 1 - RECEIVE: wait for command
# 2 - PERFORM and ACKNOWLEDGE: process command, inform server of completion,
#  return to RECEIVE state
if __name__ == '__main__':
   with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
      s.connect((HOST, PORT))
      while True:

         data = s.recv(1024)
         if not data:
            break

         data = data.decode()

         print(data)

         process_command(data)

         if data == 'drive':
            s.send('stopped'.encode())


         elif data == 'up' or data == 'down' or data == 'right' or data == 'left':
            s.send('turned'.encode())


         elif data == 'shutdown':
            cart.shutdown()
            s.close()
            break
