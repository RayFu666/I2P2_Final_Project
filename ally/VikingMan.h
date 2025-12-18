#ifndef VIKINGMAN_H_INCLUDED
#define VIKINGMAN_H_INCLUDED

#include "Ally.h"

class VikingMan : public Ally {
public:
    VikingMan(const Point& p, int lane_id);

    void update() override;
    void draw() override;
    bool can_remove() const override;

private:
    bool dying = false;
    int die_total = 25;
    int die_cnt = 0;
};

#endif
