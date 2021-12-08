#include <driver/driver.h>

#include <renderer/font_renderer.h>

#include <utils/log.h>
#include <utils/string.h>

using namespace driver;

driver_manager* driver::global_driver_manager;

//#device_driver::device_driver-doc: Empty constructor.
device_driver::device_driver() {

}

//#device_driver::~Driver-doc: Empty destructor.
device_driver::~device_driver() {

}

//#device_driver::activate-doc: Virtual function to be overridden. Activate the driver.
void device_driver::activate() {

}

//#device_driver::is_presend-doc: Virtual function to be overridden. Get if is pre send.
bool device_driver::is_presend() {
	return true;
}

//#device_driver::get_name-doc: Virtual function to be overridden. Get the driver's name.
char* device_driver::get_name() {
	return (char*) "default";
}

//#driver_manager::driver_manager-doc: driver_manager constructor.
driver_manager::driver_manager() {
	this->num_drivers = 0;
}

//#driver_manager::add_driver-doc: Add a driver to the driver manager.
void driver_manager::add_driver(device_driver* driver) {
	this->drivers[this->num_drivers] = driver;
	this->num_drivers++;
}

//#driver_manager::set_status-doc: Print the device status onto the screen.
void driver_manager::set_status(char* status, uint64_t color) {
	uint64_t old_color = renderer::global_font_renderer->color;
	printf("[");
	renderer::global_font_renderer->color = color;
	printf("%s", status);
	renderer::global_font_renderer->color = old_color;
	printf("]\n");
}

//#driver_manager::activate_driver-doc: Activate a driver.
void driver_manager::activate_driver(bool force, device_driver* driver) {
	if(force) {
		driver->activate();
		this->set_status((char*) "force", 0xffaa00ff);
	} else {
		if(driver->is_presend()) {
			driver->activate();
			this->set_status((char*) "ok", 0xff00ff00);
		} else {
			this->set_status((char*) "no device", 0xff787878);
		}
	}
}

//#driver_manager::activate_all-doc: Activate all drivers.
void driver_manager::activate_all(bool force) {
	for(int i = 0; i < this->num_drivers; i++) {
		printf("Loading driver for device: %s... ", this->drivers[i]->get_name());
		activate_driver(force, this->drivers[i]);
	}
}

//#driver_manager::find_driver_by_name-doc: Find a driver by name.
device_driver* driver_manager::find_driver_by_name(char* name) {
	for (int i = 0; i < num_drivers; i++) {
		if(strcmp(drivers[i]->get_name(), name) == 0) {
			return drivers[i];
		}
	}
	return nullptr;
}