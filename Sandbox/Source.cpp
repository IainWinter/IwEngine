#include <iostream>

#include "IwMath/vector2.h"

int main() {
	iwm::vector2 v = iwm::vector2(1, 2);
	iwm::vector2 v2 = iwm::vector2(1, 2);

	std::cout << v + v2;

	std::cin.get();
}