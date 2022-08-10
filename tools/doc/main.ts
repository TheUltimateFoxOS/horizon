import getFiles from "https://deno.land/x/getfiles/mod.ts";
import { get_documentation } from "./doc.ts";
import { gen_html, gen_index } from "./gen_html.ts";
import { notify } from "./notify.ts";

function print_progress(progress: number) {
	const toolbar_width = 80;

	// setup toolbar
	var empty_bar = " ".repeat(toolbar_width).split("");

	for (var i = 0; i < Math.round((toolbar_width) / 100 * progress); i++) {
		empty_bar[i] = "-";
	}
	empty_bar[i] = ">";

	console.log(`${Math.round(progress)}% [${empty_bar.join("")}]`);
}

function main() {
	const files = getFiles({
		root: './',
	});

	var docs_file = Deno.readTextFileSync("./docs.txt");

	var ignore_ext = [];
	var exclude_regex = eval(`/# ?ignore-ext:\\s*(.*)\\s*/g`);
	var exclude_regex_result = exclude_regex.exec(docs_file);
	if (exclude_regex_result) {
		ignore_ext = exclude_regex_result[1].split(",").map(function (ext: string) {
			return ext.trim();
		});
	}

	ignore_ext.push("");

	var num_functions = 0;
	var num_functions_documented = 0;

	for (const file of files) {
		if (file.path.includes(".git")) {
			continue;
		}
		
		try {
			if (ignore_ext.includes(file.ext)) {
				continue;
			}

			var exclude_regex = eval(`/# ?exclude:${file.path.replaceAll("/", "_")}/g`);
			var exclude_match = exclude_regex.exec(docs_file); 
			if (exclude_match) {
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
				num_functions++;
				var doc_regex = eval(`/# ?${file.path.replaceAll("/", "_")}:${d.function_name.replaceAll("[", "\\[").replaceAll("]", "\\]")}: ?([\\w\\d ;:_\\-#+\\*.,'/]*)/g`);
				var doc_match = doc_regex.exec(docs_file);
				if (doc_match) {
					num_functions_documented++;
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
					console.error(`Could not find doc for ${file.path.replaceAll("/", "_")}:${d.function_name}`);
				}
			}

			gen_html(`./docs/${file.path.replaceAll("/", "_")}.html`, file.path, new_doc);
		} catch (e) {
			console.error(e);
			continue;
		}
	}

	gen_index("./docs/index.html");

	console.log("Documented " + num_functions + " functions from witch " + num_functions_documented + " have a description. This are " + Math.round(num_functions_documented / num_functions * 100) + "% of the functions.")
	print_progress((num_functions_documented / num_functions) * 100);

	if (Deno.args.length == 1) {
		console.log("Notifying...");
		notify(Deno.args[0], num_functions_documented, num_functions);
	}
}

main();