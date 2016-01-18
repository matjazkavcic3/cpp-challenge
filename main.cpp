#include <zmq.hpp>
#include <zmq.h>
#include <iostream>
#include <unistd.h>

#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace zmq;
using namespace cv;

const int OK_CODE = 200;
const int ERROR_CODE = 400;

String test = "";
String PONG = "pong";
String ERR_TOO_MUCH_DATA = "Too much data.";
String ERR_IMG = "Error reading the image.";
String ERR_MISSING_DATA = "Missing data.";
String ERR_CMD = "Unknown command.";
String ERR_CODE = "Recieved data is not a code.";
String ERR_PROCESSING = "Error processing request.";

int64 more = 0;
size_t moreSize = 0;

void sentReply(int replayCode, String replyData, socket_t& socket){
	message_t reply1(4);
	message_t reply2(replyData.size());

	memcpy ((void *) reply1.data(), &replayCode, 4);
	memcpy ((void *) reply2.data(), replyData.c_str(), sizeof(replyData.size()));
	socket.send(reply1, ZMQ_SNDMORE);
	socket.send(reply2);
}

void sentReply(int replayCode, int replyData, socket_t& socket){
	message_t reply1(4);
	message_t reply2(4);

	memcpy ((void *) reply1.data(), &replayCode, 4);
	memcpy ((void *) reply2.data(), &replyData, 4);
	socket.send(reply1, ZMQ_SNDMORE);
	socket.send(reply2);
}

void flushSocket(socket_t& socket){
	moreSize = sizeof(more);
	socket.getsockopt(ZMQ_RCVMORE, &more, &moreSize);
	
	while(more){
		message_t faultyData;
		socket.recv (&faultyData);

		moreSize = sizeof(more);
		socket.getsockopt(ZMQ_RCVMORE, &more, &moreSize);
	}
}

int main(int argc, char* argv[]){
	context_t context(1);
	socket_t socket (context, ZMQ_REP);
	if(argc <= 1)
		socket.bind ("tcp://127.0.0.1:5000");
	else{
		try
		{
			socket.bind (argv[1]);
		}
		catch (Exception e)
		{
			std::cout << "ERROR CONNECTING TO THE PROVIDED ADDRESS" << std::endl << std::endl;
			std::cout << "Usage example: tcp://127.0.0.1:5000" << std::endl;
			return 1;
		}
	}

	unsigned int requestCode = 0;
	
	char sendAnwser = 0;

	while(true){
		sendAnwser = 0;
		message_t code;

		socket.recv (&code);

		if(code.size() == 4){
			memcpy(&requestCode, (void*)code.data(), code.size());

			moreSize = sizeof(more);
			socket.getsockopt(ZMQ_RCVMORE, &more, &moreSize);

			if(requestCode == 700){
				if(!more){
					sentReply(OK_CODE, test, socket);
					sendAnwser = 1;
				}
				else{
					flushSocket(socket);
					sentReply(ERROR_CODE, ERR_TOO_MUCH_DATA, socket);
					sendAnwser = 1;
				}
			}
			else if(requestCode == 701){
				if(!more){
					sentReply(OK_CODE, PONG, socket);
					sendAnwser = 1;
				}
				else{
					flushSocket(socket);
					sentReply(ERROR_CODE, ERR_TOO_MUCH_DATA, socket);
					sendAnwser = 1;
				}
			}
			else if(requestCode == 702){
				if(more){
					message_t img;
					socket.recv (&img);
				
					moreSize = sizeof(more);
					socket.getsockopt(ZMQ_RCVMORE, &more, &moreSize);

					if(more){
						flushSocket(socket);
						sentReply(ERROR_CODE, ERR_TOO_MUCH_DATA, socket);
						sendAnwser = 1;
					}
					else{
						cv::Mat tempMat(1, img.size(), CV_8UC1, img.data());
						cv::Mat imgMat = cv::imdecode(tempMat, CV_LOAD_IMAGE_GRAYSCALE);

						if(!imgMat.empty()){
							int anwser = (int)cv::mean(imgMat).val[0];
							sentReply(OK_CODE, anwser, socket);
							sendAnwser = 1;
						}
						else{
							sentReply(ERROR_CODE, ERR_IMG, socket);
							sendAnwser = 1;
						}
					}
				}
				else{
					sentReply(ERROR_CODE, ERR_MISSING_DATA, socket);
					sendAnwser = 1;
				}
			}
			else{
				flushSocket(socket);
				sentReply(ERROR_CODE, ERR_CMD, socket);
				sendAnwser = 1;
			}
		}
		else{
			flushSocket(socket);
			sentReply(ERROR_CODE, ERR_CODE, socket);
			sendAnwser = 1;
		}

		moreSize = sizeof(more);
		socket.getsockopt(ZMQ_RCVMORE, &more, &moreSize);

		flushSocket(socket);

		if(!sendAnwser){
			sentReply(ERROR_CODE, ERR_PROCESSING, socket);
		}
	}
	
	return 0;
}
