let g_obj_dragged, 
	g_mouse_x = 0, 
	g_mouse_y = 0;

function g_update_mouse(e)
{
	g_mouse_x = e.pageX;
	g_mouse_y = e.pageY;
}

function g_update_dragged(e)
{
	let dist_x = g_mouse_x - g_obj_dragged.start_x;
	let dist_y = g_mouse_y - g_obj_dragged.start_y;

	let step = 100; // e.shiftKey ? 1000 : 
	let offset = (dist_x - dist_y) / step;
	let value = (g_obj_dragged.initial + offset).toFixed(2);
	
	g_obj_dragged.dom.val(value + "").trigger("change");
}

const vscode = acquireVsCodeApi();

// // this only works for sliders
// function component_xyzw_input_changed(new_value)
// {
// 	let component_name = g_obj_dragged.dom.parentElement.parentElement.parentElement.parentElement.parentElement.children[0].innerText;
// 	let var_name = g_obj_dragged.dom.id.substring(0, g_obj_dragged.dom.id.indexOf('__'));
// 	let xyzw_name = g_obj_dragged.dom.id.substring(g_obj_dragged.dom.id.lastIndexOf('_') + 1);

// 	vscode.postMessage({
// 		command: "update-value",
// 		name: [component_name, var_name, xyzw_name],
// 		value: new_value
// 	});
// }

function dragging_continue(e)
{
	g_update_dragged(e);
	g_update_mouse(e);
}

function dragging_end(e)
{
	$(document).unbind("mousemove", dragging_continue);
	$(document).unbind("mouseup", dragging_end);
}

function dragging_start(e)
{
	g_update_mouse(e);

	let input = $(this).find(".input_titled_number_input");
	let value = parseFloat(input.val());

	if (isNaN(value) || value === "")
	{
		value = 0.0;
	}

	g_obj_dragged = {
		dom: input,
		initial: value,
		start_x: g_mouse_x,
		start_y: g_mouse_y,
	};

	g_update_dragged(e);
	
	$(document).bind("mousemove", dragging_continue);
	$(document).bind("mouseup",   dragging_end);
}

$(".input_titled_number").bind("mousedown", dragging_start);