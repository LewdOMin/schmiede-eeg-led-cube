import socket

HOST = ''
PORT = 65535


def main():
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        s.bind((HOST, PORT))
        while True:
            data, addr = s.recvfrom(16384)
            print('Received data from: ', addr)


if __name__ == '__main__':
    main()

