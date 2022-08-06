import { split_c_signature } from "./doc.ts";

export function _do_documentation_cpp(text: string) {
	var functions = [];

	var regex = /(([\w \*:]+)(?!.*(=)).+)((?<=[\s:~])((?!.*(if|switch$|do$|for$|while|\[$|\]$)).+)\s*\(([\w\s,<>\[\].=&':/*+]*?)\)\s*(const)?\s*(?={))/g;
	
	let match: RegExpExecArray | null;
	while ((match = regex.exec(text))) {
		var signature = match[0];
		var _function = split_c_signature(signature);

		var maybe_discard_regex = eval(`/\\/{2} ?# ?${_function.function_name}-discard/g`);
		if (maybe_discard_regex.exec(text)) {
			// console.warn(`Discarding ${_function.function_name}`);
			continue;
		}

		functions.push(_function);
	}

	return functions;
}