#include "Application.h"

int main(int argc, char** argv) {
	Florencia::Application* app = new Florencia::Application();
	app->Run();
	delete app;
	return 0;
}