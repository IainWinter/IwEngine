#pragma once

extern void setup();
extern bool loop();

int main()
{
	setup();
	while (loop()) {}
	return 0;
}