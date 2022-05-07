"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.deactivate = exports.activate = void 0;
const vscode = require("vscode");
const simple_server_1 = require("./simple_server");
const scene_data_provider_1 = require("./scene_data_provider");
const component_webview_1 = require("./component_webview");
const JSONBig = require('json-bigint');
const conn = new simple_server_1.ServerConnection("localhost", 15001);
function activate(context) {
    console.log('Winter Editor is now active');
    context.subscriptions.push(vscode.commands.registerCommand('test.helloWorld', () => {
    }));
    context.subscriptions.push(vscode.commands.registerCommand('test.inspect-components', (index, version, archetype) => {
        console.log(index, version, archetype);
        let request = {
            "kind": 1,
            "data": [index, version, archetype]
        };
        conn.write(JSONBig.stringify(request), (data) => {
            component_webview_1.ComponentPanelProvider.SetComponentStateForNewEntity(data);
        });
    }));
    context.subscriptions.push(vscode.window.createTreeView('scene-inspector', { treeDataProvider: new scene_data_provider_1.SceneDataProvider(conn) }));
    context.subscriptions.push(vscode.window.registerWebviewViewProvider("entity-inspector", new component_webview_1.ComponentPanelProvider(context.extensionUri)));
}
exports.activate = activate;
function deactivate() {
    console.log('Winter Editor has been deactivated');
}
exports.deactivate = deactivate;
//# sourceMappingURL=extension.js.map