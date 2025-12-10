#ifndef MONSTER_H_INCLUDED
#define MONSTER_H_INCLUDED

#include "../Object.h"
#include "../shapes/Rectangle.h"
#include "../shapes/Point.h"
#include <vector>
#include <queue>

enum class Dir;

// fixed settings
enum class MonsterType {
    WOLF, CAVEMAN, WOLFKNIGHT, DEMONNIJIA, MONSTERTYPE_MAX
};

enum class MonsterState {
    WALK,
    ATTACK,
    DIE
};

class Ally;

/**
 * @brief The class of a monster (enemies).
 * @details Monster inherits Object and takes Rectangle as its hit box.
 */
class Monster : public Object
{
public:
    static Monster* create_monster(MonsterType type, const std::vector<Point>& path);
public:
    Monster(const std::vector<Point>& path, MonsterType type);
    void update();
    void draw();
    const int& get_money() const { return money; }
    int HP;
    const std::queue<Point>& get_path() const { return path; }
    bool is_dead() const;
    void take_damage(int dmg);
protected:
    /**
     * @var HP
     * @brief Health point of a monster.
     **
     * @var v
     * @brief Moving speed of a monster.
     **
     * @var money
     * @brief The amount of money that player will earn when the monster is killed.
     **
     * @var bitmap_img_ids
     * @brief The first vector is the Dir index, and the second vector is image id.
     * @details `bitmap_img_ids[Dir][<ordered_id>]`
     **
     * @var bitmap_switch_counter
     * @brief Counting down for bitmap_switch_freq.
     * @see Monster::bitmap_switch_freq
     **
     * @var bitmap_switch_freq
     * @brief Number of frames required to change to the next move pose for the current facing direction.
     * @details The variable is left for child classes to define.
     *
     * @var bitmap_img_id
     * @brief Move pose of the current facing direction.
     **
     * @var dir
     * @brief Current facing direction.
     **
     * @var path
     * @brief The walk path of a monster, represented in grid format.
     * @see Level::grid_to_region(const Point &grid) const
    */
    int v;
    int money;
    std::vector<std::vector<int>> bitmap_img_ids;
    int bitmap_switch_counter;
    int bitmap_switch_freq;
    int bitmap_img_id;
private:
    MonsterState state = MonsterState::WALK;

    Ally* target_ally = nullptr;
    int atk = 2;               // 怪物攻擊力（先隨便一個值）
    int attack_cooldown = 0;
    int attack_freq = 30;      // 每 30 frame 攻擊一次
    double attack_range = 40;  // 近戰距離
    double lane_tolerance = 25;

    MonsterType type;
    Dir dir;
    std::queue<Point> path;
    void update_walk_state();
    void update_attack_state();
    void update_die_state();
};

#endif
