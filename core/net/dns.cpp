#include <net/dns.h>

#include <driver/nic.h>

#include <timer/timer.h>

#include <utils/log.h>
#include <utils/string.h>

using namespace net;

domain_name_service_provider::domain_name_service_provider(udp_socket* socket) : results(100) {
	this->socket = socket;
	this->loop = 0;
}

domain_name_service_provider::~domain_name_service_provider() {

}

void domain_name_service_provider::resolv_domain_to_hostname(char* dst_hostname, char* src_domain) {
	int len = strlen(src_domain) + 1;
	char* lbl = dst_hostname;
	char* dst_pos = dst_hostname + 1;
	uint8_t curr_len = 0;

	while (len-- > 0) {
		char c = *src_domain++;

		if (c == '.' || c == 0) {
			*lbl = curr_len;
			lbl = dst_pos++;
			curr_len = 0;
		} else {
			curr_len++;
			*dst_pos++ = c;
		}
	}
	*dst_pos = 0;
}

char* domain_name_service_provider::resolv_hostname_to_domain(uint8_t* reader, uint8_t* buffer, int *count) {
	unsigned int p = 0, i;
	unsigned int jumped = 0, offset;

	*count = 1;
	char* name = (char*) memory::malloc(256);

	name[0] = '\0';

	while(*reader != 0) {
		if(*reader >= 192)  {
			offset = (*reader) * 256 + *(reader+1) - 49152;
			reader = buffer + offset - 1;
			jumped = 1;
		} else {
			name[p++] = *reader;
		}
		reader = reader + 1;
		if(jumped == 0) {
			*count = *count + 1;
		}
	}

	name[p] = '\0';
	if(jumped == 1) {
		*count = *count + 1;
	}

	int len = strlen(name);
	for (i = 0; i < len; i++) {
		p = name[i];

		for (int j = 0; j < p; j++) {
			name[i] = name[i + 1];
			i++;
		}

		name[i] = '.';
	}

	name[i - 1] = '\0';
	return name;
}

void domain_name_service_provider::resolv_skip_name(uint8_t* reader, uint8_t* buffer, int *count) {
	unsigned int jumped = 0, offset;
	*count = 1;
	while(*reader != 0) {
		if(*reader >= 192)  {
			offset = (*reader) * 256 + *(reader+1) - 49152;
			reader = buffer + offset - 1;
			jumped = 1;
		}
		reader = reader + 1;
		if(jumped == 0) {
			*count = *count + 1;
		}
	}

	if(jumped == 1) {
		*count = *count + 1;
	}
}

void domain_name_service_provider::dns_request(char* name) {
	char* buffer = new char[2048];
	memset(buffer, 0, 2048);
	dnshdr_t* dns = (dnshdr_t*)buffer;
	char* qname = (char*)(buffer + sizeof(dnshdr_t));

	resolv_domain_to_hostname(qname, name);

	dns_question_t* question = (dns_question_t*)(buffer + sizeof(dnshdr_t) + strlen(qname) + 1);

	dns->id = 0xf00f;
	dns->opts = __builtin_bswap16(1 << 8);
	dns->qdcount = __builtin_bswap16(1);
	question->qtype = __builtin_bswap16(1);
	question->qclass = __builtin_bswap16(1);

	this->socket->send((uint8_t*) buffer, sizeof(dnshdr_t) + strlen(qname) + 1 + sizeof(dns_question_t));

	delete[] buffer;
}

