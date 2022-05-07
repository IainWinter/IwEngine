"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.ComponentPanelProvider = void 0;
const vscode = require("vscode");
const JSONBig = require('json-bigint');
class ComponentPanelProvider {
    constructor(exturi) {
        this.exturi = exturi;
        ComponentPanelProvider.exturi = exturi;
    }
    // mem leak bc no dispose?
    resolveWebviewView(view, context, token) {
        view.webview.options = { enableScripts: true };
        view.webview.onDidReceiveMessage((message) => {
            switch (message.command) {
                case "update-value":
                    {
                        ComponentPanelProvider.SetComponentValue(message.name, message.value, message.type);
                        break;
                    }
            }
        });
        ComponentPanelProvider.view = view;
        ComponentPanelProvider.UpdateHtmlForWebview();
    }
    static SetComponentStateForNewEntity(components) {
        ComponentPanelProvider.component_state = JSONBig.parse(components);
        ComponentPanelProvider.UpdateHtmlForWebview();
    }
    static SetComponentValue(component_name_tree, value, value_type) {
        let state = ComponentPanelProvider.component_state;
        let current = state.components[state.names.indexOf(component_name_tree[0])];
        let names = Object.keys(current);
        for (let i = 1; i < component_name_tree.length - 1; i++) {
            let index = names.indexOf(component_name_tree[i]);
            current = Object.values(current)[index];
        }
        let final_name = component_name_tree[component_name_tree.length - 1];
        switch (value_type) {
            case 'number':
                current[final_name] = Number(value);
                break;
            case 'boolean':
                current[final_name] = Boolean(value);
                break;
            default:
                current[final_name] = value;
                break;
        }
    }
    static UpdateHtmlForWebview() {
        let json_string = "[]";
        if (ComponentPanelProvider.component_state !== undefined) {
            json_string = JSONBig.stringify(ComponentPanelProvider.component_state);
        }
        let view = ComponentPanelProvider.view.webview;
        let uri = ComponentPanelProvider.exturi;
        // should all these to lists
        const script_main = view.asWebviewUri(vscode.Uri.joinPath(uri, 'media', 'component_view.js'));
        const script_main2 = view.asWebviewUri(vscode.Uri.joinPath(uri, 'media', 'component_slider.js'));
        const script_jquery = view.asWebviewUri(vscode.Uri.joinPath(uri, 'media', 'jquery.min.js'));
        const style_reset = view.asWebviewUri(vscode.Uri.joinPath(uri, 'media', 'reset.css'));
        const style_main = view.asWebviewUri(vscode.Uri.joinPath(uri, 'media', 'vscode.css'));
        const style_custom = view.asWebviewUri(vscode.Uri.joinPath(uri, 'media', 'component_view.css'));
        const nonce = getNonce();
        view.html = `<!DOCTYPE html>
			<html lang="en">
			<head>
				<meta charset="UTF-8">
				<!--
					Use a content security policy to only allow loading images from https or from our extension directory,
					and only allow scripts that have a specific nonce.
				-->
				<meta http-equiv="Content-Security-Policy" content="default-src 'none'; style-src ${view.cspSource}; img-src ${view.cspSource} https:; script-src 'nonce-${nonce}';">
				<meta name="viewport" content="width=device-width, initial-scale=1.0">
				<link href="${style_reset}" rel="stylesheet">
				<link href="${style_main}" rel="stylesheet">
				<link href="${style_custom}" rel="stylesheet">
			</head>
			<body>
				<div id='inputs'></div>
				<script nonce="${nonce}">let i_components = JSON.parse('${json_string}');</script>
				<script nonce="${nonce}" src="${script_jquery}"></script>
				<script nonce="${nonce}" src="${script_main}"></script>
				<script nonce="${nonce}" src="${script_main2}"></script>
			</body>
			</html>`;
    }
}
exports.ComponentPanelProvider = ComponentPanelProvider;
function getNonce() {
    let text = '';
    const possible = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
    for (let i = 0; i < 32; i++) {
        text += possible.charAt(Math.floor(Math.random() * possible.length));
    }
    return text;
}
//# sourceMappingURL=component_webview.js.map