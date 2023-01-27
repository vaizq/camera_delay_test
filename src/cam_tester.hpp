#pragma once
#include "timer.hpp"
#include <SDL_render.h>
#include <functional>
#include <system_error>
#include <thread>
#include <mutex>
#include <chrono>
#include <fmt/format.h>
#include <iostream>
#include <SDL2/SDL.h>


using namespace std::chrono_literals;


enum class Color
{
    Black,
    White
};


class TsColorRenderer
{
public:
    TsColorRenderer() = default;
    TsColorRenderer(const TsColorRenderer&) = delete;
    TsColorRenderer& operator=(const TsColorRenderer&) = delete;

    void run()
    {
        m_mtx.lock();
        {
            m_running = true;

            SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
            m_window = SDL_CreateWindow("Camera tester", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
            if (m_window == NULL)
            {
                throw std::runtime_error("Failed to create window\n");
            }

            // Setup SDL_Renderer instance
            m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
            if (m_renderer == NULL)
            {
                throw std::runtime_error("Error creating SDL_Renderer!");
            }
        }
        m_mtx.unlock();
        
        while (true)
        {
            m_mtx.lock();
            {
                if (!m_running)
                    break;

                // Hande events
                SDL_Event event;
                while (SDL_PollEvent(&event))
                {
                    if (event.type == SDL_QUIT)
                    {
                        m_running = false;
                    }
                    if (event.type == SDL_WINDOWEVENT && 
                            event.window.event == SDL_WINDOWEVENT_CLOSE && 
                            event.window.windowID == SDL_GetWindowID(m_window))
                    {
                        m_running = true;
                    }
                }

                // render
                SDL_RenderClear(m_renderer);
                SDL_RenderPresent(m_renderer);
            } 
            m_mtx.unlock();

            // sleep so other threads can use
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }


        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();

        std::cout << "Renderer quit\n";
    }

    ~TsColorRenderer()
    {
        std::cout << "~TsColorRenderer()\n";
    }

    void stop()
    {
        Guard g(m_mtx);
        m_running = false;
    }

    void setDrawColor(Color color)
    {
        Guard g(m_mtx);

        if (m_renderer == nullptr)
        {
            std::cerr << "Renderer is not running\n";
            return;
        }

        switch (color)
        {
        case Color::Black:
            SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
            break;
        case Color::White:
            SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
            break;

        };
    }

private:
    using Guard = std::lock_guard<std::mutex>;
    std::mutex m_mtx;
    SDL_Window* m_window{nullptr};
    SDL_Renderer* m_renderer{nullptr};
    bool m_running{false}; 
};


class CameraTester
{
public:
    CameraTester(TsColorRenderer& renderer)
        : m_renderer(renderer)
    {
    }

    template <typename Camera>
    steady_clock::duration testDelay(Camera& camera)
    {
        setColor(Color::White);

        // Wait until camera detects current color
        while (camera() != m_currentColor) 
        {}

        switchColor();
        const auto switchTime = steady_clock::now();

        // Wait until camera detects current color
        while (camera() != m_currentColor) 
        {}
        const auto detectTime = steady_clock::now();
        const auto delay = detectTime - switchTime;

        return delay;       
    }

    template <typename Camera>
    void testDetection(Camera& camera, const Color color)
    {
        setColor(color);
        std::this_thread::sleep_for(1s);

        auto detectedColor = camera();

        if (detectedColor != color)
        {
            auto colorStr = (color == Color::Black ? "black" : "white");
            throw std::runtime_error(fmt::format("Camera failed to detect {}\n", colorStr));
        }
    }

    Color currentColor() const
    {
        return m_currentColor;
    }

private:
    void setColor(Color color)
    {
        m_currentColor = color;
        m_renderer.setDrawColor(color);
    }

    void switchColor()
    {
        if (m_currentColor == Color::Black)
        {
            setColor(Color::White);
        }
        else
        {
            setColor(Color::Black);
        }
    }

private:
    Color m_currentColor;
    TsColorRenderer& m_renderer;
};







