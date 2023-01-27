#include "cam_tester.hpp"
#include <cstdlib>
#include <ratio>
#include <thread>
#include "camera.hpp"
#include "timer.hpp"
#include <linux/videodev2.h>


class ColorDetector
{
public:
    ColorDetector()
        : cam("/dev/video4", V4L2_PIX_FMT_YUYV, 640, 480)
    {
        cam.start();
    }

    Color operator()()
    {
        while (!cam.hasFrame()) {}

        auto frame = cam.getFrame();

        double avg = 0.0;
        for (const auto pix : frame)
        {
            avg += pix;
        }
        avg /= frame.size();

        if (avg < 120.0)
        {
            return Color::Black;
        }
        else
        {
            return Color::White;
        }
    }

private:
    Camera cam;
};


int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: test-count\n";
        return EXIT_FAILURE;
    }

    const auto testCount = std::atoi(argv[1]);

    TsColorRenderer renderer;
   
    std::thread sdlThread([&renderer]()
            {
                renderer.run();
            });

    {
        ColorDetector detector;

        std::cout << "Testing starts in 1 seconds...\n";
        std::this_thread::sleep_for(1s);

        std::cout << "Testing starts\n";

        CameraTester tester(renderer);

        // Test camera
        tester.testDetection(detector, Color::Black);
        tester.testDetection(detector, Color::White);


        // Test average delay
        auto avgDelay = std::chrono::milliseconds(0);
        for (std::size_t i = 0; i < testCount; i++)
        {
            const auto delay = tester.testDelay(detector);
            avgDelay += std::chrono::duration_cast<std::chrono::milliseconds>(delay);
        }
        avgDelay /= testCount;

        std::cout << "Avg delay: " << avgDelay.count() << " ms\n";
    }


    renderer.stop();


    std::cout << "Join thread\n";
    sdlThread.join();
    std::cout << "Thread joined\n";


    return EXIT_SUCCESS;
}



























