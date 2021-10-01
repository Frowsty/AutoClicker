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
    bool pressed_hotkey = false;

    int start_value = 1;

    uint64_t start_tick = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    uint64_t last_tick = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    uint64_t current_tick = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

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

        gui.add_label("panic_key", "Hotkey: ESC", { 140, 240 });

        gui.add_int_slider("start_time", "Start Delay (s): ", { 100, 40 }, { 100, 10 }, { 1, 10 });
        gui.add_int_slider("pause_time", "Timeout (ms): ", { 100, 60 }, { 100, 10 }, { 10, 1000 });

        gui.add_button("left_mb", "Left Click (activatable)", { 20, 80 }, { 180, 20 }, [] {});
        gui.add_button("right_mb", "Right Click (activatable)", { 20, 110 }, { 180, 20 }, [] {});
        gui.add_button("keep_top", "Keep Window ontop", { 20, 140 }, { 180, 20 }, [] {});
        gui.find_element("keep_top")->make_toggleable(false);
        gui.find_element("left_mb")->make_toggleable(false);
        gui.find_element("right_mb")->make_toggleable(false);

        gui.add_button("activate", "Start Clicker", { 220 / 2 - 45, 170 }, { 90, 20 }, [&] 
            {
                is_active = !is_active;

                start_value = gui.find_element("start_time")->get_slider_value<int>();
                timeout_value = gui.find_element("pause_time")->get_slider_value<int>();

                use_left_click = gui.find_element("left_mb")->get_button_state();
                use_right_click = gui.find_element("right_mb")->get_button_state();

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

        // about window
        gui.create_window("about_window", "About", { 220 / 2 - 80, 250 / 2 - 30 }, { 160, 50 });
        gui.find_window("about_window")->close_window(true);
        gui.find_window("about_window")->set_top_border_thickness(10);

        gui.set_active_window("about_window");
        
        gui.add_label("info", "Auto clicker with\nposibility to pick delay\nbetween clicks\nGreat for cookie clicker", { 1, 1 });

        gui.clear_active_window();

        // Credits window
        gui.create_window("credits_window", "Credits", { 220 / 2 - 80, 250 / 2 - 30 }, { 160, 50 });
        gui.find_window("credits_window")->close_window(true);
        gui.find_window("credits_window")->set_top_border_thickness(10);

        gui.set_active_window("credits_window");

        gui.add_label("cred", "Made by: Frosty\nUsing: olcPGE/FrostUI\nVersion: 1.0", { 1, 1 });

        gui.clear_active_window();

        gui.add_button("about", "About", { 5, 215 }, { 100, 20 }, [&]
            {
                gui.find_window("about_window")->close_window(false);
                gui.find_window("about_window")->set_focused(true);
            });

        gui.add_button("credits", "Credits", { 115, 215 }, { 100, 20 }, [&]
            {
                gui.find_window("credits_window")->close_window(false);
                gui.find_window("credits_window")->set_focused(true);
            });
        
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        Clear(olc::BLACK);

        keep_window_ontop = gui.find_element("keep_top")->get_button_state();

        // panic key
        if (GetKeyState(VK_ESCAPE) < 0 && !pressed_hotkey)
        {
            if (is_active)
            {
                gui.find_element("status")->set_text_color(olc::RED);
                gui.find_element("status")->set_text("Inactive");
                gui.find_element("activate")->set_text("Start Clicker");
                is_active = false;
            }
            else
            {
                gui.find_element("status")->set_text_color(olc::GREEN);
                gui.find_element("status")->set_text("Active");
                gui.find_element("activate")->set_text("Stop Clicker");
                is_active = true;
            }
            pressed_hotkey = true;
        }

        if (!(GetKeyState(VK_ESCAPE) < 0) && pressed_hotkey)
            pressed_hotkey = false;

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

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    AutoClicker* auto_clicker = new AutoClicker;

    if (auto_clicker->Construct(220, 250, 1, 1, false))
        auto_clicker->Start();

    return 0;
}