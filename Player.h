#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

class Player
{
public:
	Player();
	void update();
	//add
	void rst();
	int HP;
	int coin;
private:
	int coin_freq;
	int coin_increase;
	int coin_counter;
};

#endif
