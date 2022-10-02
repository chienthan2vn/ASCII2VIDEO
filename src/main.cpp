#include <opencv2/opencv.hpp>
#include <iostream>
#include <text2pic.h>

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
	constexpr auto isWrite = false;
	std::u32string chars = U"M@WB08Za2SX7r;i:;.";

	String video = "assets/video.mp4";
	if (argc > 1)
	{
		video = argv[1];
	}
	VideoCapture cap(video);

	if (cap.isOpened() == false)
	{
		cout << "Cannot open the video file" << endl;
		cin.get();
		return -1;
	}

	double fps = cap.get(CAP_PROP_FPS);
	double fCount = cap.get(CAP_PROP_FRAME_COUNT);
	int width = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
	int height = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));

	String windowName = "Hello";

	ddk::LoadFont("assets/Monaco.ttf", chars);

	int lastDot = video.find_last_of('.');
	String newName = video.substr(0, lastDot) + to_string(time(nullptr)) + video.substr(lastDot);

	if (isWrite)
		std::cout << "write to " << newName << std::endl;

	VideoWriter writer(newName, VideoWriter::fourcc('m','p','4','v'), fps, Size(width, height));

	if (!isWrite)
		namedWindow(windowName, WINDOW_NORMAL);

	while (true)
	{
		Mat frame;
		bool ok = cap.read(frame);
		if (ok == false)
		{
			break;
		}

		Mat&& convertedFrame = ddk::GenFrame(frame);
		if (isWrite)
			writer.write(convertedFrame);
		else
		{
			imshow(windowName, convertedFrame);
			if (waitKey(30) == 27)
			{
				break;
			}
		}
	}

	if (isWrite)
		writer.release();

	ddk::Free();

	return 0;
}
