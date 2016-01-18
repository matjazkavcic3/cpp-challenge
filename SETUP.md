# cpp-challenge setup

In order to install the server on your [Ubuntu 14.04] machine you need to first generate the project using CMake, after that your able to compile it using the generated makefile.

Example:
mkdir build
cmake ..
make
./cpp-chalange

# cpp-challenge usage

The application was started without any arguments; it’ll automatically connect to the address (tcp://127.0.0.1:5000). It can also receive 1 argument with the address with the address it’ll use or use a wildcard for example (tcp://*:5556). Should the application receive an address that cannot be bound or should the binding fail it’ll report that and give an example of a correct argument. The application continues to listen to the network until it is terminated.

# cpp-challenge requirements

The application requires the libraries [OpenCV] and [ZMQ] to be installed on the machine in order to run.
