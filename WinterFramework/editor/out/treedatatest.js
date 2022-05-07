"use strict";
// import * as vscode from 'vscode';
// import * as fs from 'fs';
// import * as path from 'path';
// class Entity extends vscode.TreeItem
// {
// 	constructor(
//     	public index: number,
//     	public version: number,
// 		public name: string,
//     	public readonly collapsibleState: vscode.TreeItemCollapsibleState)
// 	{
//     	super(name, collapsibleState);
//     	this.tooltip = `Entity Index: ${this.index}-${this.version}`;
//   	}
// }
// export class SceneInspectorProvider implements vscode.TreeDataProvider<Entity>
// {
// 	getTreeItem(element: Entity): vscode.TreeItem
// 	{
// 		return element;
// 	}
// 	getChildren(element?: Entity): vscode.ProviderResult<Entity[]>
// 	{
// 		return [];
// 	}
// }
// export class NodeDependenciesProvider implements vscode.TreeDataProvider<Dependency>
// {
// 	constructor(
// 		private workspaceRoot: string)
// 	{}
// 	getTreeItem(element: Dependency): vscode.TreeItem
// 	{
// 		return element;
// 	}
// 	getChildren(element?: Dependency): Thenable<Dependency[]>
// 	{
// 		console.log("Getting children of ->", this.workspaceRoot);
// 		if (!this.workspaceRoot)
// 		{
// 			vscode.window.showInformationMessage('No dependency in empty workspace');
// 			return Promise.resolve([]);
// 		}
// 		if (element) {
// 			return Promise.resolve(
// 				this.getDepsInPackageJson(path.join(this.workspaceRoot, 'node_modules', element.label, 'package.json'))
// 			);
// 		}
// 		const packageJsonPath = path.join(this.workspaceRoot, 'package.json');
// 		if (fs.existsSync(packageJsonPath))
// 		{
// 			return Promise.resolve(this.getDepsInPackageJson(packageJsonPath));
// 		}
// 		vscode.window.showInformationMessage('Workspace has no package.json');
// 		return Promise.resolve([]);
// 	}
// 	private getDepsInPackageJson(packageJsonPath: string): Dependency[]
// 	{
// 		console.log("Stepped into ->", packageJsonPath);
// 		if (fs.existsSync(packageJsonPath))
// 		{
// 			const packageJson = JSON.parse(fs.readFileSync(packageJsonPath, 'utf-8'));
// 			const toDep = (moduleName: string, version: string): Dependency =>
// 			{
// 				if (fs.existsSync(path.join(this.workspaceRoot, 'node_modules', moduleName)))
// 				{
// 					return new Dependency(moduleName,version, vscode.TreeItemCollapsibleState.Collapsed);
// 				}
// 				return new Dependency(moduleName, version, vscode.TreeItemCollapsibleState.None);
// 			};
// 			const deps = packageJson.dependencies 
// 					   ? Object.keys(packageJson.dependencies)
// 					   		   .map(dep => toDep(dep, packageJson.dependencies[dep]))
// 					   : [];
// 			const devDeps = packageJson.devDependencies
// 						  ? Object.keys(packageJson.devDependencies)
// 						  		  .map(dep => toDep(dep, packageJson.devDependencies[dep]))
// 						  : [];
// 			return deps.concat(devDeps);
// 		}
// 		return [];
// 	}
// }
// class Dependency extends vscode.TreeItem
// {
// 	constructor(
//     	public label: string,
//     	private version: string,
//     	public readonly collapsibleState: vscode.TreeItemCollapsibleState)
// 	{
//     	super(label, collapsibleState);
//     	this.tooltip = `${this.label}-${this.version}`;
//     	this.description = this.version;
//   	}
// }
//# sourceMappingURL=treedatatest.js.map