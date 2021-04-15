"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.TextGPS = void 0;
const { TextGps } = require("bindings")('search');
const status_1 = require("./status");
class TextGPS {
    constructor() {
        this.dbFile = '';
        this.gps = new TextGps();
    }
    open(path) {
        const rc = this.gps.open(path);
        this.dbFile = path;
        if (rc != 0) {
            const error = new Error(status_1.SqliteCode[rc]);
            console.error(error.message, error.stack);
        }
    }
    close() {
        const rc = this.gps.close();
        if (rc != 0) {
            const error = new Error(status_1.SqliteCode[rc]);
            console.error(error.message, error.stack);
        }
    }
    commit() {
        const rc = this.gps.commit();
        if (rc != 0) {
            const error = new Error(status_1.SqliteCode[rc]);
            console.error(error.message, error.stack);
        }
    }
    execute(sql, bindings) {
        const rc = this.gps.execute(sql, bindings);
        if (rc != 0) {
            const error = new Error(status_1.SqliteCode[rc]);
            console.error(error.message, error.stack);
        }
    }
    stopSearching(stop) {
        this.gps.stopSearching(stop);
    }
    setCase(value) {
        this.gps.setCase(value);
    }
    setTop(top) {
        this.gps.setTop(top);
    }
    search(queries, logics) {
        this.gps.search(queries, logics);
    }
    getResult() {
        return this.gps.getResult();
    }
    getDbPath() {
        return this.dbFile;
    }
}
exports.TextGPS = TextGPS;