uint32_t domain_name_service_provider::resolve_A(char* domain) {
	char qname[128];
	memset(qname, 0, 128);
	resolv_domain_to_hostname(qname, domain);

	//bool didExpire;
	list<dns_result_t>::node* n = results.find<char*>([](char* qname, list<dns_result_t>::node* n) {
		if (strcmp(qname, n->data.name) != 0) {
			return false;
		}

		return true;
	}, qname);

	if (n != nullptr) {
		//if (PIT::time_since_boot >= n->data.expire) {
		if (false) {
			results.remove(n);
		} else {
			if (n->data.ipv4 != 0) {
				this->loop = 0;
				return n->data.ipv4;
			} else if (n->data.cname != 0) {
				if (this->loop >= MAX_LOOP_COUNT) {
					debugf("DNS: CACHE: looping CNAME: \"%s\"\n", n->data.cname);
					results.remove(n);
					return 0;
				} else {
					debugf("DNS: CACHE: Resolving \"%s\", following CNAME: \"%s\"\n", domain, n->data.cname);
					return resolve_A(n->data.cname);
				}
			}
		}
	}
		
	wait_for_response = true;
	dns_request(domain);

	int timeout = 1000;
	while (wait_for_response) {
		if (timeout-- <= 0) {
			printf("DNS: Request timeout for: \"%s\"\n", domain);
			return 0;
		}

		timer::global_timer->sleep(10);
	}

	n = results.find<char*>([](char* qname, list<dns_result_t>::node* n) {
		return strcmp(qname, n->data.name) == 0;
	}, qname);

	if (n != nullptr) {
		if (n->data.ipv4 != 0) {
			this->loop = 0;
			return n->data.ipv4;
		} else if (n->data.cname != 0) {
			if (this->loop >= MAX_LOOP_COUNT) {
				debugf("DNS: looping CNAME: \"%s\"\n", n->data.cname);
				results.remove(n);
				return 0;
			} else {
				debugf("DNS: Resolving \"%s\", following CNAME: \"%s\"\n", domain, n->data.cname);
				return resolve_A(n->data.cname);
			}
		} else {
			results.remove(n);
		}
	} else {
		return 0;
	}
	return 0;
}

void domain_name_service_provider::on_udp_message(udp_socket *socket, uint8_t* data, size_t size) {
	dnshdr_t* dns = (dnshdr_t*)data;
	char* qname = (char*)(data + sizeof(dnshdr_t));
	dns_question_t* question = (dns_question_t*)(data + sizeof(dnshdr_t) + strlen(qname) + 1);
	char* name = (char*)(question + 1);

	int stop;
	uint16_t ancount;

	dns_result_t result;
	memset(&result, 0, sizeof(dns_result_t));
	strcpy(result.name, qname);
	bool did_get_result = false;

	if (dns->id != 0xf00f) {
		wait_for_response = false;
	}

	ancount = __builtin_bswap16(dns->ancount);
	while (ancount-- > 0) {
		dns_resource_t* resource = NULL;
		resolv_skip_name((uint8_t*) name, (uint8_t*) dns, &stop);
		name = name + stop;

		resource = (dns_resource_t*) name;
		name = name + sizeof(dns_resource_t);

		if (resource->type == __builtin_bswap16(1) && resource->_class == __builtin_bswap16(1)) {
			if (__builtin_bswap16(resource->data_len) == 4) {
				uint32_t* tmp_ip;
				uint8_t tmp_buff[4];

				for (int i = 0; i < 4; i++) {
					tmp_buff[i] = name[i];
				}

				tmp_ip = (uint32_t*) tmp_buff;

				driver::ip_u resolved_ip;
				resolved_ip.ip = *tmp_ip;

				debugf("DNS: Response \"%s\" -> %d.%d.%d.%d\n", qname, resolved_ip.ip_p[0], resolved_ip.ip_p[1], resolved_ip.ip_p[2], resolved_ip.ip_p[3]);

				result.ipv4 = resolved_ip.ip;
				//result.expire = PIT::time_since_boot + (resource->ttl * 1000);
				result.expire = 0 + (resource->ttl * 1000);
				did_get_result = true;
			}

			name = name + __builtin_bswap16(resource->data_len);
		} else if (resource->type == __builtin_bswap16(5) && resource->_class == __builtin_bswap16(1)) {
			char* out = resolv_hostname_to_domain((uint8_t*) name, (uint8_t*) dns, &stop);
			strcpy(result.cname, out);

			debugf("DNS: Response \"%s\" -> \"%s\"\n", qname, result.cname);

			//result.expire = PIT::time_since_boot + (resource->ttl * 1000);
			result.expire = 0 + (resource->ttl * 1000);
			did_get_result = true;

			name = name + stop;
		} else {
			resolv_skip_name((uint8_t*) name, (uint8_t*) dns, &stop);
			name = name + stop;
		}
	}

	if (did_get_result) {
		this->loop += 1;
		results.add(result);
	}

	wait_for_response = false;
	return;
}
