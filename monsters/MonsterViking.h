#ifndef MONSTERVIKING_H_INCLUDED
#define MONSTERVIKING_H_INCLUDED

#include "Monster.h"

class MonsterViking : public Monster
{
public:
    MonsterViking(const std::vector<Point>& path)
        : Monster{ path, MonsterType::VIKING }
    {
        HP = 30;
        v = 50;
        money = 30;
        atk = 3;

        bitmap_switch_freq = 10;

        bitmap_img_ids.clear();
        for (int d = 0; d < 4; ++d) {
            bitmap_img_ids.emplace_back(std::vector<int>{0, 1, 2, 3, 4});
        }
    }

    void update() override;
    void draw() override;
    bool can_remove() const override;
};

#endif
