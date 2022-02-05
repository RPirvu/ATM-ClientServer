#!/usr/bin/env python3

import socket
import sys

class Admin:
    adminid = 1
    username = 'admin'
    password = 'admin'
      
def login(sd, arr):
    print("Login Menu\nEnter corresponding number to login\n")
    print("1 : Admin\n")
    print("2 : Normal User\n")
    
    type = int(input())
    status = 0

    if((type==1)):
    	sd.send("1".encode())
    	admin_login(sd, arr)
    if((type==2)):
    	sd.send(2)
    	user_operation(sd,type)
    
    if(status == 1):
    	print("Logged in successfuly")
    elif(status == 0):
    	print("ERROR") 
 
    arr[0] = type
    arr[1] = status


    
def admin_login(sd, arr):
    server_addr = ('127.0.0.1', 5555)
    x_admin = Admin()
    status = 0
    
    print ("AdminID : ")
    x_admin.adminid = input()
    print ("Password : ")
    x_admin.password = input()
    
    sd.sendto(x_admin.adminid.encode(),server_addr)
    sd.sendto(x_admin.password.encode(),server_addr)
    sd.recv(status)
    
def admin_operation(sd):
	print("ADMIN")
   
if __name__ == "__main__":
    server_addr = ('127.0.0.1', 5555)
    sd = socket.socket(socket.AF_INET, socket.SOCK_STREAM,0)
    sd.connect(server_addr)
    
    arr = [0,0]
    login(sd, arr)
    type = arr[0]
    status = arr[1]
    
    if((type==1) and (status==1)):
    	admin_operation(sd)
    if((type==2) and (status==1)):
       user_operation(sd)
