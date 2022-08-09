import { _do_documentation_cpp } from "./doc_cpp.ts";
import { _do_documentation_asm } from "./doc_asm.ts";

export function split_c_signature(signature: string) {
	var return_type = signature.split("(", 1)[0];
	var function_name;
	var attributes;

	if (return_type.split(" ").length == 1) {
		function_name = return_type;
		return_type = " ";
	} else {
		var ret_array = return_type.split(" ");
		var len = ret_array.length;

		function_name = ret_array[len - 1].trim();

		ret_array.pop();
		len--;

		return_type = ret_array[len - 1];
		attributes = "";

		// loop over the reversed array
		var is_ret = true;
		for (var i = len - 2; i >= 0; i--) {
			var _allowed_return_types = ['short', 'long', 'signed', 'unsigned', 'const', 'restrict', 'restrict', 'inline', 'mutable'];
			if (_allowed_return_types.includes(ret_array[i]) && is_ret) {
				return_type = ret_array[i] + " " + return_type;
			} else {
				is_ret = false;
				attributes = ret_array[i] + " " + attributes;
			}
		}

		return_type = return_type.trim();
		attributes = attributes.trim();
	}

	var class_name = "";
	if (function_name.split("::", 1).length != 1) {
		class_name = function_name.split("::", 1)[0];
		function_name = function_name.split("::", 1)[1];
	}

	if (function_name.startsWith("*")) {
		function_name = function_name.substring(1);
		return_type = "*";
	}

	var params = signature.split("(")[1].split(")")[0];

	return {
		"return_type": return_type,
		"class_name": class_name,
		"function_name": function_name,
		"attributes": attributes,
		"params": params,
		"signature": signature
	};
}

export function get_documentation(file: string, extension: string) {
	var text = Deno.readTextFileSync(file);

	switch (extension) {
		case "cpp":
			return _do_documentation_cpp(text);
		case "asm":
			return _do_documentation_asm(text);
		default:
			throw new Error(`Unknown extension ${extension} for file ${file}`);
	}
}