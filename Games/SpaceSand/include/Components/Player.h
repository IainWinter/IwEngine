#pragma once

struct Player
{
	bool i_up; // inputs, should use event loop
	bool i_down;
	bool i_left;
	bool i_right;
	bool i_fire1;


	// replace with timer
	float fire1_timer = 0;
	float fire1_time  = .15;

};
