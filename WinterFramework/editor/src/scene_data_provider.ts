import { resolve } from 'path';
import {TreeItem, TreeDataProvider, TreeItemCollapsibleState, ProviderResult, Command} from 'vscode'
import { ServerConnection } from './simple_server';
const JSONBig = require('json-bigint');

class EntityTreeItem extends TreeItem
{
	constructor(
    	public index: number,
    	public version: number,
    	public archetype: BigInt,
		public name: string,
    	public readonly collapsibleState: TreeItemCollapsibleState)
	{
    	super(name, collapsibleState);

    	this.tooltip = `Entity Index: ${this.index}-${this.version}`;
		
		this.command = {
			title: "Inspect Components",
			command: "test.inspect-components",
			tooltip: "Request an entities components from the framework server",
			arguments: [ index, version, archetype]
		}
  	}
}

export class SceneDataProvider implements TreeDataProvider<EntityTreeItem>
{
	constructor(
		public conn : ServerConnection)
	{}

	getTreeItem(element: EntityTreeItem): TreeItem
	{
		return element;
	}

	getChildren(element?: EntityTreeItem): Thenable<EntityTreeItem[]>
	{
		// root element is null, list all entities

		if (element === undefined)
		{
			let request = { // empty entity
				"kind": 0,
				"data": [-1]
			};

			return new Promise((resolve, reject) => 
			{				
				this.conn.write(JSONBig.stringify(request).toString(), 
					(data: string) => {

						let entities : EntityTreeItem[] = [];
						
						let json = JSONBig.parse(data);
						for (let value of json)
						{
							let entity = new EntityTreeItem(
								value["index"], 
								value["version"],
								value["archetype"],
								`Entity (i: ${value["index"]} a: ${value["archetype"]})`,
								TreeItemCollapsibleState.None);

							entities.push(entity);
						}

						resolve(entities);
					});
			});
		}

		return Promise.resolve([]);
	}
}