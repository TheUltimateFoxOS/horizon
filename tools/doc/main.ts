import getFiles from "https://deno.land/x/getfiles/mod.ts";
import { get_documentation } from "./doc.ts";
import { gen_html, gen_index } from "./gen_html.ts";

const files = getFiles({
	root: './core/',
	exclude: ['.git'],
});

var docs_file = Deno.readTextFileSync("./docs.txt");

for (const file of files) {
	try {
		if (file.ext == "h") {
			continue;
		}

		var doc = get_documentation(file.path, file.ext);

		var new_doc: {
			return_type: string;
			class_name: string;
			function_name: string;
			attributes: string | undefined;
			params: string;
			signature: string;
			description: string;
		}[] = [];

		for (var d of doc) {
			var doc_regex = eval(`/# ?${file.name}:${d.function_name}: ?([\\w\\d ;:_\\-#+\\*.,'/]*)/g`);
			var doc_match = doc_regex.exec(docs_file);
			if (doc_match) {
				new_doc.push({
					...d,
					...{
						description: doc_match[1],
					}
				});
			} else {
				new_doc.push({
					...d,
					...{
						description: "No description",
					}
				});
				console.error(`Could not find doc for ${file.name}:${d.function_name}`);
			}
		}

		gen_html(`./docs/${file.name}.html`, new_doc);
	} catch (e) {
		console.error(e);
		continue;
	}
}

gen_index("./docs/index.html");