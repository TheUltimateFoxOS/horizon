#include <sanitizers/ubsan.h>

#include <utils/log.h>

#define log(...) debugf("[ubsan --- WARNING ---] " __VA_ARGS__)


extern "C" {
	void __ubsan_handle_load_invalid_value(const ubsan::invalid_value_data_t& data, void*) {
		log("load_invalid_value: %s:%d\n", data.location.file, data.location.line);
	}

	void __ubsan_handle_nonnull_arg(const ubsan::non_null_arg_data_t& data) {
		log("nonnull_arg: %s:%d\n", data.location.file, data.location.line);
	}

	void __ubsan_handle_nullability_arg(const ubsan::non_null_arg_data_t& data) {
		log("nullability_arg: %s:%d\n", data.location.file, data.location.line);
	}

	void __ubsan_handle_nonnull_return_v1(const ubsan::non_null_return_data_t&, const ubsan::source_location_t& where) {
		log("nonnull_return: %s:%d\n", where.file, where.line);
	}

	void __ubsan_handle_nullability_return_v1(const ubsan::non_null_arg_data_t&, const ubsan::source_location_t where) {
		log("nullability_return: %s:%d\n", where.file, where.line);
	}

	void __ubsan_handle_vla_bound_not_positive(const ubsan::vla_bound_data_t& data, void*) {
		log("vla_bound_not_positive: %s:%d\n", data.location.file, data.location.line);
	}

	void __ubsan_handle_add_overflow(const ubsan::overflow_data_t& data, void*, void*) {
		log("add_overflow: %s:%d\n", data.location.file, data.location.line);
	}

	void __ubsan_handle_sub_overflow(const ubsan::overflow_data_t& data, void*, void*) {
		log("sub_overflow: %s:%d\n", data.location.file, data.location.line);
	}

	void __ubsan_handle_negate_overflow(const ubsan::overflow_data_t& data, void*) {
		log("negate_overflow: %s:%d\n", data.location.file, data.location.line);
	}

	void __ubsan_handle_mul_overflow(const ubsan::overflow_data_t& data, void*, void*) {
		log("mul_overflow: %s:%d\n", data.location.file, data.location.line);
	}

	void __ubsan_handle_shift_out_of_bounds(const ubsan::shift_out_of_bounds_data_t& data, void*, void*) {
		log("shift_out_of_bounds: %s:%d\n", data.location.file, data.location.line);
	}
	
	void __ubsan_handle_divrem_overflow(const ubsan::overflow_data_t& data, void*, void*) {
		log("divrem_overflow: %s:%d\n", data.location.file, data.location.line);
	}

	void __ubsan_handle_out_of_bounds(const ubsan::out_of_bounds_data_t& data, void*) {
		log("out_of_bounds: %s:%d\n", data.location.file, data.location.line);
	}

	void __ubsan_handle_type_mismatch_v1(const ubsan::type_mismatch_data_t& data, void*) {
	#ifndef UBSAN_SUPRES_TYPE_MISSMATCH
		log("type_mismatch: %s:%d\n", data.location.file, data.location.line);
	#endif
	}
	
	void __ubsan_handle_alignment_assumption(const ubsan::alignment_assumption_data_t& data, void*, void*, void*) {
		log("alignment_assumption: %s:%d\n", data.location.file, data.location.line);
	}

	void __ubsan_handle_builtin_unreachable(const ubsan::unreachable_data_t& data) {
		log("builtin_unreachable: %s:%d\n", data.location.file, data.location.line);
	}

	void __ubsan_handle_missing_return(const ubsan::unreachable_data_t& data) {
		log("missing_return: %s:%d\n", data.location.file, data.location.line);
	}

	void __ubsan_handle_implicit_conversion(const ubsan::implicit_conversation_data_t& data, void*, void*) {
		log("implicit_conversion: %s:%d\n", data.location.file, data.location.line);
	}

	void __ubsan_handle_invalid_builtin(const ubsan::invalid_builtin_data_t& data) {
		log("invalid_builtin: %s:%d\n", data.location.file, data.location.line);
	}

	void __ubsan_handle_pointer_overflow(const ubsan::pointer_overflow_data_t& data, void*, void*) {
		log("pointer_overflow: %s:%d\n", data.location.file, data.location.line);
	}
}