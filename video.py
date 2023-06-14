import socket
import numpy as np
import cv2

IMAGE_WIDTH = 640
IMAGE_HEIGHT = 480
IMAGE_SIZE = IMAGE_WIDTH * IMAGE_HEIGHT * 2  # 2 bytes per pixel (YUYV format)

# Create a socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Configure the server address and port
server_address = ('', 12345)  # Leave the IP address blank to bind to all available network interfaces

try:
    # Bind the server socket to the specified address and port
    server_socket.bind(server_address)
    print("Server started.")

    # Listen for incoming connections
    server_socket.listen(1)
    print("Waiting for a client to connect...")

    # Accept a client connection
    client_socket, client_address = server_socket.accept()
    print("Client connected:", client_address)

    # Create a named window for displaying the video
    cv2.namedWindow('Video', cv2.WINDOW_NORMAL)

    while True:
        # Receive the video frame from the client
        frame_data = client_socket.recv(IMAGE_SIZE)

        # Check if the received data is empty (end of stream)
        if not frame_data:
            print("End of video stream.")
            break

        # Convert the received data to a numpy array
        frame = np.frombuffer(frame_data, dtype=np.uint8)

        # Check if the received frame has the correct size
        if len(frame) != IMAGE_SIZE:
            print("Received frame size mismatch. Skipping frame...")
            print(f"Expected size: {IMAGE_SIZE} bytes, received size: {len(frame)} bytes.")
            continue

        # Reshape the frame to the correct dimensions
        frame = frame.reshape((IMAGE_HEIGHT, IMAGE_WIDTH, 2))

        # Convert the YUYV image to RGB format
        frame_rgb = cv2.cvtColor(frame, cv2.COLOR_YUV2RGB_YUYV)

        # Display the frame
        cv2.imshow('Video', frame_rgb)

        # Wait for the 'q' key to quit
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

except socket.error as e:
    print(f"Error: {str(e)}")

finally:
    # Close the client socket
    client_socket.close()

    # Close the server socket
    server_socket.close()

    # Destroy the window
    cv2.destroyAllWindows()
