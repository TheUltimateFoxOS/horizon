#pragma once

#include <stdint.h>

namespace ubsan {
	struct source_location_t {
		const char* file;
		uint32_t line;
		uint32_t column;
	};

	struct type_descriptor_t {
		uint16_t kind;
		uint16_t info;
		char name[];

		inline bool is_int() {
			return kind == 0;
		}

		inline bool is_float() {
			return kind == 1;
		}

		inline bool is_singed() {
			return info & 1;
		}

		inline bool is_unsigned() {
			return !is_singed();
		}

		inline int bit_width() {
			return 1 << (info >> 1);
		}
	};

	struct invalid_value_data_t {
		source_location_t location;
		const type_descriptor_t* type;
	};

	struct non_null_arg_data_t {
		source_location_t location;
		source_location_t attribute_location;
		int argument_index;
	};

	struct non_null_return_data_t {
		source_location_t location;
	};

	struct overflow_data_t {
		source_location_t location;
		const type_descriptor_t* type_descriptor;
	};

	struct vla_bound_data_t {
		source_location_t location;
		const type_descriptor_t* type_descriptor;
	};

	struct shift_out_of_bounds_data_t {
		source_location_t location;
		const type_descriptor_t* left_operand;
		const type_descriptor_t* right_operand;
	};

	struct out_of_bounds_data_t {
		source_location_t location;
		const type_descriptor_t* array_type;
		const type_descriptor_t* index_type;
	};

	struct type_mismatch_data_t {
		source_location_t location;
		const type_descriptor_t* type;
		uint8_t log_alignment;
		uint8_t type_check_kind;
	};

	struct alignment_assumption_data_t {
		source_location_t location;
		source_location_t assumption_location;
		const type_descriptor_t* type;
	};

	struct unreachable_data_t {
		source_location_t location;
	};

	struct implicit_conversation_data_t {
		source_location_t location;
		const type_descriptor_t* from_type;
		const type_descriptor_t* to_type;
		uint8_t kind;
	};

	struct invalid_builtin_data_t {
		source_location_t location;
		uint8_t kind;
	};

	struct pointer_overflow_data_t {
		source_location_t location;
	};

	struct float_cat_overflow_data_t {
		source_location_t location;
		const type_descriptor_t* from_type;
		const type_descriptor_t* to_type;
	};
}

extern "C" {
	void __ubsan_handle_load_invalid_value(const ubsan::invalid_value_data_t& data, void*);
	void __ubsan_handle_nonnull_arg(const ubsan::non_null_arg_data_t& data);
	void __ubsan_handle_nullability_arg(const ubsan::non_null_arg_data_t& data);
	void __ubsan_handle_nonnull_return_v1(const ubsan::non_null_return_data_t&, const ubsan::source_location_t& where);
	void __ubsan_handle_nullability_return_v1(const ubsan::non_null_arg_data_t&, const ubsan::source_location_t where);
	void __ubsan_handle_vla_bound_not_positive(const ubsan::vla_bound_data_t& data, void*);
	void __ubsan_handle_add_overflow(const ubsan::overflow_data_t& data, void*, void*);
	void __ubsan_handle_sub_overflow(const ubsan::overflow_data_t& data, void*, void*);
	void __ubsan_handle_negate_overflow(const ubsan::overflow_data_t& data, void*);
	void __ubsan_handle_mul_overflow(const ubsan::overflow_data_t& data, void*, void*);
	void __ubsan_handle_shift_out_of_bounds(const ubsan::shift_out_of_bounds_data_t& data, void*, void*);
	void __ubsan_handle_divrem_overflow(const ubsan::overflow_data_t& data, void*, void*);
	void __ubsan_handle_out_of_bounds(const ubsan::out_of_bounds_data_t& data, void*);
	void __ubsan_handle_type_mismatch_v1(const ubsan::type_mismatch_data_t& data, void*);
	void __ubsan_handle_alignment_assumption(const ubsan::alignment_assumption_data_t& data, void*, void*, void*);
	void __ubsan_handle_builtin_unreachable(const ubsan::unreachable_data_t& data);
	void __ubsan_handle_missing_return(const ubsan::unreachable_data_t& data);
	void __ubsan_handle_implicit_conversion(const ubsan::implicit_conversation_data_t& data, void*, void*);
	void __ubsan_handle_invalid_builtin(const ubsan::invalid_builtin_data_t& data);
	void __ubsan_handle_pointer_overflow(const ubsan::pointer_overflow_data_t& data, void*, void*);
}