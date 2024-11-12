#!/usr/bin/env python3
"""Main control flow for network client"""

from argparse import ArgumentParser
import os
from client import Client


def start_args():
    """@brief parses the arguements from the command line, needs IN_FOLDER
    and OUT_FOLDER
    @return return a namespace object of the four command line values:
    IN_FOLDER, OUT_FOLDER, IP, PORT"""
    parser = ArgumentParser()
    parser.add_argument("-s", "--IP", help="IP address", default="localhost")
    parser.add_argument("-p", "--PORT", help="Port to connect to", default=31337, type=int)
    parser.add_argument("-i", "--IN_FOLDER", help="Location of input folder")
    parser.add_argument("-o", "--OUT_FOLDER", help="Location of output folder")
    parser.parse_args()
    args = parser.parse_args()
    if args.IN_FOLDER is None:
        args.IN_FOLDER = input("Please specify IN_FOLDER: ")
    if args.OUT_FOLDER is None:
        args.OUT_FOLDER = input("Please specify OUT_FOLDER: ")
    print(f"IP : {args.IP} Port : {args.PORT} Input folder :\
           {args.IN_FOLDER} Output folder : {args.OUT_FOLDER}\n")
    return args

def main():
    """Main function for net_client"""
    args = start_args()
    # Validate port range
    if args.PORT > 65535 or args.PORT < 1024:
        print("Invalid port\n")
        return -1
    client = Client(args.IP, args.PORT, (args.IN_FOLDER, args.OUT_FOLDER))
    if(os.path.isdir(args.IN_FOLDER) and os.path.isdir(args.OUT_FOLDER)):
        try:
            client.manage_files()
        except (ConnectionResetError, ConnectionRefusedError) as error:
            print("Connection ended by server\n", error)
    else:
        print("Failed to open  directory")
        return -3
    return 0

if __name__ == "__main__":
    main()
