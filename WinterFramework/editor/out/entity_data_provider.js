"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.EntityDataProvider = void 0;
const vscode_1 = require("vscode");
class EntityTreeItem extends vscode_1.TreeItem {
    constructor(index, version, name, collapsibleState) {
        super(name, collapsibleState);
        this.index = index;
        this.version = version;
        this.name = name;
        this.collapsibleState = collapsibleState;
        this.tooltip = `Entity Index: ${this.index}-${this.version}`;
    }
}
class EntityDataProvider {
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
                "data": [-1, -1, -1]
            };
            return new Promise((resolve, reject) => {
                this.conn.write(JSON.stringify(request).toString(), (data) => {
                    let entities = [];
                    let json = JSON.parse(data);
                    for (let value of json) {
                        let entity = new EntityTreeItem(value["index"], value["version"], "Entity", vscode_1.TreeItemCollapsibleState.None);
                        entities.push(entity);
                    }
                    resolve(entities);
                });
            });
        }
        return Promise.resolve([]);
    }
}
exports.EntityDataProvider = EntityDataProvider;
//# sourceMappingURL=entity_data_provider.js.map