#! usr/bin/env python3
"""establishes the class for the client for network calc"""
import socket
import os
import ipaddress
import pathlib
import struct
from constants import ClientConstants as cc

class Client():
    """Client class for network calc can be instantiated with or without in out folders"""

    def __init__(self, host, port, extra_args=None):
        self.host = host
        self.port = port
        self.addr = (host, port)
        self.extra_args = extra_args
        if extra_args is not None:
            self.in_folder = extra_args[0]
            self.out_folder = extra_args[1]
        else:
            self.in_folder = None
            self.out_folder = None
        if self.validate_ip() is False:
            print("Invalid host")

    def validate_ip(self):
        """@brief: Validates the ip address returns true if valid
            @return: True if valid, False otherwise"""
        if self.host == 'localhost':
            self.host = '127.0.0.1'
            return True
        try:
            ip_test = ipaddress.ip_address(self.host)
        except ValueError:
            print(f"Invalid IP address {ip_test}")
            return False
        return True

    def manage_files(self):
        """Utilizes the socket class to connect to host and send files"""
        if self.in_folder is None:
            return
        for filename in os.listdir(self.in_folder):
            if pathlib.Path(filename).suffix == '.equ':
                with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
                    sock.connect(self.addr)
                    try:
                        filepath = os.path.join(self.in_folder, filename)
                        if not os.path.isfile(filepath):
                            print(filepath + " is Not a file")
                            continue
                        header = self.create_header(filename)
                        with open(filepath, 'rb') as file_handle:
                            buffer = file_handle.read()
                        total_message = header+buffer
                        sock.sendall(total_message)
                        num_equations = self.get_num_equations(filename)
                        out_filepath = os.path.join(self.out_folder, filename)
                        self.write_into_dir(out_filepath, sock, num_equations)
                    except KeyboardInterrupt:
                        print('closing socket')

    def write_into_dir(self, out_filepath, sock, num_equations):
        """Checks the given filepath net header for validity and writes it to the given folder"""
        amount_received = 0
        data = sock.recv(cc.net_header_size)
        res = struct.unpack(cc.net_format, data)
        if res[1] == 0:
            print(f"Invalid header format for file {self.host}")
            return
        amount_expected = cc.file_header_size + (cc.equation_size*num_equations)
        try:
            with open(out_filepath, 'wb') as out_handle:
                while amount_received < amount_expected:
                    data = sock.recv(amount_expected)
                    if not data:
                        return
                    amount_received += len(data)
                    out_handle.write(data)
        except KeyboardInterrupt:
            print("Ctrl+C caught\n")

    def create_header(self, filename):
        """@brief Creates the network header for the given file
            @param filename: The path to the file
            @return: returns the header in a packed struct"""
        filename_length = len(filename)
        num_equations = self.get_num_equations(filename)
        total_length = filename_length + cc.equ_header_len +(cc.equation_length * num_equations)
        header = struct.pack(cc.net_format, cc.net_header_size,
                             filename_length, total_length, bytes(filename, 'ascii'))
        return header


    def get_num_equations(self, filename):
        """@brief: Returns the number of equations
            @param filename: The path to the file
            @return int number of equations"""
        filepath = os.path.join(self.in_folder, filename)
        with open(filepath, 'rb') as file_handle:
            chunk = file_handle.read(cc.necessary_bytes)
        num_eq = int.from_bytes(chunk[-8:], byteorder='little', signed=False)
        return num_eq
