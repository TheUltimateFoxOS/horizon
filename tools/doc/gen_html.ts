var doc_files: {
	html_name: string;
	path: string;
}[] = [];

var template = `
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">
    <link rel="stylesheet" href="https://theultimatefoxos.dev/style.css">
    <link rel="stylesheet" href="https://fonts.googleapis.com/css2?family=Noto+Sans+JP&display=swap">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
    <script src="https://theultimatefoxos.dev/main.js" defer></script>
    <title>FoxOS - Docs</title>
  </head>
  <body>
    <div class="fox-bar" id="fox-bar">
      <a href="/">Home</a>
      <a href="/about/">About us</a>
      <a href="/FoxOS-kernel/">Docs</a>
      <a href="/modules/">Kernel modules</a>
      <a href="https://github.com/TheUltimateFoxOS">GitHub</a>
      <a href="javascript:void(0);" class="icon" onclick="nav_bar_expand()">
        <i class="fa fa-bars"></i>
      </a>
    </div>
    
    <header class="fox-header" style="padding:50px 16px">
      <h1 class="fox-header-title">FoxOS</h1>
      <p class="fox-header-subtitle">Welcome to the FoxOS website!</p>
    </header>
    
    <div class="fox-text">
`;

var index_template = `
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">
    <link rel="stylesheet" href="https://theultimatefoxos.dev/style.css">
    <link rel="stylesheet" href="https://fonts.googleapis.com/css2?family=Noto+Sans+JP&display=swap">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
    <script src="https://theultimatefoxos.dev/main.js" defer></script>
    <title>FoxOS - Docs</title>
  </head>
  <body>
    <div class="fox-bar" id="fox-bar">
      <a href="/">Home</a>
      <a href="/about/">About us</a>
      <a href="/FoxOS-kernel/">Docs</a>
      <a href="/modules/">Kernel modules</a>
      <a href="https://github.com/TheUltimateFoxOS">GitHub</a>
      <a href="javascript:void(0);" class="icon" onclick="nav_bar_expand()">
        <i class="fa fa-bars"></i>
      </a>
    </div>
    
    <header class="fox-header" style="padding:50px 16px">
      <h1 class="fox-header-title">FoxOS</h1>
      <p class="fox-header-subtitle">Welcome to the FoxOS website!</p>
    </header>
    
    <div class="fox-text">
      <ul class="fox-a-nodecoration fox-ul">
`;

function gen_function(func: {
	return_type: string;
	class_name: string;
	function_name: string;
	attributes: string | undefined;
	params: string;
	signature: string;
	description: string;
}) {
	var html = "";
	html = "      <div>\n"
	html = html + "        <h2>" + func.function_name + "</h2>\n"
	html = html + "        <b>Signature:</b> <code>" + func.signature + "</code><br/>\n"
	
	if (func.class_name != "") {
		html = html + "        <b>Class/Namespace:</b> <code>" + func.class_name + "</code><br/>\n"
	}

	if (func.attributes != "") {
		html = html + "        <b>Attributes:</b> <code>" + func.attributes + "</code><br/>\n"
	}

	html = html + "        <b>Description:</b> " + func.description + "<br/>\n"

	if (func.params != "") {
		html = html + "        <br/>\n"
		html = html + "        <b>Parameters:</b><br/>\n"
		html = html + "        <ul>\n"
		for (let param_for of func.params.split(",")) {
			html = html + "          <li><code>" + param_for.trim() + "</code></li>\n"
		}

		html = html + "        </ul>\n"
	}
	if (func.return_type != "") {
		html = html + "        <br/>\n"
		html = html + "        <b>Return:</b> <code>" + func.return_type + "</code>\n"
	}
	html = html + "      </div>\n"
	html = html + "      <hr>\n"

	return html;
}

export function gen_html(save_in: string, path: string, funcs: {
	return_type: string;
	class_name: string;
	function_name: string;
	attributes: string | undefined;
	params: string;
	signature: string;
	description: string;
}[]) {
	doc_files.push({
		html_name: save_in.split("/").pop() as string,
		path: path
	});

	var html = "";
	for (var func of funcs) {
		html += gen_function(func);
	}

	Deno.writeTextFileSync(save_in, template + html + "    </div>\n  </body>\n</html>");
}

interface file_hierarchy {
	next: file_hierarchy[] | undefined;
	folder_name: string;
	files: {
		html_name: string;
		file_name: string;
	}[];
};

function gen_hierarchy(hierarchy: file_hierarchy[], level: number) {
	// recursively generate the html for the hierarchy
	var html = "";
	for (var folder of hierarchy) {
		html += "      <li>\n";
		html += "        <p>" + folder.folder_name + "</p>\n";
		html += "        <ul>\n";
		if (folder.next != undefined) {
			html += gen_hierarchy(folder.next, level + 1);
		}
		for (var file of folder.files) {
			html += "          <li><a href=\"" + file.html_name + "\">" + file.file_name + "</a></li>\n";
		}
		html += "        </ul>\n";
		html += "      </li>\n";
	}
	return html;
}

export function gen_index(save_in: string) {
	var hierarchy: file_hierarchy = {
		next: undefined,
		folder_name: "horizon",
		files: []
	};

	for (var func of doc_files) {
		var path = func.path.split("/");
		var current = hierarchy;
		for (var i = 0; i < path.length; i++) {
			var folder_name = path[i];
			if (i == path.length - 1) {
				current.files.push({
					html_name: func.html_name,
					file_name: func.path.split("/").pop() as string
				});
			} else {
				var next = current.next;
				if (next == undefined) {
					next = [];
					current.next = next;
				}
				var found = false;
				for (var j = 0; j < next.length; j++) {
					if (next[j].folder_name == folder_name) {
						found = true;
						current = next[j];
						break;
					}
				}
				if (!found) {
					var new_folder = {
						next: undefined,
						folder_name: folder_name,
						files: []
					};
					next.push(new_folder);
					current = new_folder;
				}
			}
		}
	}


	var html = index_template;
	html += "      <ul>\n";
	html += gen_hierarchy([hierarchy], 0);
	html += "      </ul>\n";
	html += "    </div>\n  </body>\n</html>";
	Deno.writeTextFileSync(save_in, html);
	Deno.writeTextFileSync(save_in.replace(".html", ".json"), JSON.stringify(hierarchy, null, "\t"));
}