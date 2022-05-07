// This script will be run within the webview itself
// It cannot access the main VS Code APIs directly.

let pannel = document.getElementById("inputs");

const dim_titles = ['x', 'y', 'z', 'w'];
const dim_colors = ['red', 'green', 'blue', 'orange'];

// !! use _ for spaces !!

// let component_transform = {
// 	name: "Transform",
// 	inputs:[
// 		{name: "Position", kind: "vec3f", value: [0, 0, 0]}, 
// 		{name: "Rotation", kind: "vec3f", value: [0, 0, 0]}, 
// 		{name: "Scale",    kind: "vec3f", value: [0, 0, 0]}
// 	]
// };

// let component_another = {
// 	name: "Another Component",
// 	inputs:[
// 		{name: "Some_prop", kind: "vec1f", value: [0]}, 
// 		{name: "Another_thing", kind: "vec2f", value: [0, 0]}, 
// 		{name: "Something_with_a_long_name",    kind: "vec3f", value: [0, 0, 0]}, 
// 		{name: "vec4",    kind: "vec4f", value: [0, 0, 0, 0]}
// 	]
// };

let components = [];

for (let i = 0; i < i_components.names.length; i++)
{
	let component_name   = i_components.names[i];
	let component_inputs = i_components.components[i];

	let component = {
		name: component_name,
		inputs: []
	};

	for (let input_name in component_inputs) 
	{
		// might be able to figure out type from inputs

		// vec1 is anything that has 1 numbers that are named x
		// vec2 is anything that has 2 numbers that are named x y
		// vec3 is anything that has 3 numbers that are named x y z
		// vec4 is anything that has 4 numbers that are named x y z w

		// not sure how to get the above types, if json serializer forces a decimal for floating point
		// then we could detuce between ints floats and bools

		// strings are if the value is a string

		let input_value = component_inputs[input_name];
		let input_kind = "unknown";

		if (typeof input_value == "string")
		{
			input_kind = "string";
		}

		else
		{
			let dim = Object.keys(input_value).length;
			let bad = false;

			let i = 0;
			for (let special_name in input_value)
			{
				if (special_name != dim_titles[i])
				{
					bad = false;
					break;
				}
				
				i++;
			}

			if (bad)
			{
				input_kind = "unknown";
			}

			else
			{
				input_kind = "vec" + dim;

				switch (typeof Object.values(input_value)[0])
				{
					case "number":
						input_kind += 'f';
						break;
					case "boolean":
						input_kind += 'b';
						break;
					default:
						input_kind = 'unknown';
						break;
				}
			}
		}

		let input = {
			name: input_name.toString(),
			kind: input_kind,
			value: component_inputs[input_name]
		};

		component.inputs.push(input);
	}

	components.push(component);
}

for (let component of components)
{
	let componentDiv   = $('<div class="component"></div>');
	let componentName  = $(`<h3 class="title component_title">${component.name}</h3>`);
	let componentItems = $('<div class="component_items"></div>');

	$("#inputs").append(componentDiv);
	componentDiv.append(componentName);
	componentDiv.append(componentItems);

	for (let input of component.inputs)
	{
		let row   = $('<div class="row"></div>');
		let title = $(`<p class="row_title">${input.name.replaceAll('_', ' ')}</p>`);
		let items = $('<div class="row_item">');

		componentItems.append(row);
		row.append(title);
		row.append(items);

		switch (input.kind)
		{
			case "string":
			{
				let inputHTML = `<input id="${input.name}__${input.kind}_" class="input_listen_for_change input_string" value="${input.value}"/>`;

				items.append(inputHTML);

				break;
			}

			case "vec1b":
			case "vec2b":
			case "vec3b":
			case "vec4b":
			{
				let dim = parseInt(input.kind[3]);

				for (let i = 0; i < dim; i++)
				{
					let inputHTML = `<div class="input_titled_number">
										<input id="${input.name}__${input.kind}_${dim_titles[i]}" class="input_listen_for_change input_titled_number_input" value="${Object.values(input.value)[i]}" type="boolean"/>	
										<p class="input_titled_number_title ${dim_colors[i]}">${dim_titles[i].toUpperCase()}</p>
									</div>`;

					items.append(inputHTML);
				}

				break
			}

			case "vec1f":
			case "vec2f":
			case "vec3f":
			case "vec4f":
			{
				let dim = parseInt(input.kind[3]);

				for (let i = 0; i < dim; i++)
				{
					let inputHTML = `<div class="input_titled_number">
										<input id="${input.name}__${input.kind}_${dim_titles[i]}" class="input_listen_for_change input_titled_number_input" value="${Object.values(input.value)[i]}" type="number" step="0.01"/>	
										<p class="input_titled_number_title ${dim_colors[i]}">${dim_titles[i].toUpperCase()}</p>
									</div>`;

					items.append(inputHTML);
				}

				break
			}
			default:
			{
				let inputHTML = `<p class="input_unknown">Unknown kind: ${input.kind}</p>`;

				items.append(inputHTML);
				break;	
			}
		}
	}

	$(".input_listen_for_change").on("change", (e) => 
	{
		let target = e.target;
		let component_name = $(target).closest(".component").children()[0].innerText;
		let var_name = target.id.substring(0, target.id.indexOf('__'));
		let xyzw_name = target.id.substring(target.id.lastIndexOf('_') + 1);
	
		let name_path = [component_name, var_name]; 

		if (xyzw_name.length > 0)
		{
			name_path.push(xyzw_name);
		}

		vscode.postMessage({
			command: "update-value",
			name: name_path,
			value: target.value,
			type: target.type
		});
	});

	// initial setting
	//componentItems.css({ "max-height": componentItems.height() });

	componentName.on('mousedown', (e) =>
	{
		if (e.button !== 0) return;

		// toggle collapse class

		if (componentItems.hasClass("collapsed")) 
		{
			componentItems.removeClass("collapsed");
		}

		else
		{
			//componentItems.css({ "max-height": componentItems.get(0).scrollHeight });
			componentItems.addClass("collapsed");
		}
	});
}

//$("#inputs").append("Json: " + JSON.stringify(i_components));

function resize_component_input_width()
{
	const max_row_width = 445.0; // for vec4
	const row_padding = 30;

	if (window.innerWidth <= max_row_width + row_padding)
	{
		$(".row_item").css({ "min-width": "" });
	}

	else
	{
		let minWidthForComponentItemRow = 100 * (max_row_width) / window.innerWidth;
		$(".row_item").css({ "min-width": minWidthForComponentItemRow + "%" });
	}
}

// this only works for sliders
function component_string_input_changed(new_value)
{
	let component_name = g_obj_dragged.dom.parentElement.parentElement.parentElement.parentElement.parentElement.children[0].innerText;
	let var_name = g_obj_dragged.dom.id.substring(0, g_obj_dragged.dom.id.indexOf('__'));
	let xyzw_name = g_obj_dragged.dom.id.substring(g_obj_dragged.dom.id.lastIndexOf('_') + 1);

	vscode.postMessage({
		command: "update-value",
		name: [component_name, var_name, xyzw_name],
		value: new_value
	});
}

$(window).on('resize', () =>
{
	resize_component_input_width();
});

resize_component_input_width();