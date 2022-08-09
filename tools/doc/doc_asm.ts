import { split_c_signature } from "./doc.ts";

export function _do_documentation_asm(text: string) {
	var functions = [];

	var regex = /^([\w.]+: *)(?![\w ]+)/gm;

	let match: RegExpExecArray | null;
	while (match = regex.exec(text)) {
		if (match[1].charAt(0) == '.') {
			continue;
		} else {
			match[1] = match[1].trim();
			match[1] = match[1].substring(0, match[1].length - 1);

			var signature_regex = eval(`/^; ?# ?${match[1]}-signature: ?([\\w\\d ;\\":_\\-#+\\*.,'()]*);?/gm`);
			var signature = signature_regex.exec(text);

			var maybe_discard_regex = eval(`/^; ?# ?${match[1]}-discard/gm`);
			var discard = maybe_discard_regex.exec(text);

			if (discard) {
				// console.log(`Discarding ${match[1]}`);
				continue;
			}

			if (signature) {
				signature = signature[1].trim();
				//console.log(`${match[1]}-signature: ${signature}`);
			} else {
				console.error(`Could not find signature for ${match[1]}`);
				continue;
			}

			if (!signature.startsWith('extern "C"')) {
				signature = `extern "C" ${signature}`;
			}

			if (!signature.endsWith(';')) {
				signature = `${signature};`;
			}

			functions.push(split_c_signature(signature));
		}
	}

	return functions;
}