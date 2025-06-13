#include "snake.hpp"

#include <deque>

bool snake(IDashGfxWrapper &gfx,
           const std::vector<PushButton> &buttons,
           PageState state)
{
    static bool game_over = false;

    constexpr int grid_sz = 16;
    const int grid_w = gfx.width() / grid_sz - 1;
    const int grid_h = gfx.height() / grid_sz - 1;

    static unsigned long last_time = 0;
    static std::deque<std::pair<int, int>> snake;
    static int dir_x = 1;
    static int dir_y = 0;

    static int food = 0;
    static int food_x = -1;
    static int food_y = -1;

    if (state == PageState::on_switch)
    {
        last_time = 0;
        game_over = false;
        randomSeed(last_time);
        snake.clear();
        snake.push_front({grid_w / 2, grid_h / 2});

        food = 0;
        food_x = random(5, grid_w - 4);
        food_y = random(5, grid_h - 4);
        dir_x = 1;
        dir_y = 0;
    }
    else if (game_over == false)
    {
        if (buttons[0].state() == PushButtonState::down)
        {
            dir_x = 0;
            dir_y = -1;
        }
        else if (buttons[1].state() == PushButtonState::down)
        {
            dir_x = 0;
            dir_y = 1;
        }
        else if (buttons[4].state() == PushButtonState::down)
        {
            dir_x = -1;
            dir_y = 0;
        }
        else if (buttons[5].state() == PushButtonState::down)
        {
            dir_x = 1;
            dir_y = 0;
        }

        gfx.fillRect(food_x * grid_sz, food_y * grid_sz, grid_sz, grid_sz, RA8875_GREEN);

        int x = snake.front().first;
        int y = snake.front().second;

        if (millis() - last_time > 150)
        {
            x += dir_x;
            y += dir_y;

            if (x < 0 || x > grid_w || y < 0 || y > grid_h)
            {
                game_over = true;
                return true;
            }
            for (const auto &pos : snake)
            {
                if (x == pos.first && y == pos.second)
                {
                    game_over = true;
                    return true;
                }
            }

            if (x == food_x && y == food_y)
            {
                food++;
                food_x = random(5, grid_w - 4);
                food_y = random(5, grid_h - 4);
            }

            snake.push_front({x, y});
            gfx.fillRect(x * grid_sz, y * grid_sz, grid_sz, grid_sz, RA8875_WHITE);

            x = snake.back().first;
            y = snake.back().second;
            if (food <= 0)
            {
                snake.pop_back();
            }
            else
            {
                food--;
            }
            gfx.fillRect(x * grid_sz, y * grid_sz, grid_sz, grid_sz, RA8875_BLACK);

            last_time = millis();
        }
    }
    else
    {
        constexpr char *msg = "GAME OVER";
        const int msg_w = gfx.textWidth(msg, 3);
        gfx.textWrite((gfx.width() - msg_w) / 2, (gfx.height() - gfx.chHeight(3)) / 2, 3, RA8875_WHITE, RA8875_BLACK, msg);
    }

    return true;
}
