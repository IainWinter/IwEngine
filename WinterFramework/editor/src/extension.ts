import { createSocket, Socket } from 'dgram';
import * as vscode from 'vscode';
import { ServerConnection } from './simple_server';
import {SceneDataProvider} from './scene_data_provider';
import {ComponentPanelProvider} from './component_webview';
const JSONBig = require('json-bigint');

const conn = new ServerConnection("localhost", 15001);

export function activate(context: vscode.ExtensionContext)
{
	console.log('Winter Editor is now active');
	
	context.subscriptions.push(
		vscode.commands.registerCommand('test.helloWorld', () =>
		{
		})
	);

	context.subscriptions.push(
		vscode.commands.registerCommand('test.inspect-components', (index, version, archetype) =>
		{
			console.log(index, version, archetype);

			let request = {
				"kind": 1,
				"data": [index, version, archetype]
			};

			conn.write(JSONBig.stringify(request), (data : string) =>
			{
				ComponentPanelProvider.SetComponentStateForNewEntity(data);
			});
		})
	);

	context.subscriptions.push(
		vscode.window.createTreeView(
			'scene-inspector', 
			{ treeDataProvider: new SceneDataProvider(conn)})
	);

	context.subscriptions.push(
        vscode.window.registerWebviewViewProvider("entity-inspector", 
			new ComponentPanelProvider(context.extensionUri)
		)
    );
}

export function deactivate()
{
	console.log('Winter Editor has been deactivated');
}