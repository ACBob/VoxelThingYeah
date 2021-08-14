// TODO:

#include <random>

int main (int argc, char* args[]) {
	const char *fortune[6] = {
		"Outlook hazy",
		"Ask again later",
		"Unclear",
		"Bad",
		"Good",
		"Try again"
	};
	int i = random() % 6;
	printf("Your fortune: %s\n", fortune[i]);

	return 0;
}