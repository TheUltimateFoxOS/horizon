#include <acpi/acpi.h>

#include <utils/string.h>
#include <utils/log.h>

using namespace acpi;

void* acpi::find_table_xsdt(sdt_header_t* sdt_header, char* signature, int idx) {
	int entries = (sdt_header->length - sizeof(sdt_header_t)) / 8;

	for(int t = 0; t < entries; t++) {
		sdt_header_t* new_header = (sdt_header_t*) * (uint64_t*) ((uint64_t) sdt_header + sizeof(sdt_header_t) + (t * 8));
		if (memcmp(new_header->signature, signature, 4) == 0) {
			if (idx == 0) {
				return new_header;
			}
			idx--;
		}
	}

	return 0;
}

void* acpi::find_table_rsdt(sdt_header_t* sdt_header, char* signature, int idx) {
	int entries = (sdt_header->length - sizeof(sdt_header_t)) / 4;

	for(int t = 0; t < entries; t++) {
		sdt_header_t* new_header = (sdt_header_t*) (uint64_t) * (uint32_t*) ((uint64_t) sdt_header + sizeof(sdt_header_t) + (t * 4));
		if (memcmp(new_header->signature, signature, 4) == 0) {
			if (idx == 0) {
				return new_header;
			}
			idx--;
		}
	}

	return 0;
}

void* acpi::find_table(stivale2_struct* bootinfo, char* signature, int idx) {


	void* result = nullptr;

	stivale2_struct_tag_rsdp* rsdp_tag = stivale2_tag_find<stivale2_struct_tag_rsdp>(bootinfo, STIVALE2_STRUCT_TAG_RSDP_ID);

	rsdp2_t* rsdp = (rsdp2_t*) ((uint64_t) rsdp_tag->rsdp);

	if (rsdp->xsdt_address != 0) {
		sdt_header_t* xsdt = (sdt_header_t*) (((rsdp2_t*) rsdp_tag->rsdp)->xsdt_address);

		result = find_table_xsdt(xsdt, (char*) signature, idx);
	} else {
		sdt_header_t* rsdt = (sdt_header_t*) (uint64_t) (((rsdp2_t*) rsdp_tag->rsdp)->rsdt_address);
			
		result = find_table_rsdt(rsdt, (char*) signature, idx);
	}

	debugf("WARNING: search result for %s:%d is %x\n", signature, idx, result);

	return result;
}
