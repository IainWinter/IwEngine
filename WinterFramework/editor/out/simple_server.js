"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.ServerConnection = void 0;
const net = require("net");
const MESSAGE_HEADER_SIZE = 8;
class Message {
    constructor(id, size, text) {
        this.id = id;
        this.size = size;
        this.text = text;
    }
}
class ServerConnection {
    constructor(m_host, m_port) {
        this.m_host = m_host;
        this.m_port = m_port;
        this.m_client = net.createConnection({ host: m_host, port: m_port });
        this.m_client.on('data', (data) => { this.recieve(data); });
        this.m_client.on('connect', () => { console.log('connected to server'); });
        this.m_client.on('end', () => { console.log('disconnected from server'); });
        this.m_client.setTimeout(2e3, () => { console.log(`failed to connect to server, check that ${m_host} is listening on port ${m_port}`); });
        this.m_cur = undefined;
        this.m_nextid = 0;
        this.m_callbacks = new Map();
    }
    write(data, then) {
        this.m_nextid += 1;
        let msg = new Message(this.m_nextid, data.length, data);
        if (then !== undefined) {
            this.m_callbacks.set(msg.id, then);
        }
        let arr = new ArrayBuffer(MESSAGE_HEADER_SIZE);
        let view = new DataView(arr);
        view.setUint32(0, msg.id, true);
        view.setUint32(4, msg.size, true);
        this.m_client.write(Buffer.from(arr));
        this.m_client.write(msg.text);
    }
    recieve(data) {
        let offset_from_header = 0;
        if (this.m_cur === undefined) // header
         {
            this.m_cur = new Message(data.readUInt32LE(0), data.readUInt32LE(4), "");
            offset_from_header = MESSAGE_HEADER_SIZE;
        }
        if (data.length - offset_from_header >= this.m_cur.size) // body
         {
            this.m_cur.text = data.toString('utf8', offset_from_header);
            this.m_callbacks.get(this.m_cur.id)?.call(undefined, this.m_cur.text);
            // reset for next read
            this.m_cur = undefined;
        }
    }
}
exports.ServerConnection = ServerConnection;
//# sourceMappingURL=simple_server.js.map