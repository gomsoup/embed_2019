import socket
import threading
import string
import random
from pyfcm import FCMNotification

host = ''
port = 4000
con = 0

def pushFCM(head, body, key):
    push_service = FCMNotification(api_key=key)
    result = push_service.notify_topic_subscribers(topic_name="com_example_doorlock", message_title=head, message_body=body)
    print(result)

def sendPassword():
    print('send')

def getInput():
    while(True):
        data = con.recv(4).decode()
        print('client: ', data)
        if data == 'PASS':
            fd = open('./pass')
            password = fd.read(4)
            if not password:
                con.send('1234'.encode('utf-8'))  #default passwd
            else:
                con.send(password.encode('utf-8')) #user password
            fd.close()

        if data == 'TEMP': # requset temp passwd
            open('./temp', 'w').close()
            fd = open('./temp', 'w')
            
            pool = string.digits
            temp = ""

            for i in range(4):
                temp += random.choice(pool);
            
            fd.write(temp)
            con.send(temp.encode('utf-8'))

            print("temp passwd : ", temp)
            fd.close()

        if data == 'URGT': # argent moment
            fd = open('/home/ubuntu/api_key')
            key = fd.readline().splitlines()
            pushFCM("URGENT!", "침입이 감지되었습니다!", key[0])

            print("Urgent alert pushed")
            fd.close()

        if data == 'UDAT':
            open('./pass', 'w').close()
            fd = open('./pass', 'w')
            password = con.recv(4).decode()
            
            print("update passwd ", password)
            fd.write(password)
            fd.close()

        if data == 'CLOS':
            break
    con.close()


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind((host, port))
s.listen(1)
print("doorlock server now listen..")

while True:
    con, addr = s.accept()
    print('Connected by :' , addr)
    threading._start_new_thread(getInput, ())

