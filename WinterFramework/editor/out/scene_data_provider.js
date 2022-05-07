"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.SceneDataProvider = void 0;
const vscode_1 = require("vscode");
const JSONBig = require('json-bigint');
class EntityTreeItem extends vscode_1.TreeItem {
    constructor(index, version, archetype, name, collapsibleState) {
        super(name, collapsibleState);
        this.index = index;
        this.version = version;
        this.archetype = archetype;
        this.name = name;
        this.collapsibleState = collapsibleState;
        this.tooltip = `Entity Index: ${this.index}-${this.version}`;
        this.command = {
            title: "Inspect Components",
            command: "test.inspect-components",
            tooltip: "Request an entities components from the framework server",
            arguments: [index, version, archetype]
        };
    }
}
class SceneDataProvider {
    constructor(conn) {
        this.conn = conn;
    }
    getTreeItem(element) {
        return element;
    }
    getChildren(element) {
        // root element is null, list all entities
        if (element === undefined) {
            let request = {
                "kind": 0,
                "data": [-1]
            };
            return new Promise((resolve, reject) => {
                this.conn.write(JSONBig.stringify(request).toString(), (data) => {
                    let entities = [];
                    let json = JSONBig.parse(data);
                    for (let value of json) {
                        let entity = new EntityTreeItem(value["index"], value["version"], value["archetype"], `Entity (i: ${value["index"]} a: ${value["archetype"]})`, vscode_1.TreeItemCollapsibleState.None);
                        entities.push(entity);
                    }
                    resolve(entities);
                });
            });
        }
        return Promise.resolve([]);
    }
}
exports.SceneDataProvider = SceneDataProvider;
//# sourceMappingURL=scene_data_provider.js.map