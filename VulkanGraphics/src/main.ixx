export module main;
import Application;

export int main() {
	Florencia::Application* app = new Florencia::Application();
	app->Run();
	delete app;
	return 0;
}