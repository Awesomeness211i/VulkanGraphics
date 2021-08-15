#include "Application.h"

int main() {
	auto app = new Application();
	app->Run();
	delete app;
	return 0;
}