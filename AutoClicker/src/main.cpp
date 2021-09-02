#define OLC_PGE_APPLICATION
#define OLC_PGEX_FUI
#include "headers/olcPixelGameEngine.h"
#include "headers/olcPGEX_FrostUI.h"
#include <iostream>

class AutoClicker : public olc::PixelGameEngine
{
public:
    AutoClicker()
    {
        sAppName = "Auto Clicker";
    }

    olc::FrostUI gui;

    bool is_active = false;
    int timeout_value = 1;
    bool use_left_click = false;
    bool use_right_click = false;
    bool keep_window_ontop = false;
    bool keep_window_ontop_old = false;

    int start_value = 1;

    long long start_tick = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    long long last_tick = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    long long current_tick = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    void auto_clicker()
    {
        if (current_tick - last_tick > timeout_value)
        {
            if (use_left_click)
                mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            if (use_right_click)
                mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
            last_tick = current_tick;
        }
    }

    void keep_ontop()
    {
        if (keep_window_ontop)
        {
            RECT window;
            if (GetWindowRect(olc::pge_handle, &window))
            {
                int width = window.right - window.left;
                int height = window.bottom - window.top;
                SetWindowPos(olc::pge_handle, HWND_TOPMOST, window.left, window.top, width, height, 0);
            }
        }
        else
        {
            RECT window;
            if (GetWindowRect(olc::pge_handle, &window))
            {
                int width = window.right - window.left;
                int height = window.bottom - window.top;
                SetWindowPos(olc::pge_handle, HWND_NOTOPMOST, window.left, window.top, width, height, 0);
            }
        }
    }
public:
    bool OnUserCreate() override
    {
        gui.add_groupbox("background", "", { 0, -1 }, { 220, 252 });

        auto title_size = GetTextSizeProp("Auto Clicker") * olc::vf2d{ 2.0f, 2.0f };
        gui.add_label("title", "Auto Clicker", { 220 / 2 - title_size.x / 2, 5 });
        gui.find_element("title")->scale_text({ 2.0f, 2.0f });

        gui.add_label("version", "Version 1.0 - By Frosty", { 220 / 2 - title_size.x / 2, 25 });

        auto status_size = GetTextSizeProp("Status: ");
        gui.add_label("status_title", "Status:", { 5, 240 });
        gui.add_label("status", "Inactive", { 5 + status_size.x, 240 });
        gui.find_element("status")->set_text_color(olc::RED);

        gui.add_label("panic_key", "Stop Key: ESC", { 130, 240 });

        gui.add_int_slider("start_time", "Start Delay (s): ", { 100, 40 }, { 100, 10 }, { 1, 10 }, &start_value);
        gui.add_int_slider("pause_time", "Timeout (ms): ", { 100, 60 }, { 100, 10 }, { 10, 1000 }, & timeout_value);

        gui.add_button("left_mb", "Left Click (activatable)", { 20, 80 }, { 180, 20 }, [] {});
        gui.add_button("right_mb", "Right Click (activatable)", { 20, 110 }, { 180, 20 }, [] {});
        gui.add_button("keep_top", "Keep Window ontop", { 20, 140 }, { 180, 20 }, [] {});
        gui.find_element("keep_top")->make_toggleable(&keep_window_ontop);
        gui.find_element("left_mb")->make_toggleable(&use_left_click);
        gui.find_element("right_mb")->make_toggleable(&use_right_click);

        gui.add_button("activate", "Start Clicker", { 220 / 2 - 45, 170 }, { 90, 20 }, [&] 
            {
                is_active = !is_active;

                if (is_active)
                {
                    gui.find_element("status")->set_text_color(olc::GREEN);
                    gui.find_element("status")->set_text("Active");
                    gui.find_element("activate")->set_text("Stop Clicker");
                }
                else
                {
                    gui.find_element("status")->set_text_color(olc::RED);
                    gui.find_element("status")->set_text("Inactive");
                    gui.find_element("activate")->set_text("Start Clicker");
                }

                if (is_active)
                    start_tick = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            });

        
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        Clear(olc::BLACK);

        // panic key
        if (GetKeyState(VK_ESCAPE) < 0)
        {
            gui.find_element("status")->set_text_color(olc::RED);
            gui.find_element("status")->set_text("Inactive");
            gui.find_element("activate")->set_text("Start Clicker");
            is_active = false;
        }

        if (GetKey(olc::ESCAPE).bPressed)
            return false;

        gui.run();

        if (keep_window_ontop != keep_window_ontop_old)
        {
            keep_ontop();
            keep_window_ontop_old = keep_window_ontop;
        }

        if (is_active && current_tick - start_tick > (start_value * 1000))
            auto_clicker();

        current_tick = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        return true;
    }
};

int main() 
{
    AutoClicker* auto_clicker = new AutoClicker;

    if (auto_clicker->Construct(220, 250, 1, 1, false))
        auto_clicker->Start();

    return 0;
}