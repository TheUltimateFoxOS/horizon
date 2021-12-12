#include <driver/nic.h>

#include <utils/log.h>

using namespace driver;

namespace driver {
	nic_driver_manager* global_nic_manager;
}

void driver::load_network_stack() {
	debugf("driver::load_network_stack not implemented\n");
}

nic_device::nic_device() {};
nic_device::~nic_device() {};

void nic_device::send(uint8_t* data, int32_t len) {
	debugf("driver::nic_device::send not implemented\n");
}

void nic_device::register_nic_data_manager(nic_data_manager* _nic_data_manager) {
	this->_nic_data_manager = _nic_data_manager;
	_nic_data_manager->nic = this;
}

uint64_t nic_device::get_mac() {
	debugf("driver::nic_device::get_mac not implemented\n");
	return 0;
}

uint32_t nic_device::get_ip() {
	debugf("driver::nic_device::get_ip not implemented\n");
	return 0;
}

void nic_device::set_ip(uint32_t ip) {
	debugf("driver::nic_device::set_ip not implemented\n");
}

nic_data_manager::nic_data_manager(int id) {
	this->nic_id = id;
	global_nic_manager->get_nic(id)->register_nic_data_manager(this);
	debugf("nic_data_manager created for nic id %d\n", id);
}

nic_data_manager::~nic_data_manager() {}

void nic_data_manager::send(uint8_t* data, int32_t len) {
	this->nic->send(data, len);
}

bool nic_data_manager::recv(uint8_t* data, int32_t len) {
	debugf("driver::nic_data_manager::recv not implemented\n");
	return false;
}

nic_driver_manager::nic_driver_manager() {
	this->num_nics = 0;
}

void nic_driver_manager::add_nic(nic_device* nic) {
	this->nics[this->num_nics] = nic;
	debugf("Adding new Nic at idx %d!\n", this->num_nics);
	this->num_nics++;
}

nic_device* nic_driver_manager::get_nic(int nic_id) {
	return this->nics[nic_id];
}